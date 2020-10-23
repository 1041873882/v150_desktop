
#include "dxml.h"
#include "dmsg.h"
#include "SysCaller.h"
#include "SysLogger.h"
#include "wTalk.h"

mWindow *SysTalk = NULL;

static const char *dtmf[] = {
	"1", "2", "3",
	"4", "5", "6",
	"7", "8", "9",
	"*", "0", "#"
};

wTalk::wTalk() : mWindow("talk")
{
	SysTalk = this;
	fb.enable(1);

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_answer.setParent(this);
	if (sCaller.mode() == sCaller.CALLING) {
		m_type = sLogger.talk.OUT;
		m_answer_st = 1;
		m_answer.load(m_style, "stop");
	} else {
		m_type = sLogger.talk.NA;
		m_answer_st = 0;
		m_answer.load(m_style, "answer");
	}

	m_unlock.setParent(this);
	m_unlock.load(m_style, "unlock");
	m_mic.setParent(this);
	m_mic.load(m_style, "mic");
	m_keyboard.setParent(this);
	m_keyboard.load(m_style, "keyboard");

	m_host.setParent(this);
	m_host.load(m_style, "host");
	m_host.setText(sCaller.m_id);

	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

	m_dtmf_text.setParent(this);
	m_dtmf_text.load(m_style, "dtmf_text");
	m_dtmf_text.hide();

	int x_offset = m_style.getInt("/style/dtmf/x_offset", 0);
	int y_offset = m_style.getInt("/style/dtmf/y_offset", 0);
	for(int i=0; i<12; i++) {
		m_dtmf[i].setParent(this);
		m_dtmf[i].load(m_style, "dtmf");
		m_dtmf[i].move(m_dtmf[i].x()+(i%3)*x_offset, m_dtmf[i].y()+(i/3)*y_offset);
		m_dtmf[i].setText(dtmf[i]);
		m_dtmf[i].hide();
	}

	m_vo_st = 0;
	m_ts = time(NULL);
	m_jpeg = time(NULL);
	m_busy_ts = 0;
}

wTalk::~wTalk()
{
	SysTalk = NULL;
	fb.osd(0, 0, 0, 0);

	sLogger.talk.insert(m_type);
}

void wTalk::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEventJpeg) {
		const char *url = (const char *)e->wParam;
		this->jpeg(url);
	} else if (e->type == mEventBusy) {
		const char *id = (const char *)e->wParam;
		char s[128], host[64];
		this->host2id(id, host);
		sprintf(s, "%s %s", host, m_style.getText("/style/busy"));
		m_host.setText(s);
		m_busy_ts = time(NULL);
		sCaller.m_h2id_mark = 0;
	} else if (e->type == mEventHook) {
		int st = e->wParam;
		if (st == 0) { //提机
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "stop");
				sCaller.answer();
			}
		} else {
			if (m_answer_st)
				sCaller.stop();
		}
	} else if (e->type == mEvent::KeyPress) {
		if (e->wParam == KEY_M_N3) {
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "stop");
				sCaller.answer();
			} else {
				sCaller.stop();
			}
		} else if (e->wParam == KEY_M_UNLOCK) {
			sCaller.unlock();
		}
	} else if (e->type == mEvent::TouchEnd) {
		if (fb.m_osd_alpha) {
			for(int i=0; i<12; i++) {
				if (e->wParam == m_dtmf[i].id()) {
					dxml p;
					dmsg req;
					p.setText("/params/dtmf", dtmf[i]);
					req.request("/talk/send_dtmf", p.data());

					if (m_dtmf_key.length() > 18)
						m_dtmf_key = "";
					m_dtmf_key += dtmf[i];
					m_dtmf_text.setText(m_dtmf_key);
					break;
				}
			}
		}
		if (e->wParam == m_keyboard.id()) {
			if (fb.m_osd_alpha == 0) {
				fb.m_osd_alpha = 0xA0;
				m_dtmf_text.show();
				for(int i=0; i<12; i++) {
					m_dtmf[i].show();
				}
			} else {
				fb.m_osd_alpha = 0;
				m_dtmf_text.hide();
				for(int i=0; i<12; i++) {
					m_dtmf[i].hide();
				}
			}
		} else if (e->wParam == m_answer.id()) {
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "stop");
				sCaller.answer();
			} else {
				sCaller.stop();
			}
		} else if (e->wParam == m_unlock.id()) {
			if (!this->doElevVisit(sCaller.m_id))
				sCaller.unlock();
		} else if (e->wParam == m_mic.id()) {
			if (sCaller.mode() == sCaller.TALKING) {
				if (sCaller.m_mute[0]) {
					sCaller.mute(0, 0);
					m_mic.load(m_style, "mic");
				} else {
					sCaller.mute(0, 1);
					m_mic.load(m_style, "mic2");
				}
			}
		}
	}
}

