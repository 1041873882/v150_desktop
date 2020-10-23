#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <time.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "HttpRequest.h"

#define TIMEOUT				7
#define CONNECT_TIMEOUT		5

enum {
	METHOD_HTTPPOST,
	METHOD_HTTPGET
};

static void *handle_message_thread(void *p)
{
	pthread_detach(pthread_self());
	HttpRequest *http_handle = (HttpRequest *)p;
	http_handle->run();
	return NULL;
}

HttpRequest http_request;
HttpRequest::HttpRequest()
{
	if (!m_running) {
		pthread_t pid;
		m_task = 0;
		m_running = 1;
		for (int idx = 0; idx < MULTI_REQUEST_NUM; idx++) {
			m_curl[idx] = NULL;
			m_headers[idx] = NULL;
			m_recv_buf[idx].clear();
		}
		curl_global_init(CURL_GLOBAL_ALL);
		m_curlMulti = curl_multi_init();
		/* we can optionally limit the total amount of connections this multi handle
		uses */
		curl_multi_setopt(m_curlMulti, CURLMOPT_MAXCONNECTS, (long)MULTI_REQUEST_NUM);
		pthread_cond_init(&m_cond, NULL);
		pthread_mutex_init(&m_mutex, NULL);
		if (pthread_create(&pid, NULL, handle_message_thread, this) != 0) {
			perror("pthread_create http_handle_thread!\n");
		}
	}
}

HttpRequest::~HttpRequest()
{
	m_running = 0;
	pthread_cond_signal(&m_cond);
	// wait thread exit
#ifdef WIN32
	Sleep(10);
#else
	usleep(10 * 1000);
#endif
	curl_multi_cleanup(m_curlMulti);
	curl_global_cleanup();
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
}

size_t HttpRequest::writeData(void *buffer, size_t size, size_t count, void *p)
{
	string *recv_buf = (string *)p;
	size_t len = size * count;
	recv_buf->append((char *)buffer, len);
	return len;
}

int HttpRequest::headersDictCopy(struct curl_slist **slist_headers, dict *dict_headers)
{
	if (!slist_headers || !dict_headers) {
		return -1;
	}
	char buf[256] = {0};
	for (dict::iterator iter = dict_headers->begin(); iter != dict_headers->end(); iter++) {
		snprintf(buf, sizeof(buf), "%s: %s", iter->first.c_str(), iter->second.c_str());
		*slist_headers = curl_slist_append(*slist_headers, buf);
	}
	*slist_headers = curl_slist_append(*slist_headers, "Expect:");
	return 0;
}

