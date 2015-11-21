#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__

class RtspServer
{
public:
  RtspServer();
  ~RtspServer();
  
public:
  int start();
  int stop();
  
private:
};

#endif /*__RTSP_SERVER_H__*/
