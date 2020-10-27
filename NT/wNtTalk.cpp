
#include "sys.h"
#include "SysCaller.h"
#include "SysLogger.h"
#include "mFB.h"
#include "wNtMain.h"
#include "wNtTalk.h"
#include "SysSecurity.h"

extern mWindow *SysTalk;

wNtTalk::wNtTalk() : wNtWindow("talk")
{
	SysTalk = this;
	fb.enable(1);

	m_host.setParent(this);
	m_host.load(m_style, "host");
	m_host.setText(sCaller.m_id);

	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

	if (sCaller.mode() == sCaller.CALLING) {
		m_type = sLogger.talk.OUT;
		m_answer_st = 1;
		m_btn[0].load(m_style, "btn0_b");
	} else {
		m_type = sLogger.talk.NA;
		m_answer_st = 0;
	}

	m_vo_st = 0;
	m_ts = time(NULL);
	m_jpeg = time(NULL);
	m_busy_ts = 0;
}

wNtTalk::~wNtTalk()
{
	SysTalk = NULL;
	fb.osd(0, 0, 0, 0);

	sLogger.talk.insert(m_type);
}

void wNtTalk::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEventJpeg) {
		const char *url = (const char *)e->wParam;
		this->jpeg(url);
	} else if (e->type == mEventBusy) {
		const char *id = (const char *)e->wParam;
		char s[128], host[64];
		this->host2id(id, host);
		sprintf(s, "%s %s", host, "无法呼入");
		m_host.setText(s);
		m_busy_ts = time(NULL);
		sCaller.m_h2id_mark = 0;
	} else if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N3) {
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				sCaller.answer();
				m_btn[0].load(m_style, "btn0_b");
			} else {
				sCaller.stop();
				printf("back main2\n");
			}
		} else if (e->wParam == KEY_M_N2) {
		} else if (e->wParam == KEY_M_N5) {
			sCaller.unlock();
			sSecurity.start_alarm();
		}
	}
}

void wNtTalk::doTimer(void)
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
		dxml p;
		dmsg req;
		sprintf(s, "%s/%ld.jpg", sLogger.jpeg_dir(), time(NULL));
		p.setText("/params/url", s);
		req.request("/talk/snapshot", p.data());

		if (sys.talk.answer()) {
			//自动接听
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
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

	wNtWindow::doTimer();
}

void wNtTalk::jpeg(const char *url)
{
	if (url)
		sLogger.jpeg.insert(sCaller.m_tv.tv_sec, url);
}

void wNtTalk::host2id(const char *sid, char *s)
{
	if (sid == NULL || s == NULL)
		return;

	strcpy(s, sid);
	host2id_t *id = sCaller.host2id(s);
	if (id != NULL) {
		if (id->build == 0 && id->unit == 0 && id->floor == 0) {
			sprintf(s, "%s%02d", "管理中心", id->family);
		} else if (id->build && id->floor == 99) {
			sprintf(s, "%s%02d", "门口机", id->family);
		} else if (id->build == 0 && id->floor == 99) {
			sprintf(s, "%s%02d", "围墙机", id->family);
		} else if (strlen(id->host) > 9) { //小门口机
			char s2[32];
			memcpy(s2, id->host, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", "小门口机", atoi(s2));
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
			sprintf(s, "%s%s", "管理中心", s2);
		} else if (sid[0] == 'M') {
			char s2[32];
			memcpy(s2, sid+7, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", "门口机", atoi(s2)+1);
		} else if (sid[0] == 'W') {
			char s2[32];
			memcpy(s2, sid+1, 4);
			s2[4] = 0;
			sprintf(s, "%s%s", "围墙机", s2);
		} else if (sid[0] == 'H') {
			char s2[32];
			memcpy(s2, sid+11, 1);
			s2[1] = 0;
			sprintf(s, "%s%d", "小门口机", atoi(s2)+1);
		}
	}
}
