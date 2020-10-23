
#include "mFB.h"
#include "mWindow.h"
#include "mCombox.h"

mComboxPopup *mComboxPopup::object = NULL;

mComboxPopup::mComboxPopup()
{
	m_name = "mComboxPopup";
	mComboxPopup::object = this;

	int pixel = 40, font = 20, top = 10;
	if (fb.height() < 480) {
		pixel = 28;
		font = 18;
		top = 2;
	}

	m_combox = NULL;
	m_pixel = pixel;
	m_width = 0;
	m_height = 0;
	for(int i=0; i<16;i++) {
		m_text[i].setParent(this);
		m_text[i].setFont(font);
		m_text[i].setMarginLeft(8);
		m_text[i].setMarginTop(top);
		m_text[i].hide();
	}
	m_text2.setParent(this);
	m_text2.setText("√");
	m_text2.setMarginLeft(8);
	m_text2.setMarginTop(top);
	m_text2.setColor(0xFFFF0000);
	m_text2.setFont(font);

	this->setLevel(1010);
	if (mWindow::window)
		mObject::setParent(mWindow::window);
}

mComboxPopup::~mComboxPopup()
{
	mComboxPopup::object = NULL;
}

void mComboxPopup::doTouch(int x, int y, int m)
{
	mEvent::doFifo(this);
	if (!m)
		return;
	if (x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
		int n = (y-m_y)/m_pixel;

		mEvent e(mEvent::ComboxEnd);
		e.wParam = (uint32_t)m_combox;
		e.lParam = n;
		mEvent::post(&e);
	} else {
		mEvent e(mEvent::ComboxEnd);
		e.wParam = (uint32_t)m_combox;
		e.lParam = -1;
		mEvent::post(&e);
	}
}

void mComboxPopup::setParent(mCombox *obj)
{
	if (obj == NULL)
		return;

	m_combox = obj;
	m_width = fb.width()/2;
	m_height = m_combox->m_sz*m_pixel;
	m_x = (fb.width()-m_width)/2;
	m_y = (fb.height()-m_height)/2;
	for(int i=0; i<m_combox->m_sz; i++) {
		m_text[i].setText(m_combox->m_data[i]);
		m_text[i].move(m_x, m_y+i*m_pixel);
		m_text[i].setRect(m_width, m_pixel);
		if (i == m_combox->m_select) {
			m_text[i].setColor(0xFFFF0000);
			m_text2.move(m_x+m_width-m_pixel, m_y+i*m_pixel);
			m_text2.setRect(m_pixel, m_pixel);
		}
		m_text[i].show();
	}
}

void mComboxPopup::doPaint(void)
{
	if (m_visible) {
		int x = (fb.width()-m_width)/2;
		int y = (fb.height()-m_height)/2;
		for(int yy=0; yy<m_height; yy++) {
			if (yy%m_pixel == 0) {
				for(int xx=0; xx<m_width; xx++) {
					fb.pixel(x+xx, y+yy, 0xFF808080);
				}
			} else {
				for(int xx=0; xx<m_width; xx++) {
					fb.pixel(x+xx, y+yy, 0xFFFFFFFF);
				}
			}
		}
	}
	mObject::doPaint();
}

mCombox::mCombox()
{
	m_name = "mCombox";
	m_expand = 0;
	m_sz = 0;
	m_select = 0;
	m_pop = NULL;
}

mCombox::~mCombox()
{
	if (m_pop) {
		delete m_pop;
		m_pop = NULL;
	}
}

void mCombox::setFocus(int val)
{
	if (m_expand == 0 && mComboxPopup::object == NULL) {
		m_pop = new mComboxPopup();
		m_pop->setParent(this);
		m_expand = 1;
		m_color = m_select_color;
		this->paint();
	}
}

void mCombox::doTouch(int x, int y, int m)
{
	if (m && m_visible && m_enable) {
		if (x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
			mEvent::doFifo(this);
		}
	}
	mObject::doTouch(x, y, m);
}

void mCombox::doEvent(mEvent *e)
{
	if (e->type == mEvent::ComboxEnd && e->wParam == this->id()) {
		this->doSelect(e->lParam);
	}
}

void mCombox::doPaint(void)
{
	mText::doPaint();
}

void mCombox::insert(const char *s)
{
	if (m_sz == 0)
		this->setText(s);
	if (m_sz < 16) {
		m_data[m_sz] = s;
		m_sz++;
	}
}

void mCombox::doSelect(int n)
{
	if (m_pop) {
		delete m_pop;
		m_pop = NULL;
	}
	m_expand = 0;
	if (n != -1 && n < m_sz) {
		this->setText(m_data[n]);
		m_select = n;

		mEvent e(mEvent::EditEnd);
		e.wParam = (uint32_t)this;
		mEvent::post(&e);
	}
	m_color = m_normal_color;
	this->paint();
}

void mCombox::select(int n)
{
	if (n < m_sz) {
		m_select = n;
		this->setText(m_data[n]);
	}
}

void mCombox::setStyle(const char *s)
{
	mText::setStyle(s);
	m_normal_color = m_color;
	m_select_color = m_color;

	const char *s_color = strstr(s, "s_color:");
	if (s_color) {
		s_color += strlen("s_color:");
		int val;
		sscanf(s_color, "%x", &val);
		this->setSelectColor(val);
	}
}

void mCombox::setSelectColor(uint32_t c)
{
	if ((c&0xFF000000) == 0)
		c |= 0xFF000000;
	m_select_color = c;
}
