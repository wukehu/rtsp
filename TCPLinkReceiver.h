#ifndef __TCP_LINK_RECEIVER_H__
#define __TCP_LINK_RECEIVER_H__

class TCPTransfer;
class TCPLinkReceiver
{
public:
	TCPLinkReceiver();
	virtual ~TCPLinkReceiver();
public:
	//设置传输器
	void setTCPTransfer(TCPTransfer* p_pobjTCPTransfer);
	TCPTransfer* getTCPTransfer();
	//设置私有数据
	void setPrivateData(void* p_pPrivateData);
	void* getPrivateData()const;

	//设置最后活跃时间
	void setLastActiveTime(const unsigned long p_dwLastActiveTime);
	unsigned long getLastActiveTime()const;
	
	//处理网络事件
	virtual int handleEvent(const int p_nNetworkEventType);
protected:
	TCPTransfer*           m_pobjTCPTransfer;
	void*                  m_pPrivateData;
	unsigned long          m_dwLastActiveTime;
};

#endif /*__TCP_LINK_RECEIVER_H__*/

