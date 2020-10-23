
#ifndef __W_SETUP_VERSION_LABEL_H__
#define __W_SETUP_VERSION_LABEL_H__

#include "mWindow.h"
#include "mText.h"

class wSetupVersionLabel : public mWindow {
public:
	wSetupVersionLabel();
	virtual ~wSetupVersionLabel();

private:
	mText m_fw_text;
	mText m_fw;
	mText m_ui_text;
	mText m_ui;
	mText m_ip_text;
	mText m_ip;
	mText m_mac_text;
	mText m_mac;
};

#endif
