#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include "TCPLinkReceiver.h"

class RtspSession : public TCPLinkReceiver
{
public:
    RtspSession();
    ~RtspSession();
public:
    int handleEvent(const int p_nNetworkEventType);
};

#endif /*__RTSP_SESSION_H__*/

