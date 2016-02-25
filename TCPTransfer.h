#ifndef __TCP_TRANSFER_H__
#define __TCP_TRANSFER_H__

#include "ITransfer.h"

class TCPTransfer;

class TCPTransfer : public ITransfer
{
public:
	TCPTransfer();
	~TCPTransfer();
public:
	//创建SOCKET
	int open();
	//绑定到指定地址
	int bind(const InetAddr& p_objInetAddr);
	//监听
	int listen(const int p_nBakLog);
	//接收
	int accept(ITransfer*& p_pobjTransfer);
	//连接
	int connect(const InetAddr& p_objInetAddr, const int p_nTimeOut);
	//关闭
	void close();
	//超时时间发送,超时时间仅为等待可写时机
	int send(const char* p_pData, const int p_nDataLen, const int p_nTimeOut, const InetAddr& p_objToAddr);
	//非阻塞发送
	int send(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToAddr);
	//非阻塞发送
	int nonBlockSend(const char* p_pData, const int p_nDataLen);
	//超时时间接收
	int recv(char* p_pBuffer, const int p_nBuffSize, const int p_nTimeOut);
	//阻塞式接收，直到收到数据
	int recv(char* p_pBuffer, const int p_nBuffSize);
	//虚实现
	int sendTo(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToInetAddr, const int p_nTimeOut = 0);
	//虚实现
	int recvFrom(char* p_pBuffer, const int p_nBuffSize, InetAddr& p_objIFromnetAddr, const int p_nTimeOut = 0);	
};

#endif /*__TCP_TRANSFER_H__*/

