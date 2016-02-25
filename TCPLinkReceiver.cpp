#include "TCPLinkReceiver.h"
#include "VioletError.h"
#include "TCPTransfer.h"
#include "VioletLog.h"

TCPLinkReceiver::TCPLinkReceiver()
:m_pobjTCPTransfer(NULL)
,m_pPrivateData(NULL)
{
}

TCPLinkReceiver::~TCPLinkReceiver()
{
	if (m_pobjTCPTransfer != NULL)
	{
		delete m_pobjTCPTransfer;
		m_pobjTCPTransfer = NULL;
	}
}

void TCPLinkReceiver::setTCPTransfer(TCPTransfer* p_pobjTCPTransfer)
{
	m_pobjTCPTransfer = p_pobjTCPTransfer;
}

TCPTransfer* TCPLinkReceiver::getTCPTransfer()
{
	return m_pobjTCPTransfer;
}

void TCPLinkReceiver::setPrivateData(void* p_pPrivateData)
{
	m_pPrivateData = p_pPrivateData;
}

void* TCPLinkReceiver::getPrivateData()const
{
	return m_pPrivateData;
}

void TCPLinkReceiver::setLastActiveTime(const unsigned long p_dwLastActiveTime)
{
	m_dwLastActiveTime = p_dwLastActiveTime;
}

unsigned long TCPLinkReceiver::getLastActiveTime()const
{
	return m_dwLastActiveTime;
}

int TCPLinkReceiver::handleEvent(const int p_nNetworkEventType)
{
	return VIOLET_SUCCESS;
}

