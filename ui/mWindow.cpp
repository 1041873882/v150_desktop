
#include <queue>
#include "sys.h"
#include "mFB.h"
#include "mWindow.h"

static std::queue<mWindow *> mWindowQueue;

mWindow *mWindow::window = NULL;

pthread_mutex_t mWindow::m_mutex = PTHREAD_MUTEX_INITIALIZER;

mWindow::mWindow(const char *name)
{
	m_name = "mWindow";
	AutoMutex m(&m_mutex);

	char s[256], root[256];
	struct stat st;
	if (sys.m_limit == 220) {
		if (!stat("/var/style_nt/", &st)) {
			sprintf(root, "/var/style_nt/%dx%d/%s", fb.width(), fb.height(), name);
		} else {
			sprintf(root, "/dnake/style_nt/%dx%d/%s", fb.width(), fb.height(), name);
		}
	} else {
		if (!stat("/var/style/", &st)) { //var目录存在style,使用调试style
			sprintf(root, "/var/style/%dx%d/%s", fb.width(), fb.height(), name);
		} else {
			sprintf(root, "/dnake/style/%dx%d/%s", fb.width(), fb.height(), name);
		}
	}

	sprintf(s, "%s/%s", root, sys.style());
	if (stat(s, &st)) {
		sprintf(s, "%s/style.xml", root);
	}
	m_style.loadFile(s);
	m_style.setText("/style/root", root);
}

mWindow::~mWindow()
{
}

void mWindow::doTouch(int x, int y, int m)
{
	if (m_visible) {
		if (x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
			mEvent::doFifo(this);
		}
	}
	mObject::doTouch(x, y, m);
}

void mWindow::show(void)
{
	AutoMutex m(&m_mutex);

	mObject::show();
	if (m_parent == NULL) {
		mEvent::doFlush(); //界面切换，清空未处理事件，避免内存异常访问
		if (window) {
			mWindowQueue.push(window);
		}
		mWindow::window = this;
		this->paint();
	}
	fb.bright(sys.lcd.bright());
}

//#define PAINT_DEBUG	1
int debug_x = 0, debug_y = 0;

void ipwd_force_wait(int val); //IP冲突检测，界面刷新时不处理

void mWindow::doPaint(void)
{
	if (m_parent == NULL) {
		AutoMutex m(&m_mutex);
		if (mObject::sys_refresh) {
			mObject::sys_refresh = 0;

			ipwd_force_wait(1);
#ifdef PAINT_DEBUG
			struct timeval tv;
			gettimeofday(&tv, NULL);
#endif

			fb.clean();
			mObject::doPaint();
			if (sys_factory) {
				for(int i=0; i<fb.m_width; i++) {
					fb.pixel(i, debug_y, 0xFFFF0000);
					fb.pixel(i, debug_y+1, 0xFFFF0000);
				}
				for(int i=0; i<fb.m_height; i++) {
					fb.pixel(debug_x, i, 0xFFFF0000);
					fb.pixel(debug_x+1, i, 0xFFFF0000);
				}
			}

			fb.sync();
			ipwd_force_wait(0);
#ifdef PAINT_DEBUG
			printf("mWindow::doPaint: %d\n", __ts(tv));
#endif
		}
	} else
		mObject::doPaint();
}

void mWindow::process(void)
{
	AutoMutex m(&m_mutex);
	if (!mWindowQueue.empty()) {
		mWindow *w = mWindowQueue.front();
		if (w != NULL) {
			delete w;
		}
		mWindowQueue.pop();
	}
}
