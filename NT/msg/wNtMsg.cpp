
#include "sys.h"
#include "SysLogger.h"
#include "wNtMain.h"
#include "wNtMsg.h"

#define MAX_MSG_SZ	8

wNtMsg::wNtMsg() : wNtWindow("msg")
{
	m_title.setParent(this);
	m_title.load(m_style, "title");
	for(int i=0; i<8; i++) {
		m_data[i].setParent(this);
		m_data[i].load(m_style, "data");
		m_data[i].move(m_data[0].x(), m_data[0].y()+i*44);

		m_context[i].setParent(this);
		m_context[i].load(m_style, "context");
		m_context[i].move(m_context[0].x(), m_context[0].y()+i*44);

		m_date[i].setParent(this);
		m_date[i].load(m_style, "date");
		m_date[i].move(m_date[0].x(), m_date[0].y()+i*44);
	}

	m_text_bkg.setParent(this);
	m_text_bkg.load(m_style, "text_bkg");
	m_text_bkg.hide();
	m_text_date.setParent(this);
	m_text_date.load(m_style, "text_date");
	m_text_date.hide();
	m_text_data.setParent(this);
	m_text_data.load(m_style, "text_data");
	m_text_data.setMultiLine(1);
	m_text_data.hide();

	m_mode = 0;
	m_select = 0;
	m_first = 0;
	this->showData(m_first);
	this->showSelect();

	sLogger.msg.m_have = 0;
}

wNtMsg::~wNtMsg()
{
}

void wNtMsg::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);
	if (e->type == mEvent::KeyPress) {
	} else if (e->type == mEvent::KeyRelease) {
		if (m_mode == 0) {
			if (e->wParam == KEY_M_N1) {
				if (m_select > 0) {
					m_select--;
					this->showSelect();
				} else {
					if (m_first >= MAX_MSG_SZ) {
						m_select = 0;
						m_first -= MAX_MSG_SZ;
						this->showData(m_first);
						this->showSelect();
					}
				}
			} else if (e->wParam == KEY_M_N2) {
				if (m_select+1 < m_max) {
					m_select++;
					this->showSelect();
				} else {
					if (m_first+MAX_MSG_SZ < sLogger.msg.m_sz) {
						m_select = 0;
						m_first += MAX_MSG_SZ;
						this->showData(m_first);
						this->showSelect();
					}
				}
			} else if (e->wParam == KEY_M_N3) {
				this->showText();
			} else if (e->wParam == KEY_M_N4) {
				if (m_first+m_select < sLogger.msg.m_sz) {
					sLogger.msg.remove(m_first+m_select);
					if (m_first >= sLogger.msg.m_sz)
						m_first = 0;
					this->showData(m_first);
					if (m_select >= m_max)
						m_select = m_max-1;
					this->showSelect();
				}
			} else if (e->wParam == KEY_M_N5) {
				wNtMain *w = new wNtMain();
				w->show();
			}
		} else {
			if (e->wParam == KEY_M_N1) {
				if (m_text_data.offset() >= m_text_data.height())
					m_text_data.scrollTo(-m_text_data.height());
			} else if (e->wParam == KEY_M_N2) {
				if (m_text_data.scroll())
					m_text_data.scrollTo(m_text_data.height());
			} else if (e->wParam == KEY_M_N5) {
				m_text_bkg.hide();
				m_text_date.hide();
				m_text_data.hide();
				m_btn[0].setText("上一条");
				m_btn[1].setText("下一条");
				m_btn[2].show();
				m_btn[3].show();
				m_mode = 0;
			}
		}
	}
}

void wNtMsg::doTimer(void)
{
	wNtWindow::doTimer();
}

void wNtMsg::showData(int n)
{
	m_max = MAX_MSG_SZ;
	if (n+MAX_MSG_SZ > sLogger.msg.m_sz)
		m_max = sLogger.msg.m_sz-n;

	for(int i=0; i<MAX_MSG_SZ; i++) {
		if (i < m_max) {
			char s[4*1024];
			sprintf(s, "%d. %s", n+i+1, sLogger.msg.data[n+i].text.c_str());
			m_context[i].setText(s);

			time_t ts = sLogger.msg.data[n+i].ts;
			struct tm *tm = localtime(&ts);
			sprintf(s, "%d-%d-%d %02d:%02d", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
			m_date[i].setText(s);
		} else {
			m_context[i].setText("");
			m_date[i].setText("");
		}
	}
}

void wNtMsg::showSelect(void)
{
	for(int i=0; i<MAX_MSG_SZ; i++) {
		if (m_select == i) {
			m_context[i].setColor(0xFF0000);
			m_date[i].setColor(0xFF0000);
		} else {
			m_context[i].setColor(0xFFFFFF);
			m_date[i].setColor(0xFFFFFF);
		}
	}
}

void wNtMsg::showText(void)
{
	int n = m_first+m_select;
	if (n >= sLogger.msg.m_sz)
		return;

	sLogger.msg.done(n);

	m_text_data.setText(sLogger.msg.data[n].text);

	char s[128];
	time_t ts = sLogger.msg.data[n].ts;
	struct tm *tm = localtime(&ts);
	sprintf(s, "日期: %d-%d-%d %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	m_text_date.setText(s);

	m_text_bkg.show();
	m_text_date.show();
	m_text_data.show();
	m_btn[0].setText("上一页");
	m_btn[1].setText("下一页");
	m_btn[2].hide();
	m_btn[3].hide();
	m_mode = 1;
}
