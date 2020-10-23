
#ifndef __W_NT_WINDOW_H__
#define __W_NT_WINDOW_H__

#include "mWindow.h"
#include "mButton.h"

class wNtWindow : public mWindow {
public:
	wNtWindow(const char *name);
	virtual ~wNtWindow();

	void doEvent(mEvent *e);

protected:
	mPixmap m_bkg;
	mButton m_btn[5];
};

#endif
