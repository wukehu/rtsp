#include "TCPServer.h"
#include "VioletError.h"
#include "TCPTransfer.h"
#include "TCPLinkReceiver.h"
#include "VioletLog.h"
#include "VioletTime.h"


//TCP连接超时时间
#define MAX_CONNETION_TIMEOUT_VAL  60

TCPServer::TCPServer()
:m_fdCount(0)
,m_nMaxConnectionCount(10)
,m_objListenTCPTransfer(NULL)
{
	PUB_InitLock(&m_objLock);
}

TCPServer::~TCPServer()
{
	stop();
	PUB_DestroyLock(&m_objLock);
}

void TCPServer::setBindAddr(const InetAddr& p_objInetAddr)
{
	m_objInetAddr = p_objInetAddr;
}

const InetAddr& TCPServer::getBindAddr()const
{
	return m_objInetAddr;
}

void TCPServer::setMaxConnectionCount(const int p_nMaxConnectionCount)
{
	m_nMaxConnectionCount = p_nMaxConnectionCount;
}

int TCPServer::getMaxConnectionCount()const
{
	return m_nMaxConnectionCount;
}

int TCPServer::start()
{
	if (m_bThreadRunFlag)
		return VIOLET_SUCCESS;

	//1.创建监听传输器
	m_objListenTCPTransfer = new TCPTransfer();
	struct epoll_event ev;
	int nRet = m_objListenTCPTransfer->open();
	if (VIOLET_SUCCESS != nRet)
		goto fail;
	
	//2.绑定端口
	m_objListenTCPTransfer->setRecvBufSize(128*1024);
	m_objListenTCPTransfer->setSendBufSize(128*1024);
	nRet = m_objListenTCPTransfer->bind(m_objInetAddr);
	if (VIOLET_SUCCESS != nRet)
		goto fail;
	
	//3.开启监听
	nRet = m_objListenTCPTransfer->listen(m_nMaxConnectionCount);
	if (VIOLET_SUCCESS != nRet)
		goto fail;
	
	//4.创建epoll
	m_nEpollFd = epoll_create(EPOLL_SIZE_MAX);
	if(m_nEpollFd == -1) 
	{
		nRet = VIOLET_ERROR_SOCKET_EPOLL_CREATE_FAILED;
		goto fail;
	}
	
	//5.添加监听事件
	m_fdCount = 1;
	ev.events = EPOLLIN;
	ev.data.fd = m_objListenTCPTransfer->getSocket();
	nRet = epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, ev.data.fd, &ev);
	if(nRet == -1) 
	{
		nRet = VIOLET_ERROR_SOCKET_EPOLL_CTRL_FAILRD;
		goto fail;
	}
	
	//6.启动服务线程
	m_nServerThreadId = createThread(TCPServerThread, this, &m_bThreadRunFlag);
	if( CREATE_THREAD_FAIL == m_nServerThreadId )
	{
		nRet = VIOLET_ERROR_THREAD_CREATED_FAILED;
		goto fail;
	}
	
	return VIOLET_SUCCESS;
fail:
	if (m_objListenTCPTransfer != NULL)
	{
		m_objListenTCPTransfer->close();
		delete m_objListenTCPTransfer;
		m_objListenTCPTransfer = NULL;
	}
	return nRet;
}

void*  TCPServer::TCPServerThread(void *pParam)
{
	INFO("TCPServer", "ThreadId=%u", GET_CURRENT_THREADID);
	TCPServer *pTCPServer = static_cast<TCPServer*>(pParam);
	pTCPServer->run();
	return NULL;
}

void TCPServer::stop()
{
	//1.停止工作线程
	if (m_nServerThreadId != 0)
		exitThread(&m_nServerThreadId, &m_bThreadRunFlag);
	m_bThreadRunFlag = false;

	//2.关闭监听
	m_objListenTCPTransfer->close();
	delete m_objListenTCPTransfer;
	m_objListenTCPTransfer = NULL;

	//3.销毁epoll文件描述符
	::CLOSESOCKET(m_nEpollFd);

	//4.清理连接对象列表
	std::list<TCPLinkReceiver*>::iterator it = m_objTCPLinkReceiverList.begin();
	for (; it != m_objTCPLinkReceiverList.end(); ++it)
	{
		TCPLinkReceiver* pobjTCPLinkReceiver = *it;
		if (NULL != pobjTCPLinkReceiver)
			delete pobjTCPLinkReceiver;
	}
	m_objTCPLinkReceiverList.clear();
}

