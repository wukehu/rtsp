#ifndef __RTP_PACKET_H__
#define __RTP_PACKET_H__

typedef struct tagRTP_HEADER
{
	unsigned char v:1;
}RTP_HEADER;

typedef struct tagRTP_EXTENSION_HEADER
{
    unsigned short tag;
    unsigned short len;

}RTP_EXTENSION_HEADER;

class RTPPacket
{
public:
    RTPPacket();
    ~RTPPacket();
private:
    int                     m_nChannleId;
    int                     m_nTotalLen;
    RTP_HEADER              m_objRTPHeader;
    RTP_EXTENSION_HEADER    m_objExtensionHeader;
};


#endif //__RTP_PACKET_H__

