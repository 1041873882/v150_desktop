
#ifndef __W_SETUP_ROOM_LABEL_H__
#define __W_SETUP_ROOM_LABEL_H__

#include "mWindow.h"
#include "mEdit.h"

class wSetupRoomLabel : public mWindow {
public:
	wSetupRoomLabel();
	virtual ~wSetupRoomLabel();

	void doEvent(mEvent *e);
	void save(void);

private:
	mText m_build_text;
	mEdit m_build;
	mText m_unit_text;
	mEdit m_unit;
	mText m_room_text;
	mEdit m_room;
	mText m_dcode_text;
	mEdit m_dcode;
	mText m_sync_text;
	mEdit m_sync;

	mText m_logo_text;
	mButton m_logo;
	int m_logo_st;

	mButton m_ok;
};

#endif
