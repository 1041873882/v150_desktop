
#ifndef __M_PIXMAP_H__
#define __M_PIXMAP_H__

#include "mObject.h"

class mPixmap : public mObject {
public:
	mPixmap();
	virtual ~mPixmap();

	int loadFile(const char *url);
	int png(const char *url);
	int jpeg(const char *url);
	void recycle(void);

	void load(dxml &p, const char *zone);
	int width(void) { return m_width; };
	int height(void) { return m_height; };
	void alpha(uint8_t val) { m_alpha = val; };
	int resize(int w, int h);
	int scale(void *src, int sw, int sh, void *dst, int dw, int dh);

	void doPaint(void);

protected:
	uint32_t *m_argb;
	uint8_t m_alpha;
	int m_jpeg;
};

#endif
