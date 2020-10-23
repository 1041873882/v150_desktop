
#include "sys.h"
#include "sFifo.h"
#include "SysSound.h"
#include "SysSecurity.h"
#include "mFB.h"
#include "mWindow.h"
#include "mEvent.h"

#define ABS_MT_SLOT             0x2f    /* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR      0x30    /* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR      0x31    /* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR      0x32    /* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR      0x33    /* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION      0x34    /* Ellipse orientation */
#define ABS_MT_POSITION_X       0x35    /* Center X ellipse position */
#define ABS_MT_POSITION_Y       0x36    /* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE        0x37    /* Type of touching device */
#define ABS_MT_BLOB_ID          0x38    /* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID      0x39    /* Unique ID of initiated contact */
#define ABS_MT_PRESSURE         0x3a    /* Pressure on contact area */

static sFifo sys_event_fifo; //ui事件

static float sys_tp_sx = 1.0; //X轴缩放
static float sys_tp_sy = 1.0; //Y轴缩放

static int sys_tp_mode = 0; //0:电容屏 1:电阻屏
int sys_tp_data[7]; //电阻屏校准数据

#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

static void sys_tp_detect(int fd, int num)
{
	int mode = -1;

{
	char s[256];
	sprintf(s, "/sys/class/input/event%d/device/name", num);
	FILE *fp = fopen(s, "r");
	if (fp) {
		fscanf(fp, "%s", s);
		fclose(fp);
		if (!strcmp(s, "gslX680"))
			mode = 0;
		else if (!strcmp(s, "NS2009"))
			mode = 1;
	}
}

	if (mode == 0) { //电容屏
		sys_tp_mode = 0;
		uint8_t ab[(ABS_MAX + 1) / 8];
		ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(ab)), ab);
		if (test_bit(ABS_MT_POSITION_X, ab) && test_bit(ABS_MT_POSITION_Y, ab)) {
			struct input_absinfo d;
			float x = 1, y = 1;
			if(!ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &d)) {
				x = d.maximum-d.minimum;
			}
            		if(!ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &d)) {
            			y = d.maximum-d.minimum;
			}
			if (x != fb.m_width)
				sys_tp_sx = (float)fb.m_width/x;
			if (y != fb.m_height)
				sys_tp_sy = (float)fb.m_height/y;
            	}
	} else if (mode == 1) { //电阻屏
		sys_tp_mode = mode;
		memset(sys_tp_data, 0, sizeof(sys_tp_data));

		FILE *fp = fopen("/dnake/cfg/pointercal", "r");
		if (fp) {
			for(int i=0; i<7; i++)
				fscanf(fp, "%d", &sys_tp_data[i]);
			fclose(fp);
		}
		if (sys_tp_data[6] == 0) {
			char url[128];
			if (fb.m_width == 800)
				strcpy(url, "/dnake/etc/pointercal_800x480");
			else
				strcpy(url, "/dnake/etc/pointercal");
			fp = fopen(url, "r");
			if (fp) {
				for(int i=0; i<7; i++)
					fscanf(fp, "%d", &sys_tp_data[i]);
				fclose(fp);
			}
		}
	}
}

mEvent::mEvent(Type t) : type(t)
{
	wParam = 0;
	lParam = 0;
}

mEvent::~mEvent()
{
}

static struct timeval sys_mute_tv;
static struct timeval sys_touch_tv;

void mEvent::doKeyEvent(struct input_event *ev)
{
	//ev.value 0:放开 1:按下
	if (ev->value) {
		sound.press();
	}

	mEvent e(ev->value ? KeyPress : KeyRelease);
	switch(ev->code) {
	case KEY_M_UNLOCK: //开锁
		e.wParam = KEY_M_UNLOCK;
		break;

	case KEY_M_ANSWER: //摘机
		e.wParam = KEY_M_ANSWER;
		break;

	case KEY_M_MENU: //预留
		e.wParam = KEY_M_MENU;
		break;

	case KEY_M_CALL: //呼叫
		e.wParam = KEY_M_CALL;
		break;

	case KEY_M_MONITOR: //监视
		e.wParam = KEY_M_MONITOR;
		break;

	default:
		return;
	}
	mEvent::post(&e);
}

