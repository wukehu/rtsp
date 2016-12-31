#include "Thread.h"
#include "OS_define.h"
thread_t createThread(_start_routine start_routine, void* pParam, bool *pRun)
{	
	thread_t threadID;
	if (NULL != pRun) 
	{
		*pRun = true;
	}
#ifdef WIN32
	threadID = CreateThread(0, 0, start_routine, (LPVOID)pParam, 0, NULL);
	if ( (CREATE_THREAD_FAIL == threadID) && (NULL != pRun) ) {
		*pRun = false;
	}
#else
	int iRet = 0;
	if((iRet = pthread_create(&threadID, 0, start_routine, pParam)) != 0){
		threadID = CREATE_THREAD_FAIL;
		if (NULL != pRun)
		{
			*pRun = false;
		}		
	}
	errno = iRet;	
#endif
	return threadID;
}

void exitThread(thread_t *pThreadID, bool *pRun, bool bCance)
{
	if (THREAD_ID_NOINIT == *pThreadID)
	{
		return;
	}

	if (NULL != pRun) 
	{
		*pRun = false;
	}	

	if (bCance)
	{
		pthread_cancel(*pThreadID);
	}

#ifdef WIN32
	WaitForSingleObject(*pThreadID, INFINITE);
	CloseHandle(*pThreadID);
#else
	pthread_join(*pThreadID, NULL);
#endif
	*pThreadID = -1;
}



