
#include "SysCaller.h"
#include "SysLogger.h"
#include "mFB.h"
#include "SysCaller.h"
#include "wIntercom.h"
#include "wIntercomLoggerLabel.h"

wIntercomLoggerLabel::wIntercomLoggerLabel() : mWindow("intercom/logger")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	for(int i=0; i<4; i++) {
		char s[128];
		sprintf(s, "title%d", i);
		m_title[i].setParent(this);
		m_title[i].load(m_style, s);
	}

	m_offset = m_style.getInt("/style/offset", 58);
	for(int i=0; i<MAX_LOGGER_LINE; i++) {
		m_data[i].setParent(this);
		m_data[i].load(m_style, "data");
		m_data[i].move(m_data[0].x(), m_data[0].y()+i*m_offset);

		m_type[i].setParent(this);
		m_type[i].load(m_style, "type");
		m_type[i].move(m_type[0].x(), m_type[0].y()+i*m_offset);

		m_index[i].setParent(this);
		m_index[i].load(m_style, "index");
		m_index[i].move(m_index[0].x(), m_index[0].y()+i*m_offset);

		m_host[i].setParent(this);
		m_host[i].load(m_style, "host");
		m_host[i].move(m_host[0].x(), m_host[0].y()+i*m_offset);

		m_date[i].setParent(this);
		m_date[i].load(m_style, "date");
		m_date[i].move(m_date[0].x(), m_date[0].y()+i*m_offset);

		m_timeout[i].setParent(this);
		m_timeout[i].load(m_style, "timeout");
		m_timeout[i].move(m_timeout[0].x(), m_timeout[0].y()+i*m_offset);

		m_jpeg[i].setParent(this);
		m_jpeg[i].load(m_style, "jpeg");
		m_jpeg[i].move(m_jpeg[0].x(), m_jpeg[0].y()+i*m_offset);
	}

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");
	m_prompt.hide();

	m_call.setParent(this);
	m_call.load(m_style, "call");
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	m_del.setParent(this);
	m_del.load(m_style, "del");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_jpeg_view.setParent(this);
	m_jpeg_view.hide();

	m_ts = 0;
	m_start = 0;
	sLogger.talk.m_NA = 0;
	m_first = 0;
	this->showData(m_first);
}

wIntercomLoggerLabel::~wIntercomLoggerLabel()
{
}