typedef struct {
	int x;
	int y;
	int m;
} tp_data_t;

static tp_data_t mTp = {0, 0, 0};
static tp_data_t mLast = {0, 0, 0};
sFifo eEventData;

void mEvent::doTouchEvent(struct input_event *ev)
{
	static int have = 0;
	if (ev->type == EV_ABS) {
		switch (ev->code) {
		case ABS_MT_POSITION_X:
			if (sys_tp_sx != 1.0)
				mTp.x = sys_tp_sx*ev->value;
			else
				mTp.x = ev->value;
			break;

		case ABS_MT_POSITION_Y:
			if (sys_tp_sy != 1.0)
				mTp.y = sys_tp_sy*ev->value;
			else
				mTp.y = ev->value;
			break;

		case ABS_MT_TOUCH_MAJOR:
			mTp.m = ev->value;
			have = 1;
			break;
		}
	} else if (ev->type == EV_SYN) {
		if (have) {
			mEvent::doTouchData();
		}
		have = 0;
	}
}

extern int debug_x, debug_y;

void mEvent::doTouchData(void)
{
	tp_data_t tp = mTp;

	if (sys_tp_mode) {
		int x = tp.x;
		int y = tp.y;
		tp.x = (sys_tp_data[2]+sys_tp_data[0]*x + sys_tp_data[1]*y)/sys_tp_data[6];
		tp.y = (sys_tp_data[5]+sys_tp_data[3]*x + sys_tp_data[4]*y)/sys_tp_data[6];
	} else {
		if (fb.height() == 480 || sys_tp_sx != 1.0) {
			//触摸屏线太稀,边沿做特殊处理
			int yy = 12*sys_tp_sy;
			if (tp.y+yy > fb.height()) {
				tp.y -= yy;
			} else if (tp.y < yy) {
				tp.y += yy;
			}
		}
	}

	if (sys_factory) {
		debug_x = tp.x;
		debug_y = tp.y;
		mObject::sys_refresh = 1;
	}

	mEvent e(TouchRaw);
	e.wParam = tp.m;
	e.lParam = mTp.x | (mTp.y<<16);
	e.lParam2 = tp.x | (tp.y<<16);
	mEvent::post(&e);

	if (tp.m == mLast.m)
		return;

	if (mWindow::window) {
		mWindow::window->doTouch(tp.x, tp.y, tp.m);
	}

	mObject *p = NULL;
	while (sys_event_fifo.used()) {
		mObject *p2;
		sys_event_fifo.get(&p2, sizeof(p2));
		if (p == NULL || p2->level() >= p->level())
			p = p2;
	}
	if (p) {
		if (tp.m) {
			if (mObject::focus() != p) {
				mEvent e(TouchBegin);
				e.wParam = (uint32_t)p;
				mEvent::post(&e);
				sound.press();
				p->setFocus(1);
			}
		} else {
			if (mObject::focus() == p) {
				mEvent e(TouchEnd);
				e.wParam = (uint32_t)p;
				mEvent::post(&e);
				p->setFocus(0);
			} else {
				if (mWindow::window)
					mWindow::window->setFocus(1);
			}
		}
	} else {
		if (mWindow::window)
			mWindow::window->setFocus(1);
	}
	//printf("xy: %d %d %d\n", tp.x, tp.y, tp.m);
	mLast = mTp;
}

void mEvent::doInput(int fd)
{
	while (1) {
		struct input_event ev;
		int r = ::read(fd, &ev, sizeof(ev));
		if (r > 0) {
			int ok = 0;
			if (fb.enabled() && __ts(sys_mute_tv) > 500)
				ok = 1;
			else if (!fb.enabled() && __ts(sys_mute_tv) > 1500)
				ok = 1;
			if (ok) {
				eEventData.put((uint8_t *)&ev, sizeof(ev));
				fb.enable(1);
			}
		} else
			break;
	}
}

