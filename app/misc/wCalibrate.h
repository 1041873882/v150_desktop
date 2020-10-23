
#ifndef __W_CALIBRATE_H__
#define __W_CALIBRATE_H__

#include "mWindow.h"
#include "mPixmap.h"

typedef struct {
	int x[5], xfb[5];
	int y[5], yfb[5];
	int a[7];
} calibration_t;

class wCalibrate : public mWindow {
public:
	wCalibrate();
	virtual ~wCalibrate();

	void doPaint(void);
	void doEvent(mEvent *e);
	void doTimer(void);

	void moveCross(int x, int y);
	void doXY(int m, int x, int y);
	int perform(calibration_t *cal);

private:
	mPixmap m_cross;
	calibration_t m_cal;
	int m_idx;
	time_t m_ts;
};

#endif
