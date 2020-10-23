
#ifndef __M_FT_H__
#define __M_FT_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "types.h"

class mFT {
public:
	mFT();
	~mFT();

	int load(FT_ULong uc);
	int pixel(int x, int y);
	int width(void);
	int height(void);
	int left(void);
	int top(void);
	FT_Vector advance(void);

	void setPixel(int val);

	static int u8_uc(const char *src, uint16_t *uc, int sz);
	static int is_u8(const char *data, int length);
	static int gbk_u8(const char *s, char *d, size_t d_sz);

protected:
	FT_Library m_library;
	FT_Face m_ascii;
	FT_Face m_font;
	FT_Bitmap m_bmp;
	FT_GlyphSlot m_glyph;
};

extern mFT ft;

#endif
