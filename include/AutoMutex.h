
#ifndef __AUTO_MUTEX_H__
#define __AUTO_MUTEX_H__

#include <pthread.h>

class AutoMutex {
public:
	AutoMutex(pthread_mutex_t *mutex);
	~AutoMutex();
private:
	pthread_mutex_t *m_mutex;
};

#endif
