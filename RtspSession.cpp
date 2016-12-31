#include "RtspSession.h"
#include "VioletError.h"
#include "VioletLog.h"
#include "TCPTransfer.h"

RtspSession::RtspSession()
{

}

RtspSession::~RtspSession()
{

}

int RtspSession::handleEvent(const int p_nNetworkEventType)
{
	INFO("RtspSession", "handle event:%d.", p_nNetworkEventType);
	char szBuf[4096] = {0};
	int nRet = m_pobjTCPTransfer->recv(szBuf, sizeof(szBuf), 50);
	if (nRet < 0)
		return nRet;
	
	INFO("RtspSession", "recv:%s", szBuf);
	return VIOLET_SUCCESS;
}


