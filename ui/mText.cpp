
#include "mFT.h"
#include "mFB.h"
#include "mText.h"

mText::mText()
{
	m_name = "mText";
	m_pixel = 24;
	m_color = 0xFF000000;
	m_alpha = 0xFF;

	m_left = 0;
	m_top = 0;

	m_align = 0;
	m_mline = 0;
	m_mline_offset = 0;
	m_scroll = 0;
}

mText::~mText()
{
}

void mText::setText(const char *val)
{
	if (val == NULL)
		return;
	m_scroll = 0;
	m_mline_offset = 0;
	m_text = val;
	this->paint();
}

void mText::doPaint(void)
{
	if (m_visible) {
		ft.setPixel(m_pixel);

		uint16_t uc[512];
		int n = ft.u8_uc(m_text.c_str(), uc, sizeof(uc));
		int x = 0, x2 = 0, y = 0, yc = 0;

		if (m_align) {
			int ww = 0;
			for(int k=0; k<n; k++) {
				ft.load(uc[k]);
				ww += ft.advance().x/64;
			}

			if (m_align & UI_ALIGN_CENTER) {
				x = (m_width-ww)/2;
				if (x < 0)
					x = 0;
				if (!m_mline)
					yc = (m_height-m_pixel)/2;
			}
			if (m_align & UI_ALIGN_LEFT)
				x = 0;
			else if (m_align & UI_ALIGN_RIGHT) {
				if (ww < m_width)
					x = m_width-ww-2;
				else
					x = 0;
			}
		}

		if (x < 0)
			x = 0;
		x += m_left;
		x2 = x;

		y -= m_mline_offset;
		m_scroll = 0;

		for(int k=0; k<n; k++) {
			if (uc[k] == '\n') {
				if (m_mline) {
					x2 = x;
					y += m_pixel;
					continue;
				} else
					break;
			} else if (uc[k] == '\r' || uc[k] == '\t')
				continue;

			ft.load(uc[k]);
			if (x2+ft.width() >= m_width) {
				if (m_mline) {
					x2 = x;
					y += m_pixel;
				} else
					break;
			}

			int y2 = y+yc+m_pixel-ft.top()+m_top;
			if (y2 > m_height) {
				m_scroll = 1;
				break;
			}

			if (y >= 0) {
				for(int j=0; j<ft.height(); j++) {
					for(int i=0; i<ft.width(); i++) {
						uint32_t c = m_color;
						uint8_t *p = (uint8_t *)&c;
						p[3] = ft.pixel(i, j)*p[3]/255;
						if (m_alpha != 0xFF)
							p[3] = p[3]*m_alpha/255;
					
						fb.pixel(x2+i+m_x+ft.left(), y2+j+m_y, c);
					}
				}
			}
			x2 += ft.advance().x/64;
		}
	}

	mObject::doPaint();
}

void mText::setColor(uint32_t c)
{
	if ((c&0xFF000000) == 0)
		c |= 0xFF000000;
	m_color = c;
}

void mText::setStyle(const char *s)
{
	if (s == NULL)
		return;

	m_color = 0xFF000000;
	m_alpha = 0xFF;
	m_left = 0;
	m_top = 0;
	m_align = 0;
	m_mline = 0;

	const char *font = strstr(s, "font:");
	const char *color = strstr(s, "color:");
	const char *left = strstr(s, "left:");
	const char *top = strstr(s, "top:");
	const char *width = strstr(s, "width:");
	const char *height = strstr(s, "height:");
	const char *align = strstr(s, "align:");

	if (font) {
		font += strlen("font:");
		this->setFont(atoi(font));
	}
	if (color) {
		color += strlen("color:");
		int val;
		sscanf(color, "%x", &val);
		this->setColor(val);
	}
	if (left) {
		left += strlen("left:");
		this->setMarginLeft(atoi(left));
	}
	if (top) {
		top += strlen("top:");
		this->setMarginTop(atoi(top));
	}
	if (width) {
		width += strlen("width:");
		m_width = atoi(width);
	}
	if (height) {
		height += strlen("height:");
		m_height = atoi(height);
	}
	if (align) {
		align += strlen("align:");
		char ss[1024];
		strcpy(ss, align);
		char *e = strchr(ss, ';');
		if (e) *e = 0;

		m_align = 0;
		if (strstr(ss, "left"))
			m_align |= UI_ALIGN_LEFT;
		if (strstr(ss, "right"))
			m_align |= UI_ALIGN_RIGHT;
		if (strstr(ss, "center"))
			m_align |= UI_ALIGN_CENTER;
	}
}

void mText::load(dxml &p, const char *zone)
{
	char s[1024];
	sprintf(s, "/style/%s/text", zone);
	this->setText(p.getText(s));
	mObject::load(p, zone);
}

void mText::show(void)
{
	m_scroll = 0;
	m_mline_offset = 0;
	mObject::show();
}
