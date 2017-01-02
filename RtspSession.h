#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include "TCPLinkReceiver.h"
#include "RtspRecvStateMachine.h"


class RtspSession : public TCPLinkReceiver
{
public:
    RtspSession();
    ~RtspSession();
public:
    int handleEvent(const int p_nNetworkEventType);

private:
    int handleRTSPCommand();

    int handleOPTION();

    int handleDESCRIBE();

    int handleSETUP();

    int handlePLAY();

    int handleGET_PARAMER();

    int handlePAUSE();

    int handleSET_PARMAER();

    int handleTEADDOWN();

private:
    RtspRecvStateMachine    m_objRecvStateMachine;
    Buffer                  m_objSendBuf;
    
};

#endif /*__RTSP_SESSION_H__*/

