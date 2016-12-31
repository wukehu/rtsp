#include "RtspServer.h"
#include "VioletError.h"
#include "VioletLog.h"
#include "RtspSession.h"

#define RTSP_DEFAULT_PORT 554


RtspServer::RtspServer()
{
}

RtspServer::~RtspServer()
{

}

int RtspServer::start()
{
	InetAddr addr;
	addr.nPort = 554;
	setBindAddr(addr);
	TCPServer::setMaxConnectionCount(128);
	return TCPServer::start();
}

int RtspServer::stop()
{
	TCPServer::stop();
	return VIOLET_SUCCESS;
}

TCPLinkReceiver* RtspServer::creatTCPLinkReceiver() 
{
	RtspSession* psession = new RtspSession();
	return psession;
}


