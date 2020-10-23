
#include "sys.h"
#include "wNtSetup.h"
#include "wNtSetupVolumeLabel.h"

wNtSetupVolumeLabel::wNtSetupVolumeLabel() : wNtWindow("setup/volume")
{
	m_bkg2.setParent(this);
	m_bkg2.load(m_style, "bkg2");
	m_title.setParent(this);
	m_title.load(m_style, "title");
	m_music_text.setParent(this);
	m_music_text.load(m_style, "music_text");
	m_music.setParent(this);
	m_music.load(m_style, "music");
	m_music.setRange(0, 5);
	m_music.setValue(5-sys.volume.music());
}

wNtSetupVolumeLabel::~wNtSetupVolumeLabel()
{
}

void wNtSetupVolumeLabel::doEvent(mEvent *e)
{
	wNtWindow::doEvent(e);

	if (e->type == mEvent::KeyRelease) {
		if (e->wParam == KEY_M_N1) {
			int n = m_music.value();
			if (n < 5) {
				m_music.setValue(n+1);
				sys.volume.music(5-m_music.value());
				sys.save();
			}
		} else if (e->wParam == KEY_M_N2) {
			int n = m_music.value();
			if (n > 0) {
				m_music.setValue(n-1);
				sys.volume.music(5-m_music.value());
				sys.save();
			}
		} else if (e->wParam == KEY_M_N3) {
		} else if (e->wParam == KEY_M_N5) {
			wNtSetup *w = new wNtSetup();
			w->show();
		}
	}
}

void wNtSetupVolumeLabel::doTimer(void)
{
	wNtWindow::doTimer();
}
