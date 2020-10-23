
#include "sys.h"
#include "SysSound.h"
#include "mFB.h"
#include "wSetupNormalLabel.h"

wSetupNormalLabel::wSetupNormalLabel() : mWindow("setup/normal")
{
	m_bright_text.setParent(this);
	m_bright_text.load(m_style, "bright_text");
	m_bright.setParent(this);
	m_bright.load(m_style, "bright");
	m_bright.setRange(0, 5);
	m_bright.setValue(sys.lcd.bright()/255.0*5.0);

	m_sound_text.setParent(this);
	m_sound_text.load(m_style, "sound_text");
	m_sound.setParent(this);
	m_sound.load(m_style, "sound");
	m_sound.setRange(0, 5);
	m_sound.setValue(5-sys.volume.music());

	m_talk_text.setParent(this);
	m_talk_text.load(m_style, "talk_text");
	m_talk.setParent(this);
	m_talk.load(m_style, "talk");
	m_talk.setRange(0, 5);
	m_talk.setValue(5-sys.volume.talk());

	m_answer_text.setParent(this);
	m_answer_text.load(m_style, "answer_text");
	m_answer.setParent(this);
	m_answer.load(m_style, "answer");
	m_answer_st = sys.talk.answer();
	if (m_answer_st)
		m_answer.setText("√");
	else
		m_answer.setText("");

	m_lang_text.setParent(this);
	m_lang_text.load(m_style, "language_text");
	m_lang.setParent(this);
	m_lang.load(m_style, "language");
	m_lang.insert("简体中文");
	m_lang.insert("English");
	if (sys.flash() != 0) {
		m_lang.insert("繁體中文");
		m_lang.insert("Polska");
		m_lang.insert("Français");
		m_lang.insert("Türkçe");
		m_lang.insert("Slovensko");
	}

	m_lang.select(sys.user.language());

	m_tz_text.setParent(this);
	m_tz_text.load(m_style, "tz_text");
	m_tz.setParent(this);
	m_tz.load(m_style, "tz");
	m_tz.setMode(mEdit::TZ);
	m_tz.setText(sys.user.tz());

	m_ok.setParent(this);
	m_ok.load(m_style, "ok");
}

wSetupNormalLabel::~wSetupNormalLabel()
{
}

void wSetupNormalLabel::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_ok.id()) {
			this->save();
		} else if (e->wParam == m_answer.id()) {
			m_answer_st = m_answer_st ? 0 : 1;
			if (m_answer_st)
				m_answer.setText("√");
			else
				m_answer.setText("");
		}
	}
}

void wSetupNormalLabel::save(void)
{
	int err = 0;
	const char *tz = m_tz.text();
	if (strlen(tz) < 6)
		err = 1;
	else {
		char s1[3], s2[3];
		memcpy(s1, tz+1, 2); s1[2] = 0;
		memcpy(s2, tz+4, 2); s2[2] = 0;
		if (atoi(s1) > 13 || atoi(s2) > 59)
			err = 1;
	}
	if (!err) {
		sys.talk.answer(m_answer_st);
		sys.user.language(m_lang.select());
		sys.user.tz(m_tz.text());

		int val = m_bright.value()/5.0*255.0;
		sys.lcd.bright(val);
		fb.bright(sys.lcd.bright());

		sys.volume.music(5-m_sound.value());
		sys.volume.talk(5-m_talk.value());
		sys.save();
		sound.setup_ok();
	} else
		sound.setup_err();
}
