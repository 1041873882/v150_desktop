#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__
#include <pthread.h>
#include "curl.h"
#include <map>

using namespace std;

#define MULTI_REQUEST_NUM	10

typedef map<string,string> dict;
typedef int (*http_callback_t)(int code, string response);

class HttpRequest
{
public:
	HttpRequest();
	~HttpRequest();
	void run(void);
	int post(const char* url, dict *headers, const char* body, http_callback_t callback);
	int get(const char* url, dict *headers, const char* body, http_callback_t callback);
	static size_t writeData(void *ptr, size_t size, size_t count, void *stream);
private:
	int headersDictCopy(struct curl_slist **slist_headers, dict *dict_headers);
	int addHttpRequest(int method, const char* url, dict *headers, const char* body, http_callback_t callback);
	int removeHttpRequest(int idx);
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
	int    m_task;
	int    m_running;
	CURLM *m_curlMulti;
	CURL  *m_curl[MULTI_REQUEST_NUM];
	string m_send_buf[MULTI_REQUEST_NUM];
	string m_recv_buf[MULTI_REQUEST_NUM];
	http_callback_t    m_callback[MULTI_REQUEST_NUM];
	struct curl_slist *m_headers[MULTI_REQUEST_NUM];
	struct curl_httppost* m_body[MULTI_REQUEST_NUM];
};

extern HttpRequest http_request;

#endif
