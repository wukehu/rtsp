#ifndef __BUFFER_H__
#define __BUFFER_H__

class Buffer
{
public:
    enum WHERE
    {
        BUF_BEGIN = 0,
        BUF_CUR = 1,
    };

public:
    Buffer();
    Buffer(const char* p_pData, const int p_nDataLen);
    ~Buffer();
public:
    int alloc(const int p_nSize);
    int write(const char* p_pData, const int p_nDataLen);
    char* read(const int p_nWhere, int& p_nDataLen);
    int seek(const int p_nPos);
    int tell();
    int getDataLen();
    int reset();
    int eraseData(const char* p_pData, const int p_nLen);
private:
    char*           m_pBuffer;
    int             m_nBufSize;
    int             m_nPos;
    int             m_nDataLen;
    
};
#endif //__BUFFER_H__