void mEvent::doFifo(mObject *obj)
{
	sys_event_fifo.put(&obj, sizeof(obj));
}

void mEvent::doFlush(void)
{
	sys_event_fifo.flush();
}

#define MAX_EVENT_SZ	4

static void *sys_input_thread(void *)
{
	sleep(1);

	int event[MAX_EVENT_SZ];
	for(int i=0; i<MAX_EVENT_SZ; i++) {
		char s[128];
		sprintf(s, "/dev/input/event%d", i);
		event[i] = ::open(s, O_RDONLY);;
		if (event[i] > 0) {
			fcntl(event[i], F_SETFL, O_NONBLOCK);
			sys_tp_detect(event[i], i);
		}
	}

	struct timeval tv;
	fd_set rfds;
	while(1) {
		int max = -1;
		FD_ZERO(&rfds);

		for(int i=0; i<MAX_EVENT_SZ; i++) {
			if (event[i] > 0) {
				FD_SET(event[i], &rfds);
				if (event[i] > max) {
					max = event[i];
				}
			}
		}

		tv.tv_sec = 0;
		tv.tv_usec = 300*1000;
		int r = select(max+1, &rfds, NULL, NULL, &tv);
		if (r == 0) {
			if (mLast.m) { //未收到放开事件
				struct input_event e;
				e.type = EV_ABS;
				e.code = ABS_MT_TOUCH_MAJOR;
				e.value = 0;
				eEventData.put((uint8_t *)&e, sizeof(e));

				e.type = EV_SYN;
				eEventData.put((uint8_t *)&e, sizeof(e));
			}
		} else {
			for(int i=0; i<MAX_EVENT_SZ; i++) {
				if (event[i] > 0 && FD_ISSET(event[i], &rfds)) {
					mEvent::doInput(event[i]);
				}
			}
		}
	}
}

static sFifo sys_event;
static int sys_wakeup[2] = {-1, -1};
void ui_main_process(void);

void mEvent::process(void)
{
	{
		pthread_t pid;
		if (pthread_create(&pid, NULL, sys_input_thread, NULL) != 0)
			perror("pthread_create sys_input_thread!\n");
	}

	if (::pipe(sys_wakeup) < 0)
		exit(-1);
	::write(sys_wakeup[1], "W", 1);

	while(1) {
		struct timeval tv;
		fd_set rfds;

		FD_ZERO(&rfds);
		FD_SET(sys_wakeup[0], &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 40*1000;
		::select(sys_wakeup[0]+1, &rfds, NULL, NULL, &tv);

		if (FD_ISSET(sys_wakeup[0], &rfds)) {
			char d[16];
			::read(sys_wakeup[0], d, sizeof(d));
		}

		while (1) {
			struct input_event ev;
			int r = eEventData.get((uint8_t *)&ev, sizeof(ev));
			if (r <= 0)
				break;
			if (ev.type == EV_KEY)
				mEvent::doKeyEvent(&ev);
			else if (ev.type == EV_ABS || ev.type == EV_SYN)
				mEvent::doTouchEvent(&ev);
		}

		while(sys_event.used() >= sizeof(mEvent)) {
			mEvent e(None);
			sys_event.get((uint8_t *)&e, sizeof(e));
			if (mWindow::window != NULL)
				mWindow::window->doEvent(&e);
		}

		mWindow::process();
		if (mWindow::window != NULL)
			mWindow::window->doPaint();
		if (mWindow::window != NULL)
			mWindow::window->doTimer();

		ui_main_process();
	}
}

void mEvent::post(mEvent *e)
{
	if (sys_wakeup[1] > 0) {
		sys_event.put((uint8_t *)e, sizeof(mEvent));
		::write(sys_wakeup[1], "W", 1);
	}
}

void mEvent::mute(void)
{
	gettimeofday(&sys_mute_tv, NULL);
	eEventData.flush();
}
