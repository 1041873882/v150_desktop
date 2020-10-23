
#include "mFT.h"
#include "mFB.h"
#include "mScrollText.h"

mScrollText::mScrollText()
{
	m_name = "mScrollText";
	m_scroll_y = -1;
}

mScrollText::~mScrollText()
{
}

void mScrollText::doTouch(int x, int y, int m)
{
	if (m_visible && m_enable && x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height)) {
		mEvent::doFifo(this);
	} else {
		if (mObject::focus() != this) {
			mEvent e(mEvent::ScrollEnd);
			e.wParam = (uint32_t)this;
			mEvent::post(&e);
		}
	}
	mText::doTouch(x, y, m);
}

void mScrollText::doEvent(mEvent *e)
{
	mText::doEvent(e);

	if (!m_visible || !m_enable)
		return;

	if (e->type == mEvent::TouchRaw) {
		int m = e->wParam;
		if (m) {
			int x = e->lParam2&0xFFFF;
			int y = (e->lParam2>>16)&0xFFFF;

			int ok = 0;
			if (x >= m_x && y >= m_y && x <= (m_x+m_width) && y <= (m_y+m_height))
				ok = 1;

			if (ok && !mObject::sys_refresh) {
				if (m_scroll_y < 0) {
					m_scroll_y = y;
				} else {
					int yy = abs(y-m_scroll_y);
					if (yy >= m_pixel) {
						if (y < m_scroll_y) {
							if (m_scroll) {
								m_mline_offset += m_pixel;
								this->paint();
							}
						} else {
							if (m_mline_offset > 0) {
								m_mline_offset -= m_pixel;
								if (m_mline_offset < 0)
									m_mline_offset = 0;
								this->paint();
							}
						}
						m_scroll_y = y;
					}
				}
			}
		} else
			m_scroll_y = -1;
	}
}

void mScrollText::scrollTo(int offset)
{
	m_mline_offset += offset;
}
