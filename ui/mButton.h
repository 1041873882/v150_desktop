
#ifndef __M_BUTTON_H__
#define __M_BUTTON_H__

#include <string>
#include "mObject.h"
#include "mPixmap.h"
#include "mText.h"

class mButton : public mPixmap {
public:
	mButton();
	virtual ~mButton();

	void load(dxml &p, const char *zone);
	void move(int x, int y);
	void doTouch(int x, int y, int m);

	void setColor(uint32_t c) { m_text.setColor(c); };
	virtual void setText(const char *text);
	virtual void setText(std::string val) { this->setText(val.c_str()); };
	void setFont(int val) { m_text.setFont(val); };
	void setFocus(int val);
	const char *text(void) { return m_text.text(); };

	void doPaint(void);

protected:
	mText m_text;
};

#endif
