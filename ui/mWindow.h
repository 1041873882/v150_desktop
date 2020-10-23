
#ifndef __M_WINDOWS_H__
#define __M_WINDOWS_H__

#include "dxml.h"
#include "AutoMutex.h"
#include "mObject.h"
#include "mEvent.h"

class mWindow : public mObject {
public:
	mWindow(const char *name);
	virtual ~mWindow();

	void doTouch(int x, int y, int m);
	void show(void);
	void doPaint(void);

public:
	static void process(void);

	static pthread_mutex_t m_mutex;
	static mWindow *window;

protected:
	dxml m_style;
};

#endif