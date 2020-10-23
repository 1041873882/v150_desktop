
#include "mFB.h"
#include "wMain.h"
#include "wCalibrate.h"

wCalibrate::wCalibrate() : mWindow("calibrate")
{
	m_cross.setParent(this);
	m_cross.load(m_style, "cross");

	m_idx = 0;
	this->moveCross(50, 50);
	m_ts = time(NULL);
}

wCalibrate::~wCalibrate()
{
}

void wCalibrate::doPaint(void)
{
	mWindow::doPaint();
}

void wCalibrate::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchRaw) {
		this->doXY(e->wParam, e->lParam&0xFFFF, (e->lParam>>16)&0xFFFF);
		m_ts = time(NULL);
	}
}

void wCalibrate::doTimer(void)
{
	mWindow::doTimer();
	if (labs(time(NULL)-m_ts) >= 20) {
		wMain *w = new wMain();
		w->show();
	}
}

void wCalibrate::moveCross(int x, int y)
{
	int x2 = x-m_cross.width()/2;
	int y2 = y-m_cross.height()/2;
	m_cross.move(x2, y2);

	m_cal.xfb[m_idx] = x;
	m_cal.yfb[m_idx] = y;
}

extern int sys_tp_data[7];

void wCalibrate::doXY(int m, int x, int y)
{
	if (m == 0) {
		fprintf(stderr, "wCalibrate::perform: xy[%d %d]\n", x, y);

		m_cal.x[m_idx] = x;
		m_cal.y[m_idx] = y;
		m_idx++;
		if (m_idx > 4) {
			this->perform(&m_cal);

			sys_tp_data[0] = m_cal.a[1];
			sys_tp_data[1] = m_cal.a[2];
			sys_tp_data[2] = m_cal.a[0];
			sys_tp_data[3] = m_cal.a[4];
			sys_tp_data[4] = m_cal.a[5];
			sys_tp_data[5] = m_cal.a[3];
			sys_tp_data[6] = m_cal.a[6];
			for(int i=0; i<7; i++) {
				fprintf(stderr, "wCalibrate::perform: sys_tp_data[%d] = %d;\n", i, sys_tp_data[i]);
			}
			FILE *fp = fopen("/dnake/cfg/pointercal", "w+");
			if (fp) {
				for(int i=0; i<7; i++)
					fprintf(fp, "%d ", sys_tp_data[i]);
				fclose(fp);
				system("sync");
			}

			wMain *w = new wMain();
			w->show();
			return;
		}

		else if (m_idx == 1) {
			this->moveCross(fb.width()-50, 50);
		} else if (m_idx == 2) {
			this->moveCross(fb.width()-50, fb.height()-50);
		} else if (m_idx == 3) {
			this->moveCross(50, fb.height()-50);
		} else if (m_idx == 4) {
			this->moveCross(fb.width()/2, fb.height()/2);
		}
	}
}

int wCalibrate::perform(calibration_t *cal)
{
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	n = x = y = x2 = y2 = xy = 0;
	for(int j=0; j<5; j++) {
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		fprintf(stderr, "wCalibrate::perform: determinant is too small -- %f\n",det);
		return 0;
	}

	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

	z = zx = zy = 0;
	for(int j=0; j<5; j++) {
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j]*cal->x[j]);
		zy += (float)(cal->xfb[j]*cal->y[j]);
	}

	cal->a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	fprintf(stderr, "wCalibrate::perform: %f %f %f\n",(a*z + b*zx + c*zy),
		(b*z + e*zx + f*zy),
		(c*z + f*zx + i*zy));

	z = zx = zy = 0;
	for(int j=0; j<5; j++) {
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j]*cal->x[j]);
		zy += (float)(cal->yfb[j]*cal->y[j]);
	}

	cal->a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	fprintf(stderr, "wCalibrate::perform: %f %f %f\n",(a*z + b*zx + c*zy),
		(b*z + e*zx + f*zy),
		(c*z + f*zx + i*zy));

	cal->a[6] = (int)scaling;
	return 1;
}
