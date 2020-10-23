
#ifndef __M_EVENT_H__
#define __M_EVENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/input.h>
#include <sys/time.h>

#include "types.h"

class mObject;

#define mEventMonitor		(mEvent::Type)(mEvent::User+2001)
#define mEventJpeg		(mEvent::Type)(mEvent::User+2002)
#define mEventElev		(mEvent::Type)(mEvent::User+2003)
#define mEventBusy		(mEvent::Type)(mEvent::User+2004)
#define mEventHook		(mEvent::Type)(mEvent::User+2005)

#define KEY_M_UNLOCK	KEY_LEFT
#define KEY_M_ANSWER	KEY_VOLUMEDOWN
#define KEY_M_MENU	KEY_MENU
#define KEY_M_CALL	KEY_ENTER
#define KEY_M_MONITOR	KEY_HOME

#define KEY_M_N1	KEY_M_ANSWER
#define KEY_M_N2	KEY_M_MENU
#define KEY_M_N3	KEY_M_CALL
#define KEY_M_N4	KEY_M_MONITOR
#define KEY_M_N5	KEY_M_UNLOCK

class mEvent {
public:
	enum Type {
		None = 0,
		Timer = 1,
		TouchBegin = 2,
		TouchUpdate = 3,
		TouchEnd = 4,
		TouchRaw = 5,
		KeyPress = 6,
		KeyRelease = 7,
		FocusIn = 8,
		FocusOut = 9,
		EditEnd = 10,
		AlertEnd = 11,
		ComboxEnd = 12,
		KeyboardEnd = 13,
		ScrollEnd = 14,
		User = 1000, // first user event id
		MaxUser = 65535 // last user event id
	};

	mEvent(Type t);
	~mEvent();

	static void process(void);
	static void post(mEvent *e);
	static void doKeyEvent(struct input_event *ev);
	static void doTouchEvent(struct input_event *ev);
	static void doTouchData(void);
	static void doInput(int fd);
	static void doFifo(mObject *obj);
	static void doFlush(void);
	static void mute(void);

public:
	Type type;
	uint32_t wParam;
	uint32_t lParam;
	uint32_t lParam2;
};

#endif
