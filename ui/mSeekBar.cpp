
#include "mSeekBar.h"

mSeekBar::mSeekBar()
{
	m_name = "mSeekBar";
	m_min = 0;
	m_max = 10;
	m_value = 0;
	m_seek.setParent(this);
}

mSeekBar::~mSeekBar()
{
}

void mSeekBar::load(dxml &p, const char *zone)
{
	mPixmap::load(p, zone);

	char s[512];
	sprintf(s, "/style/%s/seek", zone);
	const char *url = p.getText(s);
	const char *root = p.getText("/style/root");
	if (root) {
		sprintf(s, "%s/%s", root, url);
		m_seek.loadFile(s);
	}
}

void mSeekBar::doTouch(int x, int y, int m)
{
	int yy = m_seek.height()/2;
	if (m && m_visible && x >= m_x && y >= (m_y-yy) && x <= (m_x+m_width) && y <= (m_y+m_height+yy)) {
		m_tp_x = x;
		mEvent::doFifo(this);
	}
	mPixmap::doTouch(x, y, m);
}

void mSeekBar::doPaint(void)
{
	int m = m_max-m_min;
	if (m <= 0)
		m = 1;
	int v = m_value-m_min;
	int offset = (m_width-m_seek.width())*v/m;
	int sh = m_seek.height()/2;
	m_seek.move(m_x+offset, m_y-sh+m_height/2);

	mPixmap::doPaint();
}

void mSeekBar::setFocus(int val)
{
	if (val) {
		m_value = 1.1*(m_max-m_min)*(m_tp_x-m_x)/m_width+m_min;
		if (m_value > m_max)
			m_value = m_max;
		else if (m_value < m_min)
			m_value = m_min;
		this->paint();
	}
	mPixmap::setFocus(val);
}
