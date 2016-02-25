#include "UDPTransfer.h"
#include "VioletError.h"
#include "VioletLog.h"
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>


UDPTransfer::UDPTransfer()
{
}

UDPTransfer::~UDPTransfer()
{
}

int UDPTransfer::open()
{
	m_nSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == m_nSock) 
	{
		ERROR("UDPTransfer", "socket failed.");
		return VIOLET_ERROR_SOCKET_CREATE_FAILED;
	}

	int opt = 1;
	if(0 != setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int)))
	{
		::CLOSESOCKET(m_nSock);
		return VIOLET_ERROR_SOCKET_CREATE_FAILED;
	}
	
#ifdef  WIN32
	unsigned long dwVal = 1; 
	ioctlsocket(m_nSock, FIONBIO, &dwVal);
#else
	int iSave = fcntl(m_nSock,F_GETFL);
	fcntl(m_nSock, F_SETFL, iSave | O_NONBLOCK);
#endif
	return VIOLET_SUCCESS;
}

void UDPTransfer::close()
{
	::CLOSESOCKET(m_nSock);
	m_nSock = -1;
}

int UDPTransfer::bind(const InetAddr& p_objInetAddr)
{
	struct sockaddr_in struLocalInfo;
	struLocalInfo.sin_family = AF_INET;
	struLocalInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	struLocalInfo.sin_port = htons(p_objInetAddr.nPort); 	
	memset(&(struLocalInfo.sin_zero), 0, 8); 

	int nRet = ::bind(m_nSock, (sockaddr*)&struLocalInfo, sizeof(struLocalInfo));
	if (nRet != 0)
		return VIOLET_ERROR_SOCKET_BIND_FAILED;
	
	return VIOLET_SUCCESS;
}

int UDPTransfer::listen(const int p_nBakLog)
{
	return VIOLET_SUCCESS;
}

int UDPTransfer::connect(const InetAddr& p_objInetAddr, const int p_nTimeOut)
{
	return VIOLET_SUCCESS;
}

int UDPTransfer::accept(ITransfer*& p_pobjITransfer)
{
	return VIOLET_ERROR_BAD_ENVIROUMENT;
}

int UDPTransfer::send(const char* p_pData, const int p_nDataLen, const int p_nTimeOut, const InetAddr& p_objDstInetAddr)
{
	return sendTo(p_pData, p_nDataLen, p_objDstInetAddr, p_nTimeOut);
}

int UDPTransfer::recv(char* p_pBuffer, const int p_nBuffSize, const int p_nTimeOut)
{
	return VIOLET_ERROR_BAD_ENVIROUMENT;
}

int UDPTransfer::send(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToAddr)
{
	return sendTo(p_pData, p_nDataLen, p_objToAddr, 0);
}

int UDPTransfer::sendTo(const char* p_pData, const int p_nDataLen, const InetAddr& p_objDstInetAddr, const int p_nTimeOut)
{
	if (p_nTimeOut > 0)
	{
		fd_set  writeFds;
		FD_ZERO(&writeFds);
		FD_SET(m_nSock, &writeFds);
		
		struct timeval	struTimeout;		
		struTimeout.tv_sec = p_nTimeOut / 1000;  
		struTimeout.tv_usec = (p_nTimeOut % 1000 ) * 1000; 
		int nRet = select(m_nSock + 1, NULL, &writeFds, NULL, &struTimeout);		
		if (nRet < 0) 
			return VIOLET_ERROR_SOCKET_RECV_FAILED;
		else if(0 == nRet)
			return VIOLET_ERROR_SOCKET_RECV_TIMEOUT;	
	}

	struct sockaddr_in struTo;
	socklen_t  toLen = sizeof(struTo);
	struTo.sin_family = AF_INET;
	struTo.sin_addr.s_addr = p_objDstInetAddr.lIp;
	struTo.sin_port = htons(p_objDstInetAddr.nPort); 
	memset(&(struTo.sin_zero), 0, 8); 

	sockaddr* struAddr = (sockaddr*)&struTo;
	#ifdef WIN32
	int flag = 0;
	#else
	int flag = MSG_DONTWAIT | MSG_NOSIGNAL;
	#endif
	
	int nRet = sendto(m_nSock, p_pData, p_nDataLen, flag, struAddr, toLen);
	if (nRet < 0)
	{
		if ((EWOULDBLOCK == errno) || (EAGAIN == errno) || (EINTR == errno))
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;

		return VIOLET_ERROR_SOCKET_SEND_FAILED;
	}
	return nRet;
}

int UDPTransfer::recvFrom(char* p_pBuffer, const int p_nBuffSize, InetAddr& p_objFromInetAddr, const int p_nTimeOut)
{
	int nRet = -1;
	if (p_nTimeOut > 0)
	{
		fd_set  rdFds;
		FD_ZERO(&rdFds);
		FD_SET(m_nSock, &rdFds);
		
		struct timeval	struTimeout;		
		struTimeout.tv_sec = p_nTimeOut / 1000;  
		struTimeout.tv_usec = (p_nTimeOut % 1000 ) * 1000; 
		nRet = select(m_nSock + 1, &rdFds, NULL, NULL, &struTimeout);
		if (nRet < 0) 
			return VIOLET_ERROR_SOCKET_RECV_FAILED;
		else if(0 == nRet)
			return VIOLET_ERROR_SOCKET_RECV_TIMEOUT;
	}

	struct sockaddr_in struFrom;
	socklen_t  fromLen = sizeof(struFrom);
	nRet = recvfrom(m_nSock, p_pBuffer, p_nBuffSize, 0, (sockaddr*)&struFrom, &fromLen);
	if(-1 == nRet)
	{
		if ((EWOULDBLOCK == errno) || (EAGAIN == errno) || (EINTR == errno))
			return VIOLET_ERROR_SOCKET_RECV_TIMEOUT;
		
		return VIOLET_ERROR_SOCKET_RECV_FAILED;
	}
	
	p_objFromInetAddr.lIp = struFrom.sin_addr.s_addr;
	p_objFromInetAddr.nPort= ntohs(struFrom.sin_port);
	return nRet;
}


