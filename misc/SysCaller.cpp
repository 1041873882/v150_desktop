
#include "sys.h"
#include "wTalk.h"
#include "wMain.h"
#include "wNtTalk.h"
#include "wNtMain.h"
#include "d600.h"
#include "SysSound.h"
#include "SysCaller.h"

SysCaller sCaller;

SysCaller::SysCaller()
{
	m_mode = NONE;
	m_proxy = 0;
	m_result = 0;
	m_window = 0;
	m_mute[0] = m_mute[1] = 0;
	for(int i=0; i<MAX_HOST2ID_SZ; i++)
		m_h2id[i] = NULL;
	m_h2id_mark = 0;

	sip.url = NULL;
	d600.host = NULL;
	d600.ip = NULL;

	pthread_mutex_init(&m_mutex, NULL);
}

SysCaller::~SysCaller()
{
}

void SysCaller::query(const char *id)
{
	if (id == NULL)
		return;

	AutoMutex mutex(&m_mutex);

	m_mode = QUERY;
	gettimeofday(&m_tv, NULL);
	strcpy(m_id, id);

	sip.url = NULL;
	d600.host = NULL;
	d600.ip = NULL;

	dmsg req;
	dxml p;
	p.setText("/params/id", id);
	req.request("/talk/sip/query", p.data());
}

void SysCaller::start(const char *url)
{
	AutoMutex mutex(&m_mutex);
	m_mode = CALLING;
	gettimeofday(&m_tv, NULL);
	m_result = 0;
	dmsg req;
	dxml p;
	p.setText("/params/url", url);
	req.request("/talk/sip/call", p.data());
}

void SysCaller::calling(const char *id)
{
	AutoMutex mutex(&m_mutex);
	if (!m_window) {
		m_mode = CALLING;
		gettimeofday(&m_tv, NULL);
		sound.ringback();
	}
	if (id) {
		strcpy(m_id, id);
		m_h2id_mark = 1;
	}
	m_window = 1;
}

void SysCaller::ringing(const char *id)
{
	AutoMutex mutex(&m_mutex);
	if (!m_window) {
		m_mode = RINGING;
		gettimeofday(&m_tv, NULL);
		if (!sys.mute.m_enable) {
			sound.stop();
			sound.ringing();
		}
	}
	if (id) {
		strcpy(m_id, id);
		m_h2id_mark = 1;
	}
	m_window = 1;
}

void SysCaller::play(const char *id)
{
	if (m_window)
		usleep(50*1000);
	AutoMutex mutex(&m_mutex);
	sound.stop();
	if (m_mode != TALKING) {
		if (id) strcpy(m_id, id);
		m_mode = TALKING;
	}
}

void SysCaller::answer(void)
{
	AutoMutex mutex(&m_mutex);
	sound.stop();
	if (m_mode == RINGING) {
		m_mode = TALKING;
		dmsg req;
		req.request("/talk/start", NULL);
	}
}

void SysCaller::stop(void)
{
	AutoMutex mutex(&m_mutex);

	if (m_mode > QUERY) {
		dmsg req;
		req.request("/talk/stop", NULL);
	}
	if (m_mode != NONE)
		sound.stop();

	m_mode = NONE;
	m_mute[0] = m_mute[1] = 0;
	for(int i=0; i<MAX_HOST2ID_SZ; i++) {
		if (m_h2id[i] != NULL) {
			delete m_h2id[i];
			m_h2id[i] = NULL;
		}
	}
	m_window = 2;
}

void SysCaller::unlock(void)
{
	dmsg req;
	req.request("/talk/open", NULL);

	if (sys.dtmf.enable()) {
		dxml p;
		p.setText("/params/dtmf", sys.dtmf.data());
		req.request("/talk/send_dtmf", p.data());
	}
}

void SysCaller::mute(int index, int enable)
{
	if (index < 2) {
		m_mute[index] = enable;
		dmsg req;
		dxml p;
		p.setInt("/params/mode", index);
		p.setInt("/params/enable", enable);
		req.request("/talk/mute", p.data());
	}
}

