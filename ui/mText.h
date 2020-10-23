
#ifndef __M_TEXT_H__
#define __M_TEXT_H__

#include <string>
#include "mObject.h"

class mText : public mObject {
public:
	mText();
	virtual ~mText();

	void alpha(uint8_t val) { m_alpha = val; };

	void doPaint(void);
	void setText(const char *val);
	void setText(std::string val) { this->setText(val.c_str()); };
	void setFont(int val) { m_pixel = val; };
	void setMarginLeft(int val) { m_left = val; };
	void setMarginTop(int val) { m_top = val; };
	void setRect(int w, int h) { m_width = w; m_height = h; };
	void setAlign(int val) { m_align = val; };
	void setColor(uint32_t c);
	void setStyle(const char *s);
	void setMultiLine(int enable) { m_mline = enable; };

	void load(dxml &p, const char *zone);
	void show(void);
	const char *text(void) { return m_text.c_str(); };

protected:
	std::string m_text;
	int m_pixel;
	uint32_t m_color;
	uint8_t m_alpha;

	int m_left, m_top;
	int m_align;
	int m_mline; //0:单行 1:多行
	int m_mline_offset;
	int m_scroll;
};

#endif
