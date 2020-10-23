
#include "sys.h"
#include "SysLogger.h"
#include "mFB.h"
#include "wNtIntercom.h"
#include "wNtIntercomLoggerLabel.h"

#define MAX_MSG_SZ	8

wNtIntercomLoggerLabel::wNtIntercomLoggerLabel() : wNtWindow("intercom/logger")
{
	m_offset = 44;

	m_title.setParent(this);
	m_title.load(m_style, "title");
	for(int i=0; i<8; i++) {
		m_data[i].setParent(this);
		m_type[i].setParent(this);
		m_index[i].setParent(this);
		m_host[i].setParent(this);
		m_ts[i].setParent(this);
		m_jpeg[i].setParent(this);

		m_data[i].load(m_style, "data");
		m_data[i].move(m_data[0].x(), m_data[0].y()+i*m_offset);

		m_index[i].load(m_style, "index");
		m_index[i].move(m_index[0].x(), m_index[0].y()+i*m_offset);

		m_host[i].load(m_style, "host");
		m_host[i].move(m_host[0].x(), m_host[0].y()+i*m_offset);

		m_ts[i].load(m_style, "ts");
		m_ts[i].move(m_ts[0].x(), m_ts[0].y()+i*m_offset);

		m_jpeg[i].load(m_style, "jpeg");
		m_jpeg[i].move(m_jpeg[0].x(), m_jpeg[0].y()+i*m_offset);
	}

	m_jpeg_view.setParent(this);
	m_jpeg_view.hide();

	sLogger.talk.m_NA = 0;

	m_mode = 0;
	m_select = 0;
	m_first = 0;
	this->showData(m_first);
	this->showSelect();
}

wNtIntercomLoggerLabel::~wNtIntercomLoggerLabel()
{
}

void wNtIntercomLoggerLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1 && !m_mode) {
			if (m_select > 0) {
				m_select--;
				this->showSelect();
			} else {
				if (m_first >= MAX_MSG_SZ) {
					m_first -= MAX_MSG_SZ;
					m_select = 0;
					this->showData(m_first);
					this->showSelect();
				}
			}
		} else if (e->wParam == KEY_M_N2 && !m_mode) {
			if (m_select+1 < m_max) {
				m_select++;
				this->showSelect();
			} else {
				if (m_first+MAX_MSG_SZ < sLogger.talk.m_sz) {
					m_first += MAX_MSG_SZ;
					m_select = 0;
					this->showData(m_first);
					this->showSelect();
				}
			}
		} else if (e->wParam == KEY_M_N3 && !m_mode) {
			if (m_first+m_select < sLogger.talk.m_sz) {
				const char *url = sLogger.jpeg.url(sLogger.talk.data[m_first+m_select].ts);
				if (url) {
					int w = 560*fb.width()/800;
					int h = 390*fb.height()/480;
					m_jpeg_view.loadFile(url);
					m_jpeg_view.resize(w, h);
					m_jpeg_view.move((640-w)/2, 72);
					m_jpeg_view.show();

					for(int i=0; i<4; i++)
						m_btn[i].hide();
					m_mode = 1;
				}
			}
		} else if (e->wParam == KEY_M_N4 && !m_mode) {
			if (m_first+m_select < sLogger.talk.m_sz) {
				sLogger.talk.remove(m_first+m_select);
				if (m_first >= sLogger.talk.m_sz)
					m_first = 0;
				this->showData(m_first);
				if (m_select >= m_max)
					m_select = m_max-1;
				this->showSelect();
			}
		} else if (e->wParam == KEY_M_N5) {
			if (m_mode) {
				for(int i=0; i<4; i++)
					m_btn[i].show();
				m_mode = 0;
				m_jpeg_view.hide();
			} else {
				wNtIntercom *w = new wNtIntercom();
				w->show();
			}
		}
	}
}

void wNtIntercomLoggerLabel::showData(int n)
{
	m_max = MAX_MSG_SZ;
	if (n+MAX_MSG_SZ > sLogger.talk.m_sz)
		m_max = sLogger.talk.m_sz-n;

	for(int i=0; i<MAX_MSG_SZ; i++) {
		m_index[i].setColor(0xFFFFFF);
		m_host[i].setColor(0xFFFFFF);
		m_ts[i].setColor(0xFFFFFF);
		if (i < m_max) {
			char s[128];

			sprintf(s, "type_%d", sLogger.talk.data[n+i].type);
			m_type[i].load(m_style, s);
			m_type[i].move(m_type[i].x(), m_type[i].y()+i*m_offset);
			m_type[i].show();

			sprintf(s, "%d", n+i+1);
			m_index[i].setText(s);
			m_index[i].show();

			m_host[i].setText(sLogger.talk.data[n+i].id);
			m_host[i].show();

			time_t ts = sLogger.talk.data[n+i].ts;
			struct tm *tm = localtime(&ts);
			sprintf(s, "%d-%d-%d %02d:%02d:%02d", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			m_ts[i].setText(s);
			m_ts[i].show();

			if (sLogger.jpeg.url(sLogger.talk.data[n+i].ts))
				m_jpeg[i].show();
			else
				m_jpeg[i].hide();
		} else {
			m_type[i].hide();
			m_index[i].hide();
			m_host[i].hide();
			m_ts[i].hide();
			m_jpeg[i].hide();
		}
	}
}

void wNtIntercomLoggerLabel::showSelect(void)
{
	for(int i=0; i<MAX_MSG_SZ; i++) {
		if (m_select == i) {
			m_index[i].setColor(0xFF0000);
			m_host[i].setColor(0xFF0000);
			m_ts[i].setColor(0xFF0000);
		} else {
			m_index[i].setColor(0xFFFFFF);
			m_host[i].setColor(0xFFFFFF);
			m_ts[i].setColor(0xFFFFFF);
		}
	}
}