void SysCaller::m700(const char *url)
{
	AutoMutex mutex(&m_mutex);
	m_mode = CALLING;
	gettimeofday(&m_tv, NULL);
	m_result = 0;
	dmsg req;
	dxml p;
	p.setText("/params/url", url);
	p.setInt("/params/type", 1);
	req.request("/talk/sip/call", p.data());
}

void SysCaller::q600(const char *id)
{
	AutoMutex mutex(&m_mutex);

	m_mode = QUERY;
	gettimeofday(&m_tv, NULL);

	d600.host = NULL;
	d600.ip = NULL;
	c600.do_query(id);

	dmsg req;
	dxml p;
	p.setText("/params/name", id);
	req.request("/talk/device/query", p.data());
}

void SysCaller::s600(const char *id, const char *ip)
{
	AutoMutex mutex(&m_mutex);
	m_mode = CALLING;
	gettimeofday(&m_tv, NULL);
	m_result = 0;
	dmsg req;
	dxml p;
	p.setText("/params/name", id);
	p.setText("/params/ip", ip);
	req.request("/talk/call", p.data());
}

void SysCaller::m600(const char *id, const char *ip)
{
	AutoMutex mutex(&m_mutex);
	m_mode = CALLING;
	gettimeofday(&m_tv, NULL);
	m_result = 0;
	dxml p;
	dmsg req;
	p.setText("/params/name", id);
	p.setText("/params/ip", ip);
	req.request("/talk/monitor", p.data());
}

void SysCaller::host2id(host2id_t *id)
{
	for(int i=0; i<MAX_HOST2ID_SZ; i++) {
		if (m_h2id[i] && !strcmp(id->host, m_h2id[i]->host))
			return;
	}
	for(int i=0; i<MAX_HOST2ID_SZ; i++) {
		if (m_h2id[i] == NULL) {
			m_h2id[i] = new host2id_t;
			memcpy(m_h2id[i], id, sizeof(host2id_t));
			break;
		}
	}
	m_h2id_mark = 1;
}

host2id_t *SysCaller::host2id(const char *id)
{
	if (id) {
		for(int i=0; i<MAX_HOST2ID_SZ; i++) {
			if (m_h2id[i] && !strcmp(id, m_h2id[i]->host))
				return m_h2id[i];
		}
	}
	return NULL;
}

void SysCaller::logger(int mode)
{
	dmsg req;
	dxml p;
	p.setText("/params/event_url", "/msg/talk/logger");
	p.setText("/params/to", m_id);
	p.setInt("/params/mode", mode);
	req.request("/talk/center/to", p.data()); //700Э�������־
}

#include "SysSecurity.h"
void SysCaller::slaves(dxml &p)
{
	for(int i=0; i<MAX_SLAVE_SZ; i++) {
		char s[128];
		sprintf(s, "/params/url%d", i);
		std::string url = p.getText(s, "");
		if (url != m_slaves[i]) {
			m_slaves[i] = url;
			if (!sys.talk.dcode() && url.length() > 4) {
				LOGI("SysCaller::slaves sync %s\n", url.c_str());
				sSecurity.sync_cfg(url.c_str());
			}
				
		}
	}
}

//�˺�����mEvent���̵߳��ã�������������ʱ�������ز���
void SysCaller::ui_process(void)
{
	AutoMutex mutex(&m_mutex);
	if (m_window) {
		if (m_window == 1) {
			if (SysTalk == NULL) {
				if (sys.m_limit == 220) {
					wNtTalk *w = new wNtTalk();
					w->show();
				} else {
					wTalk *w = new wTalk();
					w->show();
				}
			}
		} else if (m_window == 2) {
			if (SysTalk) {
				if (sys.m_limit == 220) {
					wNtMain *w = new wNtMain();
					w->show();
				} else {
					wMain *w = new wMain();
					w->show();
				}
			}
		}
		m_window = 0;
	}
}
