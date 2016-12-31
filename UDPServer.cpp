#include "UDPServer.h"
#include "VioletError.h"
#include "VioletLog.h"
#include "UDPTransfer.h"

#define RECV_BUFFER_SIZE 1500
UDPServer::UDPServer()
{
	m_pobjListenTransfer = new UDPTransfer();
}

UDPServer::~UDPServer()
{
	if (m_pobjListenTransfer != NULL)
	{
		m_pobjListenTransfer->close();
		delete m_pobjListenTransfer;
		m_pobjListenTransfer = NULL;
	}
}

void UDPServer::setBindAddr(const InetAddr& p_objInetAddr)
{
	m_objInetAddr = p_objInetAddr;
}

const InetAddr& UDPServer::getBindAddr()const
{
	return m_objInetAddr;
}

int UDPServer::start()
{
	//1.创建传输器
	int nRet = m_pobjListenTransfer->open();
	if (VIOLET_SUCCESS != nRet)
	{
		ERROR("UDPServer", "Transfer->open() failed:%d.", nRet);
		return nRet;
	}

	//2.设置网络缓存
	m_pobjListenTransfer->setRecvBufSize(1024*1024);
	m_pobjListenTransfer->setSendBufSize(1024*1024);

	//3.绑定地址
	nRet = m_pobjListenTransfer->bind(m_objInetAddr);
	if (VIOLET_SUCCESS != nRet)
	{
		ERROR("UDPServer", "Transfer->bind() failed:%d.", nRet);
		m_pobjListenTransfer->close();
		return nRet;
	}

	//4.启动服务线程
	m_nServerThreadId = createThread(UDPServerThread, this, &m_bThreadRunFlag);
	if(CREATE_THREAD_FAIL == m_nServerThreadId )
	{
		ERROR("UDPServer", "PUB_CreateThread failed:%d.", errno);
		m_pobjListenTransfer->close();
		return VIOLET_ERROR_THREAD_CREATED_FAILED;
	}

	INFO("UDPServer", "start[port:%d] success.", m_objInetAddr.nPort);
	return VIOLET_SUCCESS;
}

void UDPServer::stop()
{
	if (m_nServerThreadId != 0)
		exitThread(&m_nServerThreadId, &m_bThreadRunFlag);

	m_bThreadRunFlag = false;
	m_pobjListenTransfer->close();
}

void* UDPServer::UDPServerThread(void* p_pParam)
{
	INFO("UDPServer", "ThreadId=%u", GET_CURRENT_THREADID);
	UDPServer *pUDPServer = static_cast<UDPServer*>(p_pParam);
	pUDPServer->run();
	return NULL;
}

int UDPServer::run()
{
	char szBuf[RECV_BUFFER_SIZE];
	InetAddr objFromInetAddr;

	while (m_bThreadRunFlag)
	{
		int nRet = m_pobjListenTransfer->recvFrom(szBuf, RECV_BUFFER_SIZE, objFromInetAddr, 5000);
		if (VIOLET_SUCCESS >= nRet)
		{
			OS_Sleep(5);
			continue;
		}

		#if 0
		char szAddr[32] = {0};
		snprintf(szAddr, sizeof(szAddr), "%d.%d.%d.%d:%d", 
			             objFromInetAddr.lIp & 0xff,
			            (objFromInetAddr.lIp >> 8)&0xff, 
			            (objFromInetAddr.lIp >> 16)&0xff,
			            (objFromInetAddr.lIp >> 24)&0xff,
			             objFromInetAddr.nPort);
		VDEBUG("UDPServer", "Data-From:%s", szAddr);
		#endif
		handleEvent(szBuf, nRet, objFromInetAddr);
	}
	return VIOLET_SUCCESS;
}

int UDPServer::handleEvent(const char* p_pData, const int p_nDataLen, const InetAddr& p_objInetAddr)
{
	return VIOLET_SUCCESS;
}

