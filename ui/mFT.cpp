
#include "mFT.h"

mFT ft;

mFT::mFT()
{
	FT_Init_FreeType(&m_library);
	FT_New_Face(m_library, "/dnake/fonts/calibri.ttf", 0, &m_ascii);
	FT_New_Face(m_library, "/dnake/fonts/DroidSansFallback.ttf", 0, &m_font);

	this->setPixel(24);
}

mFT::~mFT()
{
	FT_Done_Face(m_ascii);
	FT_Done_Face(m_font);
	FT_Done_FreeType(m_library);
}

int mFT::load(FT_ULong uc)
{
	int err = -1;

	FT_UInt index = FT_Get_Char_Index(m_ascii, uc);
	if (index > 0) {
		err = FT_Load_Glyph(m_ascii, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
		if (!err) {
			err = FT_Render_Glyph(m_ascii->glyph, FT_RENDER_MODE_NORMAL);
			m_glyph = m_ascii->glyph;
			m_bmp = m_ascii->glyph->bitmap;
		}
	} else {
		index = FT_Get_Char_Index(m_font, uc);
		if (index > 0) {
			err = FT_Load_Glyph(m_font, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
			if (!err) {
				err = FT_Render_Glyph(m_font->glyph, FT_RENDER_MODE_NORMAL);
				m_glyph = m_font->glyph;
				m_bmp = m_font->glyph->bitmap;
			}
		}
	}
	return err;
}

int mFT::pixel(int x, int y)
{
	if (m_bmp.pixel_mode == FT_PIXEL_MODE_MONO) { //单色黑白
		uint8_t *p = &m_bmp.buffer[y*m_bmp.pitch];
		int d = (p[x/8]>>(8-(x%8)))&0x01 ? 0xFF : 0x00;
		return d;
	} else if (m_bmp.pixel_mode == FT_PIXEL_MODE_GRAY) { //8位彩色
		return m_bmp.buffer[y*m_bmp.pitch+x];
	}
	return 0;
}

int mFT::width(void)
{
	return m_bmp.width;
}

int mFT::height(void)
{
	return m_bmp.rows;
}

int mFT::left(void)
{
	return m_glyph->bitmap_left;
}

int mFT::top(void)
{
	return m_glyph->bitmap_top;
}

FT_Vector mFT::advance(void)
{
	return m_glyph->advance;
}

void mFT::setPixel(int val)
{
	FT_Set_Pixel_Sizes(m_ascii, 0, val);
	FT_Set_Pixel_Sizes(m_font, 0, val);
}

int mFT::u8_uc(const char *src, uint16_t *uc, int sz)
{
	const uint8_t *p = (const uint8_t *)src;
	uint8_t *result = (uint8_t *)uc;
	int uc_sz = 0, uc_max = sz/2;

	memset(uc, 0, sz);
	while(*p) {  
		if (*p <= 0x7F) {
			//说明最高位为'0'，这意味着utf8编码只有1个字节
			*result++ = *p;  
			result++;

			uc_sz++;  
		} else if ((*p & 0xE0)== 0xC0) {
			//只保留最高三位，看最高三位是不是110，如果是则意味着utf8编码有2个字节
			char b[2] = {0, 0};
  
			b[0] = *p++ & (0x1F);//高位的后5位 (去除了头部的110这个标志位)
			b[1] = *p & (0x3F);//低位的后6位 (去除了头部的10这个标志位)

			*result++ = b[1] | ((b[0] & (0x03)) << 6);
			*result++ = b[0] >> 2;//留下其保留的三位

			uc_sz++; 
		} else if ((*p & (0xF0))== 0xE0) {//只保留最高四位，看最高三位是不是1110，如果是则意味着utf8编码有3个字节
			char b[3] = {0, 0, 0};
			b[0] = *p++ & (0x1F);
			b[1] = *p++ & (0x3F);
			b[2] = *p & (0x3F);

			*result++ = ((b[1] & (0x03)) << 6) | b[2];
			*result++ = (b[0] << 4) | (b[1] >> 2);
			uc_sz++;
		}
		p++;

		if (uc_sz >= uc_max)
			break;
	}
	return uc_sz;
}

#include "iconv.h"
int mFT::gbk_u8(const char *s, char *d, size_t d_sz)
{
	memset(d, 0, d_sz);
	iconv_t c = iconv_open("utf-8", "gbk");
	if (c) {
		size_t s_sz = strlen(s);
		char *ss = new char[s_sz+1];
		strcpy(ss, s);

		char *dst = d;
		char *src = ss;
		int r = iconv(c, &src, &s_sz, &dst, &d_sz);

		delete []ss;
		iconv_close(c);
		return r;
	}
	return -1;
}

int mFT::is_u8(const char *data, int length)
{
	if (data == NULL)
		return 0;

	int asc = 1;
	int utf8_n = 0, gbk_n = 0;
	for(int i=0; i<length; i++) {
		if (data[i] > 0 && data[i] < 0x7F)
			continue;

		asc = 0;
		if ((data[i+0] & 0xE0) == 0xC0 && i+1<=length) { //双字节格式
			if ((data[i+1] & 0xC0) == 0x80) {
				int n = data[i+0] & 0xFF;
				int n2 = data[i+1] & 0xFF;
				if ((0x81 <= n && n <= 0xFE) && (0x40 <= n2 && n2 <= 0xFE) && (n2 != 0x7F)) {
				} else {
					utf8_n++;
					i++;
					continue;
				}
			}
		} else if ((data[i+0] & 0xF0) == 0xE0 && i+2<=length) { //三字节格式
			if (((data[i+1] & 0xC0) == 0x80) && ((data[i+2] & 0xC0) == 0x80)) {
				utf8_n++;
				i += 2;
				continue;
			}
		} else if ((data[i+0] & 0xF8) == 0xF0 && i+3<=length) { //四字节格式
			if (((data[i+1] & 0xC0) == 0x80) && ((data[i+2] & 0xC0) == 0x80) && ((data[i+3] & 0xC0) == 0x80)) {
				utf8_n++;
				i += 3;
				continue;
			}
		}
		i++;
		gbk_n++;
	}
	if (asc == 0) {
		if (gbk_n > 0 && 10*utf8_n < gbk_n)
			return 0;
		return 1;
	}
	return 1;
}
