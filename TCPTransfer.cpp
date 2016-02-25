#include "TCPTransfer.h"
#include "VioletError.h"
#include "VioletLog.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

TCPTransfer::TCPTransfer()
{
}

TCPTransfer::~TCPTransfer()
{
}

int TCPTransfer::open()
{
	//1.创建SOCKET
	m_nSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == m_nSock) 
		return VIOLET_ERROR_SOCKET_CREATE_FAILED;

	//2.设置重用属性
	int  opt = 1;
	if(0 != setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int)))	
	{
		close();
		return VIOLET_ERROR_SOCKET_OPTION_FAILED;
	}
	
	//3.设置非阻塞属性
#ifdef WIN32
	unsigned long dwVal = 1; 
	ioctlsocket(m_nSock, FIONBIO, &dwVal);
#else
	int nSave = fcntl(m_nSock, F_GETFL);
	fcntl(m_nSock, F_SETFL, nSave | O_NONBLOCK);
#endif
	return VIOLET_SUCCESS;
}

int TCPTransfer::bind(const InetAddr& p_objInetAddr)
{
	struct sockaddr_in struLocalInfo;
	socklen_t  InfoLen = sizeof(struLocalInfo);
	
	struLocalInfo.sin_family = AF_INET;
	struLocalInfo.sin_addr.s_addr = INADDR_ANY; 			
	struLocalInfo.sin_port = htons(p_objInetAddr.nPort);			
	memset(&(struLocalInfo.sin_zero), 0, 8); 	
	if(0 != ::bind(m_nSock, (sockaddr*)&struLocalInfo, InfoLen))
	{
		ERROR("TCPTransfer", "bind error.");
		return VIOLET_ERROR_SOCKET_BIND_FAILED;
	}
	return VIOLET_SUCCESS;
}

int TCPTransfer::listen(const int p_nBakLog)
{
	if (::listen(m_nSock, p_nBakLog) < 0)
		return VIOLET_ERROR_SOCKET_LISTEN_FAILED;

	return VIOLET_SUCCESS;
}

int TCPTransfer::accept(ITransfer*& p_pobjTCPTransfer)
{
	struct sockaddr struAddr;
	memset(&struAddr, 0, sizeof(struct sockaddr));
	int len = sizeof(struAddr);
	SOCKET sock = ::accept(m_nSock, &struAddr, (socklen_t *)&len);
	if(-1 == sock) 
	{
		if((errno == EINTR || errno == EAGAIN) || (errno == EWOULDBLOCK)) 
			return VIOLET_ERROR_SOCKET_ACCEPT_TIMEOUT;

		return VIOLET_ERROR_SOCKET_ACCEPT_FAILED;
	}

	struct sockaddr_in* inAddr = (struct sockaddr_in*)&struAddr;
	InetAddr objPeerAddr;
	objPeerAddr.lIp = inAddr->sin_addr.s_addr;
	objPeerAddr.nPort = ntohs(inAddr->sin_port);

	p_pobjTCPTransfer = new TCPTransfer();
	p_pobjTCPTransfer->setSocket(sock);
	p_pobjTCPTransfer->setSendBufSize(256*1024);
	p_pobjTCPTransfer->setRecvBufSize(128*1024);
	p_pobjTCPTransfer->setPeerAddr(objPeerAddr);
	return VIOLET_SUCCESS;
}

int TCPTransfer::connect(const InetAddr& p_objInetAddr, const int p_nTimeOut)
{
	struct sockaddr_in struServerAddr;
	struServerAddr.sin_family = AF_INET;
	struServerAddr.sin_addr.s_addr = p_objInetAddr.lIp;
	struServerAddr.sin_port = htons(p_objInetAddr.nPort); 
	memset(&(struServerAddr.sin_zero), 0, 8); 
	socklen_t addrLen = sizeof(struServerAddr);
	int nRet = ::connect(m_nSock, (struct sockaddr *)&struServerAddr, addrLen);
	if (nRet != 0)
	{
		if (EISCONN == errno) 
			return 0;
		else if((EINPROGRESS != errno) && (EWOULDBLOCK != errno) && (EAGAIN != errno) && (EALREADY != errno))
			return VIOLET_ERROR_SOCKET_CONNECT_FAILED;
	}

	fd_set  writeFds;
	FD_ZERO(&writeFds);
	FD_SET(m_nSock, &writeFds);
	struct timeval	struTimeout;
	struTimeout.tv_sec = p_nTimeOut / 1000;  
	struTimeout.tv_usec = (p_nTimeOut % 1000 ) * 1000; 
	nRet = ::select(m_nSock + 1, NULL, &writeFds, NULL, &struTimeout);
	if (nRet <= 0)
		return VIOLET_ERROR_SOCKET_CONNECT_FAILED;

	int nSockError = 0;
	socklen_t errlen = sizeof(int);
	getsockopt(m_nSock, SOL_SOCKET, SO_ERROR, (void*)&nSockError, &errlen);	
	if (nSockError > 0)
		return VIOLET_ERROR_SOCKET_CONNECT_FAILED;
		
	return VIOLET_SUCCESS;
}

void TCPTransfer::close()
{
	if (m_nSock == -1);
		return ;
	
	::CLOSESOCKET(m_nSock);
	m_nSock = -1;
}

