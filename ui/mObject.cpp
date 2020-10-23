
#include "mObject.h"

int mObject::sys_refresh = 0;
mObject *mObject::sys_focus = NULL;

mObject::mObject()
{
	m_name = "mObject";

	m_x = 0; m_y = 0;
	m_width = m_height = 0;
	m_visible = 1;
	m_enable = 1;
	m_focus = 0;
	m_level = 1;
	m_parent = NULL;
	for(int i=0; i<MAX_SUBS_SZ; i++)
		m_subs[i] = NULL;
}

mObject::~mObject()
{
	if (sys_focus == this)
		sys_focus = NULL;
	if (m_parent)
		m_parent->remove(this);
}

void mObject::setParent(mObject *obj)
{
	if (m_parent)
		m_parent->remove(this);

	m_parent = obj;
	if (m_parent)
		m_parent->add(this);
}

int mObject::add(mObject *obj)
{
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i] == obj)
			return 0;
	}
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i] == NULL) {
			m_subs[i] = obj;
			return 0;
		}
	}
	return -1;
}

void mObject::remove(mObject *obj)
{
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i] == obj) {
			m_subs[i] = NULL;
			break;
		}
	}
}

void mObject::load(dxml &p, const char *zone)
{
	char s[1024];

	sprintf(s, "/style/%s/x", zone);
	int x = p.getInt(s, 0);
	sprintf(s, "/style/%s/y", zone);
	int y = p.getInt(s, 0);
	this->move(x, y);

	sprintf(s, "/style/%s/style", zone);
	this->setStyle(p.getText(s));

	this->paint();
}

void mObject::move(int x, int y)
{
	if (x != m_x || y != m_y)
		this->paint();
	m_x = x;
	m_y = y;
}

void mObject::show(void)
{
	if (!m_visible)
		this->paint();
	m_visible = 1;
}

void mObject::hide(void)
{
	if (m_visible)
		this->paint();
	m_visible = 0;
}

void mObject::paint(void)
{
	sys_refresh = 1;
#if 0
	if (m_name)
		printf("%s paint\n", m_name);
#endif
}

void mObject::setStyle(const char *s)
{
}

void mObject::setFocus(int val)
{
	if (sys_focus == this && sys_focus->m_focus == val)
		return;

	if (val) {
		if (sys_focus != NULL)
			sys_focus->setFocus(0);
		sys_focus = this;
	} else {
		if (sys_focus == this)
			sys_focus = NULL;
	}
	m_focus = val;
	this->paint();
}

void mObject::setEnable(int val)
{
	m_enable = val;
}

void mObject::doPaint(void)
{
	if (m_visible) {
		for(int i=0; i<MAX_SUBS_SZ; i++) {
			if (m_subs[i])
				m_subs[i]->doPaint();
		}
	}
}

void mObject::doEvent(mEvent *e)
{
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i])
			m_subs[i]->doEvent(e);
	}
}

void mObject::doTimer(void)
{
	for(int i=0; i<MAX_SUBS_SZ; i++) {
		if (m_subs[i])
			m_subs[i]->doTimer();
	}
}

void mObject::doTouch(int x, int y, int m)
{
	if (m_visible && m_enable) {
		mObject *s[MAX_SUBS_SZ];
		for(int i=0; i<MAX_SUBS_SZ; i++)
			s[i] = m_subs[i];

		for(int i=0; i<MAX_SUBS_SZ; i++) {
			if (s[i] && m_subs[i])
				m_subs[i]->doTouch(x, y, m);
		}
	}
}

mObject *mObject::focus(void)
{
	return sys_focus;
}
