
#ifndef __M_EDIT_H__
#define __M_EDIT_H__

#include "mButton.h"
#include "mKeyboard.h"

class mEdit : public mButton {
public:
	mEdit();
	virtual ~mEdit();

	enum {
		NONE = 0,
		PASSWD = 1,
		IP = 2,
		TZ = 3
	};

	void load(dxml &p, const char *zone);

	const char *text(void) { return m_data; };
	void doEvent(mEvent *e);
	void doTouch(int x, int y, int m);
	void key(int c);
	void setText(const char *text);
	void setInt(int val);
	void setMax(int val) { m_max = val; };
	void setMode(int mode) { m_mode = mode; };
	void setFocus(int val);
	void setColor(uint32_t c);

	void showKeyboard(void);
	void hideKeyboard(void);

protected:
	char m_data[128];
	int m_max;
	int m_mode; //0:普通模式 1:密码模式 2:IP模式
	mKeyboard *m_keyboard;
};

#endif