void wIntercomLoggerLabel::showData(int n)
{
	m_max = MAX_LOGGER_LINE;
	if (n+MAX_LOGGER_LINE > sLogger.talk.m_sz)
		m_max = sLogger.talk.m_sz-n;

	for(int i=0; i<MAX_LOGGER_LINE; i++) {
		m_index[i].setColor(0xFFFFFF);
		m_host[i].setColor(0xFFFFFF);
		m_date[i].setColor(0xFFFFFF);
		m_timeout[i].setColor(0xFFFFFF);
		if (i < m_max) {
			char s[128];
			sprintf(s, "type%d", sLogger.talk.data[n+i].type);
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
			sprintf(s, "%02d-%02d-%02d %02d:%02d:%02d", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			m_date[i].setText(s);
			m_date[i].show();

			sprintf(s, "%ds", sLogger.talk.data[n+i].timeout);
			m_timeout[i].setText(s);
			m_timeout[i].show();

			if (sLogger.jpeg.url(sLogger.talk.data[n+i].ts))
				m_jpeg[i].show();
			else
				m_jpeg[i].hide();
		} else {
			m_type[i].hide();
			m_index[i].hide();
			m_host[i].hide();
			m_date[i].hide();
			m_timeout[i].hide();
			m_jpeg[i].hide();
		}
	}
	m_select = -1;
}

void wIntercomLoggerLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (!m_start) {
			int n = -1;
			for(int i=0; i<MAX_LOGGER_LINE; i++) {
				if (e->wParam == m_data[i].id() && i<m_max) {
					n = i;
					break;
				}
			}
			if (n != -1) {
				if (n == m_select) {
					const char *url = sLogger.jpeg.url(sLogger.talk.data[m_first+n].ts);
					if (url) {
						int w = 540*fb.width()/800;
						int h = 380*fb.height()/480;
						m_jpeg_view.loadFile(url);
						m_jpeg_view.resize(w, h);
						m_jpeg_view.move((fb.width()-w)/2, (fb.height()-h)/2);
						m_jpeg_view.show();
						m_jpeg_view.paint();
					}
				} else {
					for(int i=0; i<m_max; i++) {
						m_index[i].setColor(0xFFFFFF);
						m_host[i].setColor(0xFFFFFF);
						m_date[i].setColor(0xFFFFFF);
						m_timeout[i].setColor(0xFFFFFF);
					}
					m_select = n;
					m_index[n].setColor(0x0000FF);
					m_host[n].setColor(0x0000FF);
					m_date[n].setColor(0x0000FF);
					m_timeout[n].setColor(0x0000FF);
				}
			}
		}

		if (e->wParam == m_jpeg_view.id()) {
			m_jpeg_view.recycle();
			m_jpeg_view.hide();
		}  else if (e->wParam == m_call.id()) {
			if (!m_start && m_select != -1) {
				int n = m_first+m_select;
				const char *id = sLogger.talk.data[n].id.c_str();
				if (id[0] >= '0' && id[0] <= '9') {
					sCaller.query(id);
					m_start = 1;
					this->showPrompt(1, NULL);
				} else if (id[0] == 'M' || id[0] == 'H' || id[0] == 'W' || id[0] == 'P') {
					return;
				} else {
					sCaller.sip.url = NULL;
					sCaller.q600(id);
					m_start = 1;
					this->showPrompt(3, id);
				}	
			}
		}  else if (e->wParam == m_del.id()) {
			if (m_select != -1) {
				sLogger.talk.remove(m_first+m_select);
				if (m_first >= sLogger.talk.m_sz)
					m_first = 0;
				this->showData(m_first);
			}
		} else if (e->wParam == m_up.id()) {
			if (m_first-MAX_LOGGER_LINE >= 0) {
				m_first -= MAX_LOGGER_LINE;
				this->showData(m_first);
			}
		} else if (e->wParam == m_down.id()) {
			if (m_first+MAX_LOGGER_LINE < sLogger.talk.m_sz) {
				m_first += MAX_LOGGER_LINE;
				this->showData(m_first);
			}
		} else if (e->wParam == m_exit.id()) {
			sCaller.stop();

			wIntercom *w = new wIntercom();
			w->show();
		}
	}
}

void wIntercomLoggerLabel::doTimer(void)
{
	mWindow::doTimer();

	if (m_start) {
		if (sCaller.mode() == sCaller.NONE) {
			this->showPrompt(2, NULL);
		} else if (sCaller.mode() == sCaller.QUERY) {
			if (sCaller.sip.url) {
				sCaller.start(sCaller.sip.url);
			} else if (sCaller.d600.host) {
				sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
			} else if (sCaller.ts() >= 2) {
				this->showPrompt(2, NULL);
			}
		} else if (sCaller.mode() == sCaller.CALLING) {
			if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
				this->showPrompt(2, NULL);
			}
		}
	}

	if (m_ts && labs(time(NULL)-m_ts) > 1) {
		m_ts = 0;
		m_start = 0;
		m_prompt.hide();
	}
}

void wIntercomLoggerLabel::showPrompt(int type, const char *id)
{
	char s[512] = "";
	if (type == 1) {
		sprintf(s, "%s %s", sCaller.m_id, m_style.getText("/style/text/calling"));
	} else if (type == 2) {
		m_start = 0;
		m_ts = time(NULL);
		sprintf(s, "%s", m_style.getText("/style/text/failed"));
	} else if (type == 3 && id != NULL) {
		sprintf(s, "%s %s", id,  m_style.getText("/style/text/calling"));
	}
	m_prompt.setText(s);
	m_prompt.show();
}
