#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include "OS_Define.h"
#include "InetAddr.h"
#include "Thread.h"

class UDPTransfer;
class UDPServer
{
public:
	UDPServer();
	virtual ~UDPServer();
public:
	//设置监听端口;
	void setBindAddr(const InetAddr& p_objInetAddr);
	const InetAddr& getBindAddr()const;

	int start();
	void stop();
	
#ifdef WIN32
	static PUB_THREAD_RESULT _stdcall UDPServerThread(void *pParam);
#else
	static PUB_THREAD_RESULT  UDPServerThread(void *pParam);
#endif

protected:
	virtual int handleEvent(const char* p_pData, const int p_nDataLen, const InetAddr& p_objInetAddr);

private:
	int run();
protected:
	InetAddr                    m_objInetAddr;         //监听地址
	UDPTransfer*                m_pobjListenTransfer;  //监听传输器
	thread_t                    m_nServerThreadId;     //服务线程
	bool                        m_bThreadRunFlag;      //监听服务线程运行标记
};

#endif /*__UDP_SERVER_H__*/

