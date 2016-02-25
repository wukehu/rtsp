#ifndef __I_TRANSFER_H__
#define __I_TRANSFER_H__

#include "OS_Define.h"
#include "InetAddr.h"

class ITransfer;
class ITransfer
{
public:
	enum TRANSPORT_PROTOCOL
	{
		UDP = 0,
		TCP = 1,
		RAW = 2,
	};
	enum AF_FAMILY
	{
		UNIX = 0,
		INET = 1,
		INET6 = 2,
	};
	enum EVENT_TYPE
	{
		READ = 0,
		WRITE = 1,
		ERROR = 2,
	};
public:
	ITransfer();
protected:	
	virtual ~ITransfer();
public:	
	virtual void setSocket(SOCKET p_nSocket);
	virtual SOCKET getSocket()const;
	virtual void setPeerAddr(const InetAddr& p_objInetAddr);
	virtual const InetAddr& getPeerAddr()const;
	virtual int setRecvBufSize(const unsigned int p_nRecvBufSize);
	virtual int setSendBufSize(const unsigned int p_nSendBufSize);
	virtual int open() = 0;
	virtual void close() = 0;
	virtual int bind(const InetAddr& p_objInetAddr) = 0;
	virtual int listen(const int p_nBakLog) = 0;
	virtual int connect(const InetAddr& p_objInetAddr, const int p_nTimeOut) = 0;
	virtual int accept(ITransfer*& p_pobjITransfer) = 0;
	virtual int send(const char* p_pData, const int p_nDataLen, const int p_nTimeOut, const InetAddr& p_objToAddr)= 0;
	virtual int send(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToAddr)= 0;
	virtual int recv(char* p_pBuffer, const int p_nBuffSize, const int p_nTimeOut) = 0;
	virtual int sendTo(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToInetAddr, const int p_nTimeOut = 0) = 0;
	virtual int recvFrom(char* p_pBuffer, const int p_nBuffSize, InetAddr& p_objIFromnetAddr, const int p_nTimeOut = 0) = 0;
protected:	
	enum TRANSPORT_PROTOCOL m_nTransportProtocol;//传输协议
	SOCKET   				m_nSock;             //SOCKET句柄
	int						m_nRefCount;         //引用计数
	InetAddr                m_objPeerInetAddr;   //对端地址
};

#endif /*__I_TRANSFER_H__*/

