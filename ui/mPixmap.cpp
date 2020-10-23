
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>

#include <jpeg/jpeglib.h>
#include <png/png.h>

#include "mFB.h"
#include "mPixmap.h"
#include "mWindow.h"

mPixmap::mPixmap()
{
	m_name = "mPixmap";
	m_argb = NULL;
	m_alpha = 0xFF;
	m_jpeg = 0;
}

mPixmap::~mPixmap()
{
	this->recycle();
}

void mPixmap::recycle(void)
{
	if (m_argb) {
		delete []m_argb;
		m_argb = NULL;
	}
	m_width = 0;
	m_height = 0;
}

int mPixmap::loadFile(const char *url)
{
	this->recycle();
	m_jpeg = 0;

	if (url == NULL || strlen(url) < 5)
		return -1;

	const char *ft = url+strlen(url)-4;
	if (!strcasecmp(ft, ".png"))
		return png(url);
	else if (!strcasecmp(ft, ".jpg"))
		return jpeg(url);
	return -1;
}

int mPixmap::png(const char *url)
{
	FILE *fp = fopen(url, "rb");
	if (fp == NULL)
		return -1;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	m_width = png_get_image_width(png_ptr, info_ptr);
	m_height = png_get_image_height(png_ptr, info_ptr);
	int type = png_get_color_type(png_ptr, info_ptr);

	m_argb = new uint32_t[m_height*m_width];
	if (m_argb) {
		int block = (type == 6 ? 4 : 3); //type == 6 有Alpha，其他无
		int n = 0;

		png_bytep *row = png_get_rows(png_ptr, info_ptr);
		if (block == 4) {
			for(int i=0; i<m_height; i++) {
				for(int j=0; j<(block*m_width); j+=block) {
					uint8_t *p = (uint8_t *)&row[i][j];
					//转换成ARGB格式
					m_argb[n++] = ((p[3]<<24) | p[0]<<16 | p[1]<<8 | p[2]);
				}
			}
		} else {
			for(int i=0; i<m_height; i++) {
				for(int j=0; j<(block*m_width); j+=block) {
					uint8_t *p = (uint8_t *)&row[i][j];
					//转换成ARGB格式
					m_argb[n++] = ((0xFF<<24) | p[0]<<16 | p[1]<<8 | p[2]);
				}
			}
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	return 0;
}

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

static void my_error_exit(j_common_ptr cinfo)
{
	struct my_error_mgr *myerr = (struct my_error_mgr *) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

int mPixmap::jpeg(const char *url)
{
	FILE *fp = fopen(url, "rb");
	if (fp == NULL)
		return -1;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	 /* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		fclose(fp);
		return -1;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	m_width = cinfo.output_width;
	m_height = cinfo.output_height;
	m_argb = new uint32_t[m_width*m_height];
	if (m_argb) {
		int sz = cinfo.output_width * cinfo.output_components;
		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, sz, 1);
		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, buffer, 1);

			int n = 0;
			uint32_t *p = m_argb+m_width*(cinfo.output_scanline-1);
			uint8_t *d = buffer[0];
			for(int i=0; i<m_width; i++) {
				p[i] = (0xFF<<24) | (d[n]<<16) | (d[n+1]<<8) | d[n+2];
				n += 3;
			}
		}
		m_jpeg = 1;
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	return 0;
}

void mPixmap::load(dxml &p, const char *zone)
{
	const char *root = p.getText("/style/root");
	if (root) {
		char s[512], s2[512];
		sprintf(s, "/style/%s/url", zone);
		sprintf(s2, "%s/%s", root, p.getText(s));
		this->loadFile(s2);
		mObject::load(p, zone);
	}
}

void mPixmap::doPaint(void)
{
	if (m_visible && m_argb && fb.m_argb) {
		if (m_jpeg && (m_x+m_width <= fb.m_width) && (m_y+m_height) <= fb.m_height) {
			int fb_offset = m_y*fb.m_width+m_x;
			int offset = 0;
			for(int y=0; y<m_height; y++) {
				memcpy(&fb.m_argb[fb_offset], &m_argb[offset], m_width<<2);
				offset += m_width;
				fb_offset += fb.m_width;
			}
		} else {
			if (m_alpha != 0xFF) {
				uint32_t c;
				uint8_t *p = (uint8_t *)&c;
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_width; x++) {
						c = m_argb[y*m_width+x];
						p[3] = (p[3]*m_alpha)>>8;
						fb.pixel(m_x+x, m_y+y, c);
					}
				}
			} else {
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_width; x++) {
						fb.pixel(m_x+x, m_y+y, m_argb[y*m_width+x]);
					}
				}
			}
		}
	}
	mObject::doPaint();
}

int mPixmap::resize(int w, int h)
{
	int result = -1;
	if (m_argb) {
		uint32_t *dst = new uint32_t[w*h];
		if (dst) {
			int r = this->scale(m_argb, m_width, m_height, dst, w, h);
			if (r > 0) {
				this->recycle();
				m_argb = dst;
				m_width = w;
				m_height = h;
				result = 0;
			} else {
				delete []dst;
			}
		}
	}
	return result;
}

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

int mPixmap::scale(void *src, int sw, int sh, void *dst, int dw, int dh)
{
	if (sw > 1024 && sw>dw) { //宽高缩小一半
		uint32_t *p = (uint32_t *)src;
		uint32_t *d = p;
		for(int yy=0; yy<sh; yy+=2) {
			int offset = yy*sw;
			for(int xx=0; xx<sw; xx+=2) {
				*d++ = p[offset+xx];
			}
		}
		sw /= 2;
		sh /= 2;
	}

	struct SwsContext *sws = sws_getContext(sw, sh, PIX_FMT_ARGB, dw, dh, PIX_FMT_ARGB, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	if (sws) {
		AVPicture s, d;
		avpicture_fill(&s, (uint8_t *)src, PIX_FMT_ARGB, sw, sh);
		avpicture_fill(&d, (uint8_t *)dst, PIX_FMT_ARGB, dw, dh);
		int r = sws_scale(sws, s.data, s.linesize, 0, sh, d.data, d.linesize); 
		sws_freeContext(sws);
		return r;
	}
	return -1;
}
