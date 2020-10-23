
#ifndef __M_COMBOX_H__
#define __M_COMBOX_H__

#include <string>
#include "mObject.h"
#include "mText.h"

class mCombox;

class mComboxPopup : public mObject {
public:
	mComboxPopup();
	virtual ~mComboxPopup();

	void doTouch(int x, int y, int m);
	void setParent(mCombox *obj);
	void doPaint(void);

public:
	static mComboxPopup *object;

protected:
	mCombox *m_combox;
	mText m_text[16];
	mText m_text2;
	int m_pixel;
	int m_width;
	int m_height;
};

class mCombox : public mText {
public:
	mCombox();
	virtual ~mCombox();

	friend class mComboxPopup;

	void setFocus(int val);
	void doTouch(int x, int y, int m);
	void insert(const char *s);
	void flush(void) { m_sz = 0; };
	void select(int n);
	int select(void) { return m_select; };
	void doEvent(mEvent *e);
	void doPaint(void);
	void doSelect(int n);

	void setStyle(const char *s);
	void setSelectColor(uint32_t c);

protected:
	int m_expand;
	mComboxPopup *m_pop;
	std::string m_data[16];
	int m_sz;
	int m_select;

	uint32_t m_select_color;
	uint32_t m_normal_color;
};

#endif
