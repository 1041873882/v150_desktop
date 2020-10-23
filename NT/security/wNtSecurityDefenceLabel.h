
#ifndef __W_NT_SECURITY_DEFENCE_LABEL_H__
#define __W_NT_SECURITY_DEFENCE_LABEL_H__

#include "mEdit.h"
#include "wNtWindow.h"

class wNtSecurityDefenceLabel : public wNtWindow {
public:
	wNtSecurityDefenceLabel();
	virtual ~wNtSecurityDefenceLabel();

	void doEvent(mEvent *e);

	void showPrompt(void);

private:
	mButton m_title;
	mPixmap m_bkg2;
	mText m_prompt;
};

#endif