int HttpRequest::addHttpRequest(int method, const char* url, dict *headers, const char* body, http_callback_t callback)
{
	if (!url || url[0] == '\0') {
		return -1;
	}
	pthread_mutex_lock(&m_mutex);
	for (int idx = 0; idx < MULTI_REQUEST_NUM; ++idx) {
		if (m_curl[idx] != NULL) {
			continue;
		}
		m_curl[idx] = curl_easy_init();
		if (m_curl[idx]) {
			curl_easy_setopt(m_curl[idx], CURLOPT_URL, url);
			curl_easy_setopt(m_curl[idx], CURLOPT_NOSIGNAL, 1);
			curl_easy_setopt(m_curl[idx], CURLOPT_WRITEFUNCTION, &writeData);
			curl_easy_setopt(m_curl[idx], CURLOPT_WRITEDATA, (void *)&m_recv_buf[idx]);
			curl_easy_setopt(m_curl[idx], CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
			curl_easy_setopt(m_curl[idx], CURLOPT_TIMEOUT, TIMEOUT);
			//curl_easy_setopt(m_curl[idx], CURLOPT_VERBOSE, 1);
			if (headers) {
				headersDictCopy(&m_headers[idx], headers);
				curl_easy_setopt(m_curl[idx], CURLOPT_HTTPHEADER, m_headers[idx]);
			}
			if (strstr(url, "https://") != NULL) {
				curl_easy_setopt(m_curl[idx], CURLOPT_SSL_VERIFYPEER, 0L);   //不验证证书和HOST
				curl_easy_setopt(m_curl[idx], CURLOPT_SSL_VERIFYHOST, 0L);
			}
			if (method == METHOD_HTTPPOST) {
				if (body) {
					m_send_buf[idx] = body;
					curl_easy_setopt(m_curl[idx], CURLOPT_POSTFIELDS, m_send_buf[idx].c_str());
				} else {
					curl_easy_setopt(m_curl[idx], CURLOPT_POSTFIELDS, "");
				}
				curl_easy_setopt(m_curl[idx], CURLOPT_POST, 1);
			} else if (method == METHOD_HTTPGET) {
				curl_easy_setopt(m_curl[idx], CURLOPT_HTTPGET, 1);
			}
			m_task++;
			m_callback[idx] = callback;
			curl_multi_add_handle(m_curlMulti, m_curl[idx]);
			pthread_cond_signal(&m_cond);
			pthread_mutex_unlock(&m_mutex);
			return 0;
		} else {
			pthread_mutex_unlock(&m_mutex);
			return -1;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return -1;
}

int HttpRequest::removeHttpRequest(int idx)
{
	if (idx >= MULTI_REQUEST_NUM) {
		return -1;
	}
	curl_multi_remove_handle(m_curlMulti, m_curl[idx]);
	curl_easy_cleanup(m_curl[idx]);
	m_curl[idx] = NULL;
	m_callback[idx] = NULL;
	if (m_body[idx]) {
		curl_formfree(m_body[idx]);
		m_body[idx] = NULL;
	}
	m_send_buf[idx].clear();
	m_recv_buf[idx].clear();
	m_task--;
	if (m_headers[idx]) {
		curl_slist_free_all(m_headers[idx]);
		m_headers[idx] = NULL;
	}
	return 0;
}

void HttpRequest::run()
{
	CURLMsg* msg;
	int running_handles = 0;
	while (m_running) {
		pthread_mutex_lock(&m_mutex);
		if (m_task == 0) {
			pthread_cond_wait(&m_cond, &m_mutex);
			pthread_mutex_unlock(&m_mutex);
			continue;
		}
		/*
		* 调用curl_multi_perform函数执行curl请求
		* url_multi_perform返回CURLM_CALL_MULTI_PERFORM时，表示需要继续调用该函数直到返回值不是CURLM_CALL_MULTI_PERFORM为止
		* running_handles变量返回正在处理的easy curl数量，running_handles为0表示当前没有正在执行的curl请求
		*/
		curl_multi_perform(m_curlMulti, &running_handles);
		if (running_handles) {
			int     max_fd = -1;
			fd_set  fd_read;
			fd_set  fd_write;
			fd_set  fd_except;
			struct timeval timeout_tv;

			FD_ZERO(&fd_read);
			FD_ZERO(&fd_write);
			FD_ZERO(&fd_except);
			curl_multi_fdset(m_curlMulti, &fd_read, &fd_write, &fd_except, &max_fd);
			long wait_time;
			if (curl_multi_timeout(m_curlMulti, &wait_time)) {
				perror("curl_multi_timeout error!\n");
			}
			if (wait_time <= 0) {
				wait_time = 10;
			}
			/**
			* When max_fd returns with -1,
			* you need to wait a while and then proceed and call curl_multi_perform anyway.
			* How long to wait? I would suggest 100 milliseconds at least,
			* but you may want to test it out in your own particular conditions to find a suitable value.
			*/
			if (-1 == max_fd) {
				pthread_mutex_unlock(&m_mutex);
#ifdef WIN32
				Sleep(wait_time);
#else
				usleep(wait_time*1000);
#endif
				pthread_mutex_lock(&m_mutex);
			} else {
				timeout_tv.tv_sec = wait_time / 1000;
				timeout_tv.tv_usec = (wait_time % 1000) * 1000;
				pthread_mutex_unlock(&m_mutex);
				if (::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv) < 0) {
					perror("http select error!\n");
				}
				pthread_mutex_lock(&m_mutex);
			}
		}
		int msgs_left;
		while ((msg = curl_multi_info_read(m_curlMulti, &msgs_left))) {
			if (CURLMSG_DONE == msg->msg) {
				CURL* curl = msg->easy_handle;
				for (int idx = 0; idx < MULTI_REQUEST_NUM; ++idx) {
					if (curl == m_curl[idx]) {
						if (m_callback[idx]) {
							int response_code;
							curl_easy_getinfo(m_curl[idx], CURLINFO_RESPONSE_CODE, &response_code);
							m_callback[idx](response_code, m_recv_buf[idx]);
						}
						removeHttpRequest(idx);
						break;
					}
				}
			}
		}
		pthread_mutex_unlock(&m_mutex);
	}
}

int HttpRequest::post(const char* url, dict *headers, const char* body, http_callback_t callback)
{
	return addHttpRequest(METHOD_HTTPPOST, url, headers, body, callback);
}

int HttpRequest::get(const char* url, dict *headers, const char* body, http_callback_t callback)
{
	return addHttpRequest(METHOD_HTTPGET, url, headers, body, callback);
}

