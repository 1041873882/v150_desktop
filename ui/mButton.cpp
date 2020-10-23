
#include "mWindow.h"
#include "mButton.h"

mButton::mButton()
{
	m_name = "mButton";
}

mButton::~mButton()
{
}

void mButton::load(dxml &p, const char *zone)
{
	char s[512];
	sprintf(s, "/style/%s/style", zone);
	m_text.setStyle(p.getText(s));
	sprintf(s, "/style/%s/text", zone);
	setText(p.getText(s));
	mPixmap::load(p, zone);

	if (m_argb == NULL) {
		m_width = m_text.width();
		m_height = m_text.height();
	}
}

void mButton::move(int x, int y)
{
	m_text.move(x, y);
	mPixmap::move(x, y);
}

void mButton::doTouch(int x, int y, int m)
{
	if (m_visible && m_enable && x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
		mEvent::doFifo(this);
	}
	mPixmap::doTouch(x, y, m);
}

void mButton::setFocus(int val)
{
	if (val)
		this->alpha(0x80);
	else
		this->alpha(0xFF);
	mPixmap::setFocus(val);
}

void mButton::setText(const char *text)
{
	m_text.setText(text);
	this->paint();
}

void mButton::doPaint(void)
{
	mPixmap::doPaint();
	if (m_visible) {
		m_text.setRect(m_width, m_height);
		m_text.doPaint();
	}
}
