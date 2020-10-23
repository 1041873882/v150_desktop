
#include "sys.h"
#include "wNtMain.h"
#include "SysCaller.h"
#include "wNtIntercom.h"
#include "wNtIntercomCenterLabel.h"

wNtIntercomCenterLabel::wNtIntercomCenterLabel() : wNtWindow("intercom/center")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_talk_bkg.setParent(this);
	m_talk_bkg.load(m_style, "talk_bkg");
	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

	sCaller.stop();
	m_start = 0;
	this->start();
}

wNtIntercomCenterLabel::~wNtIntercomCenterLabel()
{
}

void wNtIntercomCenterLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
	} else if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N3) {
			if (m_start) {
				this->stop();
			}
			wNtMain *w = new wNtMain();
			w->show();
			printf("back main1\n");
		}
	}
}

void wNtIntercomCenterLabel::doTimer(void)
{
	wNtWindow::doTimer();

	if (m_start) {
		if (sCaller.mode() == sCaller.NONE) {
			//呼叫被结束强制呼叫下一个
			this->doCenterNext();
		} else if (sCaller.mode() == sCaller.QUERY) {
			if (sCaller.sip.url) {
				sCaller.start(sCaller.sip.url);
			} else if (sCaller.ts() > 1 && sCaller.d600.host) {
				sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
			} else if (sCaller.ts() > 2) {
				this->doCenterNext();
			}
		} else if (sCaller.mode() == sCaller.CALLING) {
			if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
				this->doCenterNext();
			}
		}
	}
}

void wNtIntercomCenterLabel::doCenterNext(void)
{
	if (sys.quick.enable()) {
		this->stop();
		return;
	}
	int id = atoi(sCaller.m_id);
	if (id < 10005) {
		char s[128];
		sprintf(s, "%d", id+1);
		sCaller.query(s);
		usleep(100*1000);
		sCaller.query(s);
		sprintf(s, "Z%04d", (id+1)-10000);
		sCaller.q600(s);
		this->showText(1);
	} else {
		this->stop();
	}
}

void wNtIntercomCenterLabel::start(void)
{
	if (sCaller.mode() != sCaller.NONE) {
		return;
	}
	m_start = 1;
	if (sys.quick.enable()) {
		printf("quick.enable111 = %d", sys.quick.enable());
		sCaller.start(sys.quick.url());
	} else {
		printf("quick.enable222 = %d\n", sys.quick.enable());
		sCaller.query("10001");
		usleep(100*1000);
		printf("quick.enable333 = %d\n", sys.quick.enable());
		sCaller.query("10001");
		sCaller.q600("Z0001");
	}
	this->showText(1);
}

void wNtIntercomCenterLabel::stop(void)
{
	m_start = 0;
	sCaller.stop();
	this->showText(2);
}

void wNtIntercomCenterLabel::showText(int type)
{
	char s[1024];
	int n = atoi(sCaller.m_id)-10000;
	const char *t1 = "管理中心";
	if (type == 1) {
		const char *t2 = "呼叫中";
		sprintf(s, "%s%d %s", t1, n, t2);
	} else if (type == 2) {
		const char *t2 = "呼叫失败";
		sprintf(s, "%s %s", t1, t2);
	} else
		sprintf(s, "%s%d", t1, n);
	m_prompt.setText(s);
}
