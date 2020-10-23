
#ifndef __M_OBJECT_H__
#define __M_OBJECT_H__

#include "types.h"
#include "dxml.h"
#include "sFifo.h"

#include "mFB.h"
#include "mEvent.h"

#define UI_ALIGN_LEFT		0x01
#define UI_ALIGN_RIGHT		0x02
#define UI_ALIGN_CENTER		0x04

#define MAX_SUBS_SZ	64

class mObject {
public:
	mObject();
	virtual ~mObject();

	void setParent(mObject *obj);

	uint32_t id(void) { return (uint32_t)this; };
	int add(mObject *obj);
	void remove(mObject *obj);
	void paint(void);
	virtual void load(dxml &p, const char *zone);
	virtual void move(int x, int y);
	virtual void show(void);
	virtual void hide(void);
	virtual void key(int c) {};
	int x(void) { return m_x; };
	int y(void) { return m_y; };
	int width(void) { return m_width; };
	int height(void) { return m_height; };

	int level(void) { return m_level; };
	void setLevel(int val) { m_level = val; };

	virtual void setStyle(const char *s);
	virtual void setFocus(int val);
	virtual void setEnable(int val);
	virtual void doPaint(void);
	virtual void doEvent(mEvent *e);
	virtual void doTimer(void);
	virtual void doTouch(int x, int y, int m);

	static mObject *focus(void);

	static int sys_refresh;
	static mObject *sys_focus;

protected:
	int m_x, m_y;
	int m_width, m_height;
	int m_visible;
	int m_enable;
	int m_focus;
	int m_level;

	const char *m_name;
	mObject *m_parent;
	mObject *m_subs[MAX_SUBS_SZ];
};

#endif

