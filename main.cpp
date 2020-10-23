
#include "sys.h"
#include "d600.h"
#include "SysEdhcp.h"
#include "SysSound.h"
#include "SysCaller.h"
#include "SysSecurity.h"
#include "SysLogger.h"
#include "SysRs485.h"
#include "SysSmart.h"
#include "ipwatchd.h"
#include "mFB.h"
#include "wNtMain.h"
#include "wMain.h"

static int boot_b = 0; //后台启动

static void *dnake_misc_thread(void *)
{
	usleep(500*1000);
	if (boot_b) {
		dmsg req;
		req.request("/talk/slave/reset", NULL);
		sys.setEthMac();
	}
	sound.stop();
	sound.load();
	sLogger.load();
	sSecurity.start();
	rs485.start();
	ipwd_start();
	eDhcp.start();
	c600.start();
	if (sys.flash() != 0) {
		smart.start();
	}

	while (1) {
		sys.process();
		eDhcp.process();
		ipwd_process();
		usleep(100*1000);
	}
	return NULL;
}

void ui_main_process(void)
{
	if (fb.m_enabled) {
		if (fb.timeout() >= 8*60*60) {
			//对时导致时间偏差太大,刷新开屏时间
			fb.enable(1);
		} else if (fb.timeout() >= 60) {
			fb.enable(0);
			sys.admin.login = 0;
			sys.user.login = 0;

			//关屏，自动跳转到首页
			if (SysAlarm == NULL && SysMain == NULL) {
				if (sys.m_limit == 220) {
					wNtMain *w = new wNtMain();
					w->show();
				} else {
					wMain *w = new wMain();
					w->show();
				}
			}
		}
	} else {
		if (fb.enabled()) {
			//内核状态与UI状态不一致
			fb.enable(1);
		}
	}

	//静音超过8小时自动取消
	// if (sys.mute.m_enable && labs(time(NULL)-sys.mute.m_ts) >= 8*60*60) {
	// 	sys.mute.m_enable = 0;
	// }

	sCaller.ui_process();
	sSecurity.ui_process();
}

extern int lib_talk_main(int argc, const char *argv[]);

static void sys_signal(int s)
{
	exit(-1);
}

int main(int argc, const char *argv[])
{
	signal(SIGINT, sys_signal); // Interrupt from keyboard
	signal(SIGQUIT, sys_signal); // Quit from keyboard

	ui_msg_init();
	sys.load();

	if (sys.m_limit == 220) {
		wNtMain *w = new wNtMain();
		w->show();
	} else {
		wMain *w = new wMain();
		w->show();
	}

	for(int i=1; i<argc; i++) {
		if (!strcmp(argv[i], "-b")) {
			boot_b = 1;
		}
	}
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_misc_thread, NULL) != 0)
		perror("pthread_create dnake_misc_thread!\n");

	fprintf(stderr, "desktop start...\n");

	mEvent::process();
	return 0;
}