void wTalk::doTimer(void)
{
	fb.enable(1);

	if (sCaller.m_h2id_mark) {
		sCaller.m_h2id_mark = 0;
		char s[128];
		this->host2id(sCaller.m_id, s);
		m_host.setText(s);
	}
	if (m_busy_ts && labs(time(NULL)-m_busy_ts) >= 5) {
		m_busy_ts = 0;
		sCaller.m_h2id_mark = 1;
	}

	if (!m_vo_st) {
		m_vo_st = 1;
		int x = m_osd.x()+4;
		int y = m_osd.y()+4;
		int w = m_osd.width()-8;
		int h = m_osd.height()-8;

		dmsg req;
		dxml p;
		p.setInt("/params/x", x);
		p.setInt("/params/y", y);
		p.setInt("/params/width", w);
		p.setInt("/params/height", h);
		req.request("/talk/vo_start", p.data());
		fb.osd(x, y, w, h);
	}

	if (m_jpeg && labs(time(NULL)-m_jpeg) > 2) {
		m_jpeg = 0;
		char s[128];
		sprintf(s, "%s/%ld.jpg", sLogger.jpeg_dir(), time(NULL));
		dxml p;
		dmsg req;
		p.setText("/params/url", s);
		req.request("/talk/snapshot", p.data());

		if (sys.talk.answer()) {
			//自动接听
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "stop");
				sCaller.answer();
			}
		}
	}

	if (labs(time(NULL)-m_ts) > 2) {
		m_ts = time(NULL);
		dmsg req;
		dxml p;
		req.request("/talk/active", NULL);
		p.load(req.body());
		if (!p.getInt("/params/data", 0)) {
			sCaller.stop();
		}
	}

	mWindow::doTimer();
}

void wTalk::host2id(const char *sid, char *s)
{
	if (sid == NULL || s == NULL)
		return;

	strcpy(s, sid);
	host2id_t *id = sCaller.host2id(s);
	if (id != NULL) {
		if (id->build == 0 && id->unit == 0 && id->floor == 0) {
			sprintf(s, "%s%02d", m_style.getText("/style/center"), id->family);
		} else if (id->build && id->floor == 99) {
			sprintf(s, "%s%02d", m_style.getText("/style/panel"), id->family);
		} else if (id->build == 0 && id->floor == 99) {
			sprintf(s, "%s%02d", m_style.getText("/style/wall"), id->family);
		} else if (strlen(id->host) > 9) { //小门口机
			char s2[32];
			memcpy(s2, id->host, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", m_style.getText("/style/person"), atoi(s2));
		} else {
			sprintf(s, "%d-%d-%d%02d", id->build, id->unit, id->floor, id->family);
		}
	} else {
		if (sid[0] == 'S') {
			char s1[32], s2[32], s3[32];
			memcpy(s1, sid+1, 4);
			s1[4] = 0;
			memcpy(s2, sid+5, 2);
			s2[2] = 0;
			memcpy(s3, sid+7, 4);
			s3[4] = 0;
			sprintf(s, "%s-%s-%s", s1, s2, s3);
		} else if (sid[0] == 'Z') {
			char s2[32];
			memcpy(s2, sid+1, 4);
			s2[4] = 0;
			sprintf(s, "%s%s", m_style.getText("/style/center"), s2);
		} else if (sid[0] == 'M') {
			char s2[32];
			memcpy(s2, sid+7, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", m_style.getText("/style/panel"), atoi(s2)+1);
		} else if (sid[0] == 'W') {
			char s2[32];
			memcpy(s2, sid+1, 4);
			s2[4] = 0;
			sprintf(s, "%s%s", m_style.getText("/style/wall"), s2);
		} else if (sid[0] == 'H') {
			char s2[32];
			memcpy(s2, sid+11, 1);
			s2[1] = 0;
			sprintf(s, "%s%d", m_style.getText("/style/person"), atoi(s2)+1);
		}
	}
}

void wTalk::jpeg(const char *url)
{
	if (url)
		sLogger.jpeg.insert(sCaller.m_tv.tv_sec, url);
}

int wTalk::doElevVisit(const char *host)
{
	host2id_t *id = sCaller.host2id(host);
	if (id != NULL) {
		if (id->build == 0 && id->unit == 0 && id->floor == 0) { // 管理机
		} else if (id->floor == 99 && id->build > 0) { // 单元门口机
		} else if (id->floor == 99 && id->build == 0) { // 围墙机
		} else if (strlen(id->host) < 11) { //分机
			if (id->build != sys.talk.build() || id->unit != sys.talk.unit() ||
			    id->floor != sys.talk.floor() || id->family != sys.talk.family()) {
				dxml p;
				dmsg req;
				p.setText("/event/broadcast_url", "/elev/visit");
				p.setInt("/event/elev/from", id->floor*100+id->family);
				p.setInt("/event/elev/to", sys.talk.floor()*100+sys.talk.family());
				req.request("/talk/broadcast/data", p.data());
				return 1;
			}
		}
	}
	return 0;
}
