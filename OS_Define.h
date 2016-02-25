#ifndef __OS_DEFINE_H__
#define __OS_DEFINE_H__


#ifdef WIN32
#include <Windows.h>
#define ATOMIC_ADD(ptr, v)            ::InterlockedExchangeAdd((long*)ptr, v)
#define ATOMIC_SUB(ptr, v)            ::InterlockedExchangeAdd((long*)ptr, -v)
#define ATOMIC_FETCH_AND_ADD(ptr, v)  ::InterlockedExchangeAdd((long*)ptr, v) 
#define ATOMIC_FETCH_AND_SUB(ptr, v)  ::InterlockedExchangeAdd((long*)ptr, -v)
#define ATOMIC_FETCH(ptr)             ::InterlockedExchangeAdd((long*)ptr, 0)
#define ATOMIC_SET(ptr, v)            ::InterlockedExchange((long*)ptr, v) 
#define ATOMIC_CAS(ptr, cmp, v)       ((cmp) == ::InterlockedCompareExchange(var, *(val), (cmp)))
#else
#include <pthread.h>
#define ATOMIC_ADD(ptr, v)            __sync_add_and_fetch(ptr, v)
#define ATOMIC_SUB(ptr, v)            __sync_sub_and_fetch(ptr, v)
#define ATOMIC_ADD_AND_FETCH(ptr, v)  __sync_add_and_fetch(ptr, v)
#define ATOMIC_SUB_AND_FETCH(ptr, v)  __sync_sub_and_fetch(ptr, v)
#define ATOMIC_FETCH_AND_ADD(ptr, v)  __sync_fetch_and_add(ptr, v)
#define ATOMIC_FETCH_AND_SUB(ptr, v)  __sync_fetch_and_sub(ptr, v)
#define ATOMIC_FETCH(ptr)             __sync_add_and_fetch(ptr, 0)
#define ATOMIC_SET(ptr, v)            __sync_val_compare_and_swap(ptr, *(ptr), v)
#define ATOMIC_CAS(ptr, cmp, v)       __sync_bool_compare_and_swap(ptr, cmp, v)
#endif

#ifdef WIN32
	#define GET_LAST_ERROR       WSAGetLastError()
	#define snprintf             sprintf_s
	#define CLOSESOCKET          closesocket
#else
#include "errno.h"
#include <unistd.h>
	#define GET_LAST_ERROR  	 errno
	#define CLOSESOCKET     	 close
	#define SOCKET                   int
	#define GET_CURRENT_THREADID     syscall(__NR_gettid)
#endif

#endif /*__OS_DEFINE_H__*/

