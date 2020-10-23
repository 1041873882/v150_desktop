
#ifndef __W_SETUP_NETWORK_LABEL_H__
#define __W_SETUP_NETWORK_LABEL_H__

#include "mWindow.h"
#include "mEdit.h"

class wSetupNetworkLabel : public mWindow {
public:
	wSetupNetworkLabel();
	virtual ~wSetupNetworkLabel();

	void doEvent(mEvent *e);
	void save(void);

private:
	mText m_ip_text;
	mEdit m_ip;
	mText m_mask_text;
	mEdit m_mask;
	mText m_gateway_text;
	mEdit m_gateway;
	mText m_dns_text;
	mEdit m_dns;
	mText m_server_text;
	mEdit m_server;
	mButton m_ok;
};

#endif
