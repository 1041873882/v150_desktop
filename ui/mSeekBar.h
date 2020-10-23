
#ifndef __M_SEEK_BAR_H__
#define __M_SEEK_BAR_H__

#include "mPixmap.h"

class mSeekBar : public mPixmap {
public:
	mSeekBar();
	virtual ~mSeekBar();

	void setRange(int min, int max) { m_min = min, m_max = max; };
	void setValue(int val) { m_value = val; this->paint(); };
	int value(void) { return m_value; };

	void load(dxml &p, const char *zone);
	void doTouch(int x, int y, int m);
	void doPaint(void);

	void setFocus(int val);

protected:
	mPixmap m_seek;
	int m_min;
	int m_max;
	int m_value;

	int m_tp_x;
};

#endif
