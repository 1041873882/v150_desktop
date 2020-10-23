
#ifndef __W_NT_SETUP_H__
#define __W_NT_SETUP_H__

#include "wNtWindow.h"

class wNtSetup : public wNtWindow {
public:
	wNtSetup();
	virtual ~wNtSetup();

	void doEvent(mEvent *e);

private:
	mButton m_title;
	mText m_fw_text;
	mText m_fw;
	mText m_ui_text;
	mText m_ui;
	mText m_ip_text;
	mText m_ip;
	mText m_gw_text;
	mText m_gw;
	mText m_mac_text;
	mText m_mac;
	mText m_room_text;
	mText m_room;
};

#endif
