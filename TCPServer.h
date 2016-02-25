#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <list>
#include "Thread.h"
#include "InetAddr.h"

#define EPOLL_SIZE_MAX  32

class TCPTransfer;
class TCPLinkReceiver;

class TCPServer
{
public:
	TCPServer();
	virtual ~TCPServer();
private:
	TCPServer(const TCPServer& p_objOther);
	TCPServer& operator=(const TCPServer& p_objOther);
public:
	//设置监听端口;
	void setBindAddr(const InetAddr& p_objInetAddr);
	const InetAddr& getBindAddr()const;
	//设置最大连接数
	void setMaxConnectionCount(const int p_nMaxConnectionCount);
	int getMaxConnectionCount()const;
	//启动
	int start();
	//停止
	void stop();
#ifdef WIN32
	static THREAD_RESULT _stdcall TCPServerThread(void *pParam);
#else
	static THREAD_RESULT  TCPServerThread(void *pParam);
#endif

protected:
	virtual int handleEvent(TCPLinkReceiver* p_pobjTCPLinkReceiver, struct epoll_event *ev);
	virtual TCPLinkReceiver* creatTCPLinkReceiver() = 0;
	int addPassiveTCPTransfer(TCPTransfer* p_pobjTCPTransfer);
private:		
	int run();

	int clearTimeOutConnection();

	int epoll();

	int handleAccept();

	int closeConnection(TCPLinkReceiver* p_pobjTCPLinkReceiver);
private:
	InetAddr                    m_objInetAddr;              //服务端口
	int                         m_nMaxConnectionCount;      //最大连接数
	TCPTransfer*                m_objListenTCPTransfer;     //监听传输器
	thread_t                    m_nServerThreadId;          //服务线程
	bool                        m_bThreadRunFlag;           //监听服务线程运行标记
	int                         m_nEpollFd;                 //epoll文件描述符 
	struct 	epoll_event         m_events[EPOLL_SIZE_MAX];   //最大事件数;	
	int                         m_fdCount;                  //VAP连接数
	std::list<TCPLinkReceiver*> m_objTCPLinkReceiverList;   //客户端列表
	lock_t                      m_objLock;                  //会话列表互斥量
};

#endif /*__TCP_SERVER_H__*/

