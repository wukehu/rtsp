#ifndef __RTSP_RECV_STATE_MACHINE_H__
#define __RTSP_RECV_STATE_MACHINE_H__

#include <queue>
#include <list>
#include <Buffer.h>
#include "RtspMessage.h"

typedef int (*RTP_CALL_BACK)();
typedef int (*RTSP_MESSAGE_CALL_BACK)();

class RtspRecvStateMachine
{
public:
    enum STATE
    {
        START_LINE = 0,
        HEAD_LINE = 1,
        BODY = 2,
    };

    enum DATA_TYPE
    {
        RTSP_MESSAGE = 0,
        RTP_RTCP_DATA = 1,
    };
public:
    RtspRecvStateMachine();
    ~RtspRecvStateMachine();
public:
    int init();
    
    int parseData(const char* p_pData, const int p_nDataLen);
    
    int getMessage(std::list<RtspMessage>& p_objMessagList);

    int reset();
private:
    int getLine(const char* p_pData, const int p_nDataLen, std::string& p_strLine);
    
    int parseStartLine(const char* p, const int p_nLen);
    
    int parseRTPData(const char* p_pData, const int p_nDataLen);
private:
    std::queue<RtspMessage> m_objMessageQueue;    
    Buffer                  m_objRecvBuff;
    bool                    m_bRTPData;
    int                     m_nState;
    int                     m_nDataType;
};

#endif //__RTSP_RECV_STATE_MACHINE_H__


