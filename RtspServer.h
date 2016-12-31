#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__

#include "TCPServer.h"

class RtspServer : public TCPServer
{
public:
  RtspServer();
  ~RtspServer();
  
public:
  int start();
  int stop();

  TCPLinkReceiver* creatTCPLinkReceiver() ;
private:
};

#endif /*__RTSP_SERVER_H__*/