int TCPServer::run()
{
	int nInterval = 0;
	while (m_bThreadRunFlag)
	{
		//1.epoll接收新连接，处理注册事件业务请求
		if (epoll() != VIOLET_SUCCESS)
			nInterval += 5000;
		else
			nInterval += 10;

		//2.清理超时连接，利用其epoll的超时时间作为参考，间隔约为30s
		if (nInterval > 30000)
		{
			clearTimeOutConnection();
			nInterval= 0;
		}
		PUB_Sleep(2);
	}
	return VIOLET_SUCCESS;
}

int TCPServer::clearTimeOutConnection()
{
	unsigned long dwNow = VIOLETTime::RealSeconds();
	std::list<TCPLinkReceiver*>::iterator it = m_objTCPLinkReceiverList.begin();
	for (; it != m_objTCPLinkReceiverList.end(); )
	{
		TCPLinkReceiver* pobjTCPLinkReceiver = *it;
		if (dwNow - pobjTCPLinkReceiver->getLastActiveTime() > MAX_CONNETION_TIMEOUT_VAL)
		{
			closeConnection(pobjTCPLinkReceiver);
			it = m_objTCPLinkReceiverList.erase(it);
			continue;
		}
		++it;
	}
	return VIOLET_SUCCESS;
}

int TCPServer::epoll()
{
	//1.epoll_wait
	int nfds = epoll_wait(m_nEpollFd, m_events, m_fdCount, 5000);
	if(nfds == -1)
	{
		ERROR("TCPServer", "epoll wait error:%s epool_fd:%d fd_count:%d", strerror(GET_LAST_ERROR), m_nEpollFd, m_fdCount);
		return VIOLET_ERROR_SOCKET_EPOLL_WAIT_FAILED;
	}
	else if(nfds == 0)
	{
		VDEBUG("TCPServer", "epoll wait timeout.");
		return VIOLET_ERROR_SOCKET_EPOLL_TIMEOUT;
	}

	//2.处理注册事件
	for(int i = 0; i < nfds; i++)
	{
		//2.1 监听事件发生意味新的连接到来
		if(m_events[i].data.fd == m_objListenTCPTransfer->getSocket()) 
		{
			INFO("TCPServer", "server handle.");
			int nRet = handleAccept();
			if(nRet != 0) 
			{
				ERROR("TCPServer", "tcp new connection failure.");
				continue;
			}
		}
		else
		{
			//2.2 客户端连接产生事件
			int nRet = handleEvent((TCPLinkReceiver *)m_events[i].data.ptr, &m_events[i]);
			if (VIOLET_ERROR_SOCKET_SEND_FAILED == nRet || VIOLET_ERROR_SOCKET_RECV_FAILED == nRet)
			{
				closeConnection((TCPLinkReceiver *)m_events[i].data.ptr);
			}
		}
	}

	return VIOLET_SUCCESS;
}

int TCPServer::handleAccept()
{
	//1.accept
	ITransfer* pobjTransfer = NULL;
	int nRet = m_objListenTCPTransfer->accept(pobjTransfer);
	if(nRet != VIOLET_SUCCESS)
		return nRet;

	//2.向下转型为子类
	TCPTransfer* pobjTCPTransfer = dynamic_cast<TCPTransfer*>(pobjTransfer);
	if (pobjTCPTransfer == NULL)
	{
		pobjTransfer->close();
		return VIOLET_ERROR_NULL_POINTER;
	}
	//3.设置激活时间
	TCPLinkReceiver* pobjTCPLinkReceiver = creatTCPLinkReceiver();
	pobjTCPLinkReceiver->setTCPTransfer(pobjTCPTransfer);
	pobjTCPLinkReceiver->setLastActiveTime(VIOLETTime::RealSeconds());

	//4.初始化监听事件，当前仅注册可读
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI;
	ev.data.ptr = (void*)pobjTCPLinkReceiver;

	//5.添加监听事件
	nRet = epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, pobjTCPTransfer->getSocket(), &ev);
	if(nRet == -1)
	{
		pobjTCPTransfer->close();
		delete pobjTCPLinkReceiver;
		delete pobjTCPTransfer;
		ERROR("VAPSessionManager", "epoll ctl error:%s epool:%d", strerror(errno), m_nEpollFd);
		return VIOLET_ERROR_SOCKET_EPOLL_CTRL_FAILRD;
	}
	//6.追加到管理列表
	m_objTCPLinkReceiverList.push_back(pobjTCPLinkReceiver);
	m_fdCount++;
	return VIOLET_SUCCESS;
}

