
#ifndef __W_NT_INTERCOM_LOGGER_H__
#define __W_NT_INTERCOM_LOGGER_H__

#include "wNtWindow.h"

class wNtIntercomLoggerLabel : public wNtWindow {
public:
	wNtIntercomLoggerLabel();
	virtual ~wNtIntercomLoggerLabel();

	void doEvent(mEvent *e);

	void showData(int n);
	void showSelect(void);

private:
	mButton m_title;
	mButton m_data[8];
	mPixmap m_type[8];
	mText m_index[8];
	mText m_host[8];
	mText m_ts[8];
	mPixmap m_jpeg[8];

	mButton m_jpeg_view;
	int m_mode;

	int m_first;
	int m_max;
	int m_select;
	int m_offset;
};

#endif
