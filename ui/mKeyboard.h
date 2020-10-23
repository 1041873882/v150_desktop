
#ifndef __M_KEYBOARD_H__
#define __M_KEYBOARD_H__

#include "mWindow.h"
#include "mButton.h"

#define MAX_KEY_SZ	12

class mKeyboard : public mWindow {
public:
	mKeyboard();
	virtual ~mKeyboard();

	void doTouch(int x, int y, int m);
	void setParent(mObject *obj);
	void resize(void);
	void doEvent(mEvent *e);

public:
	static mKeyboard *keyboard;

private:
	mObject *m_edit;
	mPixmap m_bkg;
	mButton m_key[MAX_KEY_SZ];
};

#endif
