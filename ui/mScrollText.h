
#ifndef __M_SCROLL_TEXT_H__
#define __M_SCROLL_TEXT_H__

#include <string>
#include "mObject.h"
#include "mText.h"

class mScrollText : public mText {
public:
	mScrollText();
	virtual ~mScrollText();

	void doTouch(int x, int y, int m);
	void doEvent(mEvent *e);
	int scroll(void) { return m_scroll; };
	void scrollTo(int offset);
	int offset(void) { return m_mline_offset; };

protected:
	int m_scroll_y;
};

#endif
