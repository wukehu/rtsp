#ifndef __THREAD_H__
#define __THREAD_H__

#define THREAD_ID_NOINIT -1

#ifdef   WIN32
typedef  HANDLE						thread_t;			    //线程ID
typedef  LPTHREAD_START_ROUTINE		_start_routine;		    //线程函数
typedef  CRITICAL_SECTION			lock_t;				    //锁
typedef	 HANDLE						sem_t;				    //信号量
typedef	 DWORD						PUB_THREAD_RESULT;		//线程函数返回结果
#define	 THREAD_CALL			    WINAPI					//函数调用方式
#define  CREATE_THREAD_FAIL		    NULL					//创建线程失败
#define  snprintf                   _snprintf
#define  LOCK_INIT(x)               InitializeCriticalSection(x)
#define  LOCK(x)                    EnterCriticalSection(x)
#define  UNLOCK(x)                  LeaveCriticalSection(x)
#define  LOCK_UNINIT(x)             DeleteCriticalSection(x)
#else
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

typedef  pthread_t					thread_t;			//线程ID
typedef  void *(*start_routine)(void*);
typedef  start_routine				_start_routine;		//线程函数
typedef  pthread_mutex_t			lock_t;				//锁
typedef	 sem_t						sem_t;				//信号量
typedef	 void*						THREAD_RESULT;		//线程函数返回结果
#define	 THREAD_CALL									//函数调用方式
#define  CREATE_THREAD_FAIL		0						//创建线程失败
#define  LOCK_INIT(x)               pthread_mutex_init(x, NULL)
#define  LOCK(x)                    pthread_mutex_lock(x)
#define  UNLOCK(x)                  pthread_mutex_unlock(x)
#define  LOCK_UNINIT(x)             pthread_mutex_destroy(x)

#endif

thread_t createThread(_start_routine start_routine, void* pParam, bool *pRun);

void exitThread(thread_t *pThreadID, bool *pRun, bool bCance = false);

#endif /*__THREAD_H__*/

