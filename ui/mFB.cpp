
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "mEvent.h"
#include "mFB.h"

mFB fb;

mFB::mFB()
{
	m_fd = -1;
	m_bits = NULL;
	m_argb = NULL;
	m_width = 0;
	m_height = 0;
	m_enabled = 0;
	if (this->used()) {
		m_fd = open("/dev/fb0", O_RDWR);
		if (m_fd > 0) {
			struct fb_var_screeninfo vi;
			struct fb_fix_screeninfo fi; 
			ioctl(m_fd, FBIOGET_FSCREENINFO, &fi);
			ioctl(m_fd, FBIOGET_VSCREENINFO, &vi);

			m_width = vi.xres;
			m_height = vi.yres;
			m_bits = (uint32_t *)mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
			m_argb = m_bits+m_width*m_height;
			vi.yoffset = 0;
			ioctl(m_fd, FBIOPUT_VSCREENINFO, &vi);
		} else {
			LOGE("mFB::start open /dev/fb0 error!\n");
		}
	}
	m_osd_x = m_osd_y = m_osd_w = m_osd_h = 0;
	m_osd_alpha = 0;
	this->enable(1);
}

mFB::~mFB()
{
}

void mFB::pixel(int x, int y, uint32_t c)
{
	if (m_argb && x >= 0 && y >= 0 && x < m_width && y < m_height) {
		uint8_t *d = (uint8_t *)&c;
		uint8_t *s = (uint8_t *)&m_argb[y*m_width+x];
		if (d[3] != 0xFF) { //ARGBµþ¼Ó
			if (d[3] < 3) {
			} else {
				s[0] = (((d[0]-s[0]) * d[3]) >> 8) + s[0];
				s[1] = (((d[1]-s[1]) * d[3]) >> 8) + s[1];
				s[2] = (((d[2]-s[2]) * d[3]) >> 8) + s[2];
				s[3] = 0xFF;
			}
		} else
			m_argb[y*m_width+x] = c;
	}
}

void mFB::clean(void)
{
	if (m_argb) {
		int sz = m_width * m_height;
		for(int i=0; i<sz; i++) {
			m_argb[i] = 0xFF000000;
		}
	}
}

void mFB::sync(void)
{
	if (m_bits == NULL || m_argb == NULL)
		return;

	if (m_osd_w != 0 && m_osd_h != 0) {
		int offset = m_osd_y*m_width+m_osd_x;
		if (m_osd_alpha) {
			for(int y=0; y<m_osd_h; y++) {
				for(int x=0; x<m_osd_w; x++) {
					uint8_t *p = (uint8_t *)&m_argb[offset+x];
					p[3] = m_osd_alpha;
				}
				offset += m_width;
			}
		} else {
			for(int y=0; y<m_osd_h; y++) {
				for(int x=0; x<m_osd_w; x++) {
					m_argb[offset+x] = 0x00;
				}
				offset += m_width;
			}
		}
	}

	struct fb_var_screeninfo vi;
	ioctl(m_fd, FBIOGET_VSCREENINFO, &vi);
	if (vi.yoffset == 0) {
		vi.yoffset = m_height;
		m_argb = m_bits;
	} else {
		vi.yoffset = 0;
		m_argb = m_bits+m_width*m_height;
	}
	ioctl(m_fd, FBIOPUT_VSCREENINFO, &vi);
}

void mFB::osd(int x, int y, int w, int h)
{
	if (x >= m_width || x+w > m_width || y >= m_height || y+h > m_height) {
		m_osd_x = 0;
		m_osd_y = 0;
		m_osd_w = 0;
		m_osd_h = 0;
	} else {
		m_osd_x = x;
		m_osd_y = y;
		m_osd_w = w;
		m_osd_h = h;
	}
	m_osd_alpha = 0;
}

#define SUNXI_FB_CONTROL _IOWR('C', 0x55, int)

void mFB::enable(int val)
{
	if (m_enabled != val) {
		mEvent::mute();
		if (m_fd > 0) {
			ioctl(m_fd, SUNXI_FB_CONTROL, val);
		}
	}
	m_ts = time(NULL);
	m_enabled = val;
}

int mFB::enabled(void)
{
	int val = m_enabled;
	FILE *fp = fopen("/sys/class/disp/disp/attr/enabled", "r");
	if (fp) {
		fscanf(fp, "%d", &val);
		fclose(fp);
	}
	return val;
}

void mFB::bright(int val)
{
	FILE *fp = fopen("/sys/class/disp/disp/attr/lcdbl", "w");
	if (fp) {
		fprintf(fp, "%d", val);
		fclose(fp);
	}
}

int mFB::used(void)
{
	int val = 0;
	FILE *fp = fopen("/sys/class/disp/disp/attr/used", "r");
	if (fp) {
		fscanf(fp, "%d", &val);
		fclose(fp);
	}
	return val;
}
