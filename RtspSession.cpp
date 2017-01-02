#include "RtspSession.h"
#include "VioletError.h"
#include "VioletLog.h"
#include "TCPTransfer.h"

RtspSession::RtspSession()
{
	m_objRecvStateMachine.init();
}

RtspSession::~RtspSession()
{

}

int RtspSession::handleEvent(const int p_nNetworkEventType)
{
	char szBuf[4096] = {0};
	int nRet = m_pobjTCPTransfer->recv(szBuf, sizeof(szBuf), 50);
	if (nRet < 0)
		return nRet;

	m_objRecvStateMachine.parseData(szBuf, nRet);
	return VIOLET_SUCCESS;
}

int RtspSession::handleRTSPCommand()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleOPTION()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleDESCRIBE()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleSETUP()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handlePLAY()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleGET_PARAMER()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handlePAUSE()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleSET_PARMAER()
{
	return VIOLET_SUCCESS;
}

int RtspSession::handleTEADDOWN()
{
	return VIOLET_SUCCESS;
}



