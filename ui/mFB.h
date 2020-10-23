
#ifndef __M_FB_H__
#define __M_FB_H__

#include <time.h>
#include "types.h"

class mFB {
public:
	mFB();
	~mFB();

	void pixel(int x, int y, uint32_t c);
	int width(void) { return m_width; };
	int height(void) { return m_height; };
	void clean(void);
	void sync(void);

	void osd(int x, int y, int w, int h);
	void enable(int val);
	int enabled(void);

	int timeout(void) { return labs(time(NULL)-m_ts); };
	void bright(int val);

	int used(void);

public:
	int m_fd;
	int m_width;
	int m_height;
	uint32_t *m_bits;
	uint32_t *m_argb;

	int m_osd_x, m_osd_y, m_osd_w, m_osd_h;
	int m_osd_alpha;

	time_t m_ts;
	int m_enabled;
};

extern mFB fb;

#endif
