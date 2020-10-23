
#include "SysLogger.h"
#include "wMain.h"
#include "wMessage.h"

#define MAX_MSG_SZ	6

wMessage::wMessage() : mWindow("message")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	for(int i=0; i<3; i++) {
		char s[128];
		sprintf(s, "title%d", i);
		m_title[i].setParent(this);
		m_title[i].load(m_style, s);
	}

	m_offset = m_style.getInt("/style/offset", 51);
	for(int i=0; i<MAX_MSG_SZ; i++) {
		m_data[i].setParent(this);
		m_data[i].load(m_style, "data");
		m_data[i].move(m_data[0].x(), m_data[0].y()+i*m_offset);
	}

	for(int i=0; i<MAX_MSG_SZ; i++) {
		m_index[i].setParent(this);
		m_index[i].load(m_style, "index");
		m_index[i].move(m_index[0].x(), m_index[0].y()+i*m_offset);

		m_context[i].setParent(this);
		m_context[i].load(m_style, "context");
		m_context[i].move(m_context[0].x(), m_context[0].y()+i*m_offset);

		m_date[i].setParent(this);
		m_date[i].load(m_style, "date");
		m_date[i].move(m_date[0].x(), m_date[0].y()+i*m_offset);
	}

	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_del.setParent(this);
	m_del.load(m_style, "del");

	m_view.setParent(this);
	m_view.load(m_style, "text_view");
	m_view_ts.setParent(&m_view);
	m_view_ts.load(m_style, "text_ts");
	m_view_msg.setParent(&m_view);
	m_view_msg.load(m_style, "text_msg");
	m_view_msg.setMultiLine(1);
	m_view.hide();

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_first = 0;
	this->showData(m_first);

	sLogger.msg.m_have = 0;
}

wMessage::~wMessage()
{
}

void wMessage::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		int n = -1;
		for(int i=0; i<MAX_MSG_SZ; i++) {
			if (e->wParam == m_data[i].id() && i<m_max) {
				n = i;
				break;
			}
		}
		if (n != -1) {
			if (n == m_select) {
				this->showMsg(m_first+m_select);
			} else {
				for(int i=0; i<m_max; i++)
					this->showColor(i);

				m_select = n;
				m_index[n].setColor(0x0000FF);
				m_context[n].setColor(0x0000FF);
				m_date[n].setColor(0x0000FF);
			}
		}

		if (e->wParam == m_del.id()) {
			if (m_select != -1) {
				sLogger.msg.remove(m_first+m_select);
				if (m_first >= sLogger.msg.m_sz)
					m_first = 0;
				this->showData(m_first);
			}
		} else if (e->wParam == m_up.id()) {
			if (m_first-MAX_MSG_SZ >= 0) {
				m_first -= MAX_MSG_SZ;
				this->showData(m_first);
			}
		} else if (e->wParam == m_down.id()) {
			if (m_first+MAX_MSG_SZ < sLogger.msg.m_sz) {
				m_first += MAX_MSG_SZ;
				this->showData(m_first);
			}
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		}
	} else if (e->type == mEvent::ScrollEnd) {
		if (e->wParam == m_view_msg.id())
			m_view.hide();
	}
}

void wMessage::doTimer(void)
{
	mWindow::doTimer();
}

void wMessage::showColor(int n)
{
	if (n < m_max) {
		if (sLogger.msg.data[m_first+n].done) {
			m_index[n].setColor(0xB0B0B0);
			m_context[n].setColor(0xB0B0B0);
			m_date[n].setColor(0xB0B0B0);
		} else {
			m_index[n].setColor(0xFFFFFF);
			m_context[n].setColor(0xFFFFFF);
			m_date[n].setColor(0xFFFFFF);
		}
	}
}

void wMessage::showData(int n)
{
	m_max = MAX_MSG_SZ;
	if (n+MAX_MSG_SZ > sLogger.msg.m_sz)
		m_max = sLogger.msg.m_sz-n;

	for(int i=0; i<MAX_MSG_SZ; i++) {
		if (i < m_max) {
			char s[128];

			sprintf(s, "%d", n+i+1);
			m_index[i].setText(s);
			m_index[i].show();

			m_context[i].setText(sLogger.msg.data[n+i].text);
			m_context[i].show();

			time_t ts = sLogger.msg.data[n+i].ts;
			struct tm *tm = localtime(&ts);
			sprintf(s, "%d-%d-%d %02d:%02d", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
			m_date[i].setText(s);
			m_date[i].show();

			this->showColor(i);
		} else {
			m_index[i].hide();
			m_context[i].hide();
			m_date[i].hide();
		}
	}
	m_select = -1;
}

void wMessage::showMsg(int n)
{
	char s[4*1024];
	time_t ts = sLogger.msg.data[n].ts;
	struct tm *tm = localtime(&ts);
	sprintf(s, "%s %d-%02d-%02d %02d:%02d", m_style.getText("/style/text/date"), tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	m_view_ts.setText(s);

	sprintf(s, "   %s", sLogger.msg.data[n].text.c_str());
	m_view_msg.setText(s);
	m_view.show();

	sLogger.msg.done(n);
}
