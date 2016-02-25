#ifndef __UDP_TRANSFER_H__
#define __UDP_TRANSFER_H__

#include "ITransfer.h"

class UDPTransfer :public ITransfer
{
public:
	UDPTransfer();
	~UDPTransfer();
public:
	int open();
	void close();
	int bind(const InetAddr& p_objInetAddr);
	int listen(const int p_nBakLog);
	int connect(const InetAddr& p_objInetAddr, const int p_nTimeOut);
	int accept(ITransfer*& p_pobjITransfer);
	int send(const char* p_pData, const int p_nDataLen, const int p_nTimeOut, const InetAddr& p_objToAddr);
	int send(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToAddr);
	int recv(char* p_pBuffer, const int p_nBuffSize, const int p_nTimeOut);
	int sendTo(const char* p_pData, const int p_nDataLen, const InetAddr& p_objToInetAddr, const int p_nTimeOut = 0);
	int recvFrom(char* p_pBuffer, const int p_nBuffSize, InetAddr& p_objFromInetAddr, const int p_nTimeOut = 0);
};

#endif /*__UDP_TRANSFER_H__*/
