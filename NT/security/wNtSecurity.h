
#ifndef __W_NT_SECURITY_H__
#define __W_NT_SECURITY_H__

#include "mEdit.h"
#include "wNtWindow.h"

class wNtSecurity : public wNtWindow {
public:
	wNtSecurity();
	virtual ~wNtSecurity();

	void doEvent(mEvent *e);

private:
	mButton m_title;
	mPixmap m_bkg2;
};

#endif
