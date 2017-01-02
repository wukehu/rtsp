#include "RtspRecvStateMachine.h"
#include "VioletError.h"
#include "OS_Define.h"
#include "VioletLog.h"
#include "RtpPacket.h"
#include <string>

RtspRecvStateMachine::RtspRecvStateMachine()
{
	m_nDataType = RTSP_MESSAGE;
	m_nState = START_LINE;
}

RtspRecvStateMachine::~RtspRecvStateMachine()
{

}

int RtspRecvStateMachine::init()
{
	m_objRecvBuff.alloc(128*1024);
}

int RtspRecvStateMachine::parseData(const char* p_pData, const int p_nDataLen)
{
	int nRet = m_objRecvBuff.write(p_pData, p_nDataLen);
	if (nRet < 0)
	{
		m_objRecvBuff.reset();
		return nRet;
	}

	int nReadLen = 0;
	char* p  = NULL;
	char* pData = NULL;
	int nLineLen = 0;
again:
	pData = m_objRecvBuff.read(Buffer::BUF_CUR, nReadLen);
	if (pData == NULL || nReadLen <= 0)
		return VIOLET_ERROR_NOT_READY;
		
	if (*pData == '$')
	{
		INFO("RtspRecvStateMachine", "read rtp-rtcp data:%s.", pData);
		m_nDataType = RTP_RTCP_DATA;
		nRet = parseRTPData(pData, nReadLen);
		if (nRet > 0)
		{
			m_objRecvBuff.eraseData(pData, nRet);
			goto again;
		}
		return nRet;
	}

	int nContentLength = 0;
	int nPos = 0;
	int nRecvBodyLen = 0;
	while (nPos < nReadLen)
	{
		char* pStart = pData + nPos;
		std::string strLine = std::string("");
		switch (m_nState)
		{
			case START_LINE:
				strLine = std::string(""); 
				nRet = getLine(pStart, nReadLen - nPos, strLine);
				if (nRet != VIOLET_SUCCESS)
					return nRet;
				
				nRet = parseStartLine(strLine.c_str(), strLine.length());
				if (nRet != VIOLET_SUCCESS)
				{
					m_objRecvBuff.eraseData(pData, nReadLen);
					return nRet;
				}
				m_nState = HEAD_LINE;
				nPos += strLine.length() + 2;
				break;
				
			case HEAD_LINE:
				strLine = std::string(""); 
				nRet = getLine(pStart, nReadLen - nPos, strLine);
				if (nRet != VIOLET_SUCCESS)
					return nRet;
				
				INFO("RtspRecvStateMachine", "Headline:%s.", strLine.c_str());
				nPos += strLine.length() + 2;
				if (strLine.empty())
				{
					INFO("RtspRecvStateMachine", "HeadLine end!");
					m_nState = BODY;
					if (nContentLength == 0)
					{
						INFO("RtspRecvStateMachine", "rtsp message parse done!");
						return VIOLET_SUCCESS;
					}
				}
				else if (strncasecmp(strLine.c_str() , "content-length:", 15) == 0)
				{	
					nContentLength = atoi(strLine.c_str());
					INFO("RtspRecvStateMachine", "content-length:%d.", nContentLength);
				}
				break;
			case BODY:
				INFO("RtspRecvStateMachine", "content-length:%d.", nContentLength);
				nRecvBodyLen = nReadLen - nPos;
				if (nRecvBodyLen < nContentLength)
					return VIOLET_ERROR_NOT_READY;
				
				strLine = std::string(pStart, nContentLength);
				nPos += nContentLength;
				break;
			default:
				break;
		}
	}

	if (nPos > 0)
	{
		m_objRecvBuff.eraseData(pData, nPos);
	}
			
	return VIOLET_SUCCESS;
}

int RtspRecvStateMachine::getLine(const char* p_pData, const int p_nDataLen, std::string& p_strLine)
{
	char* p = strstr(p_pData, "\r\n");
	if (p == NULL)
		return VIOLET_ERROR_NOT_READY;

	p_strLine = std::string(p_pData, p - p_pData);
	return VIOLET_SUCCESS;
}

int RtspRecvStateMachine::parseStartLine(const char* p_pData, const int p_nLen)
{
	char* p = NULL;
	strtok_r((char*)p_pData, " ", &p);
	if (p == NULL)
		return VIOLET_ERROR_COMMAND_FORMAT;

	int nPos = 0;
	int nLen = p - p_pData;
	std::string strMethod = std::string(p_pData, nLen);
	nPos += nLen;
	strtok_r(NULL, " ", &p);
	if (p == NULL)
		return VIOLET_ERROR_COMMAND_FORMAT;

	nLen = p - p_pData - nPos;
	std::string strURL = std::string(p_pData + nPos, nLen);
	nPos += nLen;
	strtok_r(NULL, "/", &p);
	if (p == NULL)
		return VIOLET_ERROR_COMMAND_FORMAT;

	std::string strProtocol = std::string(p_pData + nPos, p - p_pData - nPos);
	std::string strVersion = std::string(p);
	if (strProtocol.empty() || strVersion.empty())
		return VIOLET_ERROR_COMMAND_FORMAT;
	
	INFO("RtspRecvStateMachine", "Method:%s URL:%s Protocol:%s version:%s.", 
		strMethod.c_str(), strURL.c_str(), strProtocol.c_str(), strVersion.c_str());
	return VIOLET_SUCCESS;
}

int RtspRecvStateMachine::parseRTPData(const char* p_pData, const int p_nDataLen)
{
	unsigned char  ucChannelId = 0;
	unsigned short nLen = 0;
	int nConsumeLen = 0;
	if (p_nDataLen < 4)
		return VIOLET_ERROR_NOT_READY;
	
	ucChannelId = *(p_pData + 1);
	nLen = ntohs(*(p_pData + 2));
	if (p_nDataLen < nLen + 4)
		return VIOLET_ERROR_NOT_READY;

	RTPPacket objRtpPacket;
	
	nConsumeLen = nLen + 4;
	#if 0
	char* pNext = p_pData + nConsumeLen;
	int nLeftLen = p_nDataLen - nConsumeLen;
	if (nLeftLen > 4  && *pNext == '$')
	{
		int nRet = parseRTPData(pNext, p_nDataLen - nConsumeLen);
		if (nRet < 0)
			return nConsumeLen;

		nConsumeLen += nRet;
	}
	#endif
	return nConsumeLen;
}

int RtspRecvStateMachine::getMessage(std::list<RtspMessage>& p_objMessagList)
{
	return VIOLET_SUCCESS;
}

int RtspRecvStateMachine::reset()
{
	m_objRecvBuff.reset();
	return VIOLET_SUCCESS;
}


