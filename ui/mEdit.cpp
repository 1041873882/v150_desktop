
#include "mEdit.h"

mEdit::mEdit()
{
	m_name = "mEdit";
	m_max = 20;
	m_mode = 0;
	m_keyboard = NULL;
	memset(m_data, 0, sizeof(m_data));
}

mEdit::~mEdit()
{
	if (m_keyboard) {
		delete m_keyboard;
		m_keyboard = NULL;
	}
}

void mEdit::load(dxml &p, const char *zone)
{
	mButton::load(p, zone);
	if (m_argb == NULL) {
		m_width = m_text.width();
		m_height = m_text.height();
		m_argb = new uint32_t[m_width*m_height];
		for(int i=0; i<m_width*m_height; i++)
			m_argb[i] = 0xFFFFFFFF;
	}
}

void mEdit::doEvent(mEvent *e)
{
	mButton::doEvent(e);
	if (m_keyboard && e->type == mEvent::KeyboardEnd && e->wParam == m_keyboard->id()) {
		this->key('\r');
	}
}

void mEdit::doTouch(int x, int y, int m)
{
	if (m_visible && m_enable && x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
		mEvent::doFifo(this);
	}
}

void mEdit::setFocus(int val)
{
	if (val)
		this->showKeyboard();
}

void mEdit::setColor(uint32_t c)
{
	m_text.setColor(c);
}

void mEdit::key(int c)
{
	int n = strlen(m_data);
	if (c == -1) {
		this->hideKeyboard();
	} else if (c == 0x7F) {
		if (n > 0) {
			m_data[n-1] = 0;
			if (m_mode == PASSWD) {
				char s[128];
				memset(s, 0, sizeof(s));
				memset(s, '*', strlen(m_data));
				mButton::setText(s);
			} else
				mButton::setText(m_data);
		}
	} else if (c == '\r') { //»Ø³µ
		if (m_keyboard) {
			mEvent e(mEvent::EditEnd);
			e.wParam = (uint32_t)this;
			mEvent::post(&e);
		}
		this->hideKeyboard();
	} else {
		if (m_mode == NONE || m_mode == PASSWD) {
			if (n < m_max && c != '.') {
				m_data[n] = c;
				m_data[n+1] = 0;
				if (m_mode == PASSWD) {
					char s[128];
					memset(s, 0, sizeof(s));
					memset(s, '*', strlen(m_data));
					mButton::setText(s);
				} else
					mButton::setText(m_data);
			}
		} else if (m_mode == IP) {
			int d = 0;
			char *p = m_data;
			for(int i=0; i<strlen(m_data); i++) {
				if (m_data[i] == '.') {
					p = &m_data[i+1];
					d++;
				}
			}
			if (c == '.') {
				if (d < 3 && strlen(p) > 0) {
					m_data[n++] = c;
					m_data[n] = 0;
					mButton::setText(m_data);
				}
			} else {
				if (d < 3 && strlen(p) >= 3) {
					m_data[n++] = '.';
					m_data[n++] = c;
					m_data[n] = 0;
				} else if (strlen(p) < 3) {
					m_data[n++] = c;
					m_data[n] = 0;
					if (atoi(p) > 255)
						m_data[n-1] = 0;
				}
				mButton::setText(m_data);
			}
		} else if (m_mode == TZ) {
			if (c == '.') {
				if (n == 0) {
					m_data[n++] = '-';
					m_data[n] = 0;
				}
			} else if (n < 6) {
				if (n == 0)
					m_data[n++] = '+';
				else if (n == 3) {
					m_data[n++] = ':';
					m_data[n] = 0;
				}
				m_data[n++] = c;
				m_data[n] = 0;
			}
			mButton::setText(m_data);
		}
	}
}

void mEdit::setText(const char *text)
{
	if (text == NULL)
		text = "";
	strcpy(m_data, text);
	if (m_mode == PASSWD) {
		char s[128];
		memset(s, 0, sizeof(s));
		memset(s, '*', strlen(m_data));
		mButton::setText(s);
	} else
		mButton::setText(text);
}

void mEdit::setInt(int val)
{
	char s[64];
	sprintf(s, "%d", val);
	this->setText(s);
}

void mEdit::showKeyboard(void)
{
	if (mKeyboard::keyboard == NULL && m_keyboard == NULL) {
		m_keyboard = new mKeyboard();
		m_keyboard->setParent(this);
		m_keyboard->resize();
		this->alpha(0x80);
		this->paint();
	}
}

void mEdit::hideKeyboard(void)
{
	if (m_keyboard) {
		delete m_keyboard;
		m_keyboard = NULL;
		this->alpha(0xFF);
		this->paint();
	}
}
