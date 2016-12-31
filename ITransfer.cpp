#include "ITransfer.h"
#include "VioletError.h"
#include "VioletLog.h"
#include <sys/types.h>			
#include <sys/socket.h>
#include <time.h>


ITransfer::ITransfer()
:m_nSock(-1)
,m_nRefCount(0)
{
}

ITransfer::~ITransfer()
{
}

void ITransfer::setSocket(SOCKET p_nSocket) 
{
	m_nSock = p_nSocket;
}

SOCKET ITransfer::getSocket()const
{
	return m_nSock;
} 

void ITransfer::setPeerAddr(const InetAddr& p_objInetAddr)
{
	m_objPeerInetAddr = p_objInetAddr;
}

const InetAddr& ITransfer::getPeerAddr()const
{
	return m_objPeerInetAddr;
}

int ITransfer::setRecvBufSize(const unsigned int p_nRecvBufSize)
{
	int opt = p_nRecvBufSize < 128*1024 ? 128*1024 : p_nRecvBufSize;
	int nRet = setsockopt(m_nSock, SOL_SOCKET, SO_RCVBUF, (char*)&opt, sizeof(int));
	if(nRet != 0) 
	{
		ERROR("ITransfer", "setsockopt SO_RCVBUF failure:%d.", GET_LAST_ERROR);
		return VIOLET_ERROR_SOCKET_OPTION_FAILED;
	}
	return VIOLET_SUCCESS;
}

int ITransfer::setSendBufSize(const unsigned int p_nSendBufSize)
{
	int opt = p_nSendBufSize < 128*1024 ? 128*1024 : p_nSendBufSize;
	int nRet = setsockopt(m_nSock, SOL_SOCKET, SO_SNDBUF, (char*)&opt, sizeof(int));
	if(nRet != 0)
	{
		ERROR("ITransfer", "setsockopt SO_SNDBUF failure:%d.", GET_LAST_ERROR);
		return VIOLET_ERROR_SOCKET_OPTION_FAILED;
	}
	return VIOLET_SUCCESS;
}