int TCPTransfer::nonBlockSend(const char* p_pData, const int p_nDataLen)
{
	int nSendLen = 0;
	int nRet = ::send(m_nSock, p_pData + nSendLen, p_nDataLen - nSendLen, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (nRet <= 0)
	{
		if (errno == EINTR)
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			nSendLen += nRet;
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;
		}
	
		ERROR("TCPTransfer", "Send failed:%d, error:%d", nRet, GET_LAST_ERROR);
		return VIOLET_ERROR_SOCKET_SEND_FAILED;
	}
	nSendLen += nRet;
	if (nSendLen < p_nDataLen)
		WARNING("TCPTransfer", "DstLen:%d SendLen:%d", p_nDataLen, nSendLen);
	
	return VIOLET_SUCCESS;
}

int TCPTransfer::send(const char* p_pData, const int p_nDataLen, const int p_nTimeOut, const InetAddr& p_objToAddr)
{
	int nRet = -1;
	if (p_nTimeOut > 0)
	{
		struct timeval timeout_tv;
		fd_set sockfds;
		timeout_tv.tv_sec = p_nTimeOut / 1000;
		timeout_tv.tv_usec = p_nTimeOut % 1000 * 1000;
		FD_ZERO(&sockfds);
		FD_SET(m_nSock, &sockfds);

		int nRet = select(m_nSock + 1, NULL, &sockfds, NULL, &timeout_tv);	
		if (nRet < 0)
		{
			ERROR("TCPTransfer", "select error");
			return VIOLET_ERROR_SOCKET_SEND_FAILED; 
		}
			
		if(!FD_ISSET(m_nSock, &sockfds))
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;
	}
	
	int nSendLen = 0;
again:
	nRet = ::send(m_nSock, p_pData + nSendLen, p_nDataLen - nSendLen, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (nRet < 0)
	{
		if (errno == EINTR)
			goto again;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			nSendLen += nRet;
			goto again;
		}
	
		ERROR("TCPTransfer", "send failed:%d, error:%d", nRet, GET_LAST_ERROR);
		return VIOLET_ERROR_SOCKET_SEND_FAILED;
	}
	else if (nRet == 0)
		return nSendLen;
	
	nSendLen += nRet;
	if (nSendLen < p_nDataLen)
	{
		WARNING("TCPTransfer", "DstLen:%d SendLen:%d", p_nDataLen, nSendLen);
		goto again;
	}
	return nSendLen;
}

int TCPTransfer::send(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToAddr)
{
	int nRet = ::send(m_nSock, p_pData, p_nDataLen, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (nRet < 0)
	{
		if (errno == EINTR)
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return VIOLET_ERROR_SOCKET_SEND_TIMEOUT;
		}
	
		ERROR("TCPTransfer", "Send failed:%d, error:%d", nRet, GET_LAST_ERROR);
		return VIOLET_ERROR_SOCKET_SEND_FAILED;
	}
	return nRet;
}

int TCPTransfer::recv(char* p_pBuffer, const int p_nBuffSize, const int p_nTimeOut)
{
	struct timeval timeout_tv;
	fd_set sockfds;
	timeout_tv.tv_sec = p_nTimeOut/1000;
	timeout_tv.tv_usec = p_nTimeOut%1000*1000;
	FD_ZERO(&sockfds);
	FD_SET(m_nSock, &sockfds);

	int nRet = select(m_nSock + 1, &sockfds, NULL, NULL, &timeout_tv); 
	if (nRet < 0)
	{
		ERROR("TCPTransfer", "select error:%s.", strerror(errno));
		return VIOLET_ERROR_SOCKET_RECV_FAILED; 
	}
	
	if(!FD_ISSET(m_nSock, &sockfds))
		return VIOLET_ERROR_SOCKET_RECV_TIMEOUT;
again:
	nRet = ::recv(m_nSock, p_pBuffer, p_nBuffSize, MSG_DONTWAIT);
	if (nRet <= 0)
	{
		if (errno == EINTR)
			goto again;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
			goto again;
		
		ERROR("TCPTransfer", "recv error:%s.", strerror(errno));
		return VIOLET_ERROR_SOCKET_RECV_FAILED;
	}
	
	return nRet;
}

int TCPTransfer::recv(char* p_pBuffer, const int p_nBuffSize)
{
	int nRecvLen = 0;
	while (nRecvLen <= p_nBuffSize)
	{
		int nRet = ::recv(m_nSock, p_pBuffer + nRecvLen, p_nBuffSize - nRecvLen, MSG_DONTWAIT);
		if (nRet == -1)
		{
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			
			ERROR("TCPTransfer", "recv error:%s.", strerror(errno));
			return VIOLET_ERROR_SOCKET_RECV_FAILED;
		}
		else if (nRet == 0)
		{
			ERROR("TCPTransfer", "recv 0 bytes, peer has performed an ordly shutdown.");
			return VIOLET_ERROR_SOCKET_RECV_FAILED;
		}
		else 
		{
			nRecvLen += nRet;
		}
	}
	return nRecvLen;
}

int TCPTransfer::sendTo(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToInetAddr, const int p_nTimeOut)
{
	return VIOLET_ERROR_BAD_ENVIROUMENT;
}

int TCPTransfer::recvFrom(char* p_pBuffer, const int p_nBuffSize, InetAddr& p_objIFromnetAddr, const int p_nTimeOut)
{
	return VIOLET_ERROR_BAD_ENVIROUMENT;
}