int TCPServer::addPassiveTCPTransfer(TCPTransfer* pobjTCPTransfer)
{
	if (pobjTCPTransfer == NULL)
		return VIOLET_ERROR_NULL_POINTER;

	//1.创建TCP链路对象
	TCPLinkReceiver* pobjTCPLinkReceiver = creatTCPLinkReceiver();
	pobjTCPLinkReceiver->setTCPTransfer(pobjTCPTransfer);
	pobjTCPLinkReceiver->setLastActiveTime(VIOLETTime::RealSeconds());

	//2.创建监听事件
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI;
	ev.data.ptr = (void*)pobjTCPLinkReceiver;

	//3.添加事件
	int nRet = epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, pobjTCPTransfer->getSocket(), &ev);
	if(nRet == -1)
	{
		pobjTCPTransfer->close();
		delete pobjTCPLinkReceiver;
		delete pobjTCPTransfer;
		ERROR("TCPServer", "epoll ctl error:%s epool:%d", strerror(errno), m_nEpollFd);
		return VIOLET_ERROR_SOCKET_EPOLL_CTRL_FAILRD;
	}
	//4.加入到管理列表
	m_objTCPLinkReceiverList.push_back(pobjTCPLinkReceiver);
	m_fdCount++;
	INFO("TCPServer", "addPassiveTCPTransfer done");
	return VIOLET_SUCCESS;
}

int TCPServer::handleEvent(TCPLinkReceiver* p_pobjTCPLinkReceiver, struct epoll_event *ev)
{
	if (p_pobjTCPLinkReceiver == NULL)
		return VIOLET_ERROR_NULL_POINTER;
	//1.事件分类处理
	int nNetWorkEventType = 0;
	if (ev->events & EPOLLIN)
	{
		nNetWorkEventType = ITransfer::READ;
		p_pobjTCPLinkReceiver->setLastActiveTime(VIOLETTime::RealSeconds());
	}
	else if (ev->events & EPOLLOUT)
	{
		nNetWorkEventType = ITransfer::WRITE;
	}
	else if (ev->events & EPOLLPRI)
	{
		nNetWorkEventType = ITransfer::ERROR;
		ERROR("TCPServer", "handleEvent EPOLLPRI Event");
	}
	else if (ev->events & EPOLLERR)
	{
		nNetWorkEventType = ITransfer::ERROR;
		ERROR("TCPServer", "handleEvent EPOLLERR Event");
	}
	else if (ev->events & EPOLLHUP)
	{
		nNetWorkEventType = ITransfer::ERROR;
		ERROR("TCPServer", "handleEvent EPOLLHUP Event");
	}
	else 
	{
		ERROR("TCPServer", "handleEvent UnKnown Event");
		return VIOLET_ERROR_BAD_PARAM;
	}	

	//2.具体处理交于实现者处理	
	return p_pobjTCPLinkReceiver->handleEvent(nNetWorkEventType);
}

int TCPServer::closeConnection(TCPLinkReceiver* p_pobjTCPLinkReceiver)
{
	if (p_pobjTCPLinkReceiver == NULL)
		return VIOLET_ERROR_NULL_POINTER;

	TCPTransfer* pobjTCPTransfer = p_pobjTCPLinkReceiver->getTCPTransfer();
	SOCKET sock = pobjTCPTransfer->getSocket();
	//1.从监听事件列表移除
	int nRet = epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, sock, NULL);
	if(nRet == -1) 
	{
		ERROR("TCPServer", "epoll ctl sock:%d error:%s.", sock, strerror(errno));
	}
	else
	{
		m_fdCount--;
	}
	//2.关闭链接
	pobjTCPTransfer->close();
	ERROR("TCPServer", "TCPTransfer->close:%p.", pobjTCPTransfer);
	return VIOLET_SUCCESS;
}


