
#include "sys.h"
#include "SysSound.h"
#include "wSetupRoomLabel.h"

wSetupRoomLabel::wSetupRoomLabel() : mWindow("setup/room")
{
	m_build_text.setParent(this);
	m_build_text.load(m_style, "build_text");
	m_build.setParent(this);
	m_build.load(m_style, "build");
	m_build.setInt(sys.talk.build());
	m_build.setMax(3);

	m_unit_text.setParent(this);
	m_unit_text.load(m_style, "unit_text");
	m_unit.setParent(this);
	m_unit.load(m_style, "unit");
	m_unit.setInt(sys.talk.unit());
	m_unit.setMax(2);

	m_room_text.setParent(this);
	m_room_text.load(m_style, "room_text");
	m_room.setParent(this);
	m_room.load(m_style, "room");
	m_room.setInt(sys.talk.floor()*100+sys.talk.family());
	m_room.setMax(4);

	m_dcode_text.setParent(this);
	m_dcode_text.load(m_style, "dcode_text");
	m_dcode.setParent(this);
	m_dcode.load(m_style, "dcode");
	m_dcode.setInt(sys.talk.dcode());
	m_dcode.setMax(1);

	m_sync_text.setParent(this);
	m_sync_text.load(m_style, "sync_text");
	m_sync.setParent(this);
	m_sync.load(m_style, "sync");
	m_sync.setText(sys.talk.sync());
	m_sync.setMax(6);

	m_logo_text.setParent(this);
	m_logo_text.load(m_style, "logo_text");
	m_logo.setParent(this);
	m_logo.load(m_style, "logo");
	m_logo_st = sys.logo();
	if (m_logo_st)
		m_logo.setText("√");
	else
		m_logo.setText("");
	if (!sys_factory) {
		m_logo_text.hide();
		m_logo.hide();
	}

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
}

wSetupRoomLabel::~wSetupRoomLabel()
{
}

void wSetupRoomLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->save();
		} else if (e->wParam == m_logo.id()) {
			m_logo_st = m_logo_st ? 0 : 1;
			if (m_logo_st)
				m_logo.setText("√");
			else
				m_logo.setText("");
		}
	}
}

void wSetupRoomLabel::save(void)
{
	int err = 0;
	if (strlen(m_build.text()) == 0 || atoi(m_build.text()) == 0)
		err = 1;
	if (strlen(m_unit.text()) == 0 || atoi(m_unit.text()) > 99)
		err = 1;
	if (strlen(m_room.text()) == 0)
		err = 1;
	if (strlen(m_dcode.text()) == 0)
		err = 1;
	if (strlen(m_sync.text()) == 0)
		err = 1;

	int r = atoi(m_room.text());
	int floor = r/100;
	int family = r%100;
	if (floor > 98)
		err = 1;

	if (!err) {
		if (sys_factory) {
			sys.logo(m_logo_st);
		}

		sys.talk.build(atoi(m_build.text()));
		sys.talk.unit(atoi(m_unit.text()));
		sys.talk.floor(floor);
		sys.talk.family(family);
		sys.talk.dcode(atoi(m_dcode.text()));
		sys.talk.sync(m_sync.text());
		sys.save();
		sound.setup_ok();
	} else
		sound.setup_err();
}
