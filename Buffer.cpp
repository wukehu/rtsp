#include "Buffer.h"
#include "VioletError.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "VioletLog.h"

Buffer::Buffer()
{
	m_pBuffer = NULL;
	m_nBufSize = 0;
	m_nPos = 0;
	m_nDataLen = 0;
}

Buffer::Buffer(const char* p_pData, const int p_nDataLen)
{
	m_pBuffer = new char[p_nDataLen];
	assert(m_pBuffer);
	memcpy(m_pBuffer, p_pData, p_nDataLen);
	m_nDataLen = p_nDataLen;
	m_nPos = 0;
	m_nBufSize = p_nDataLen;
}

Buffer::~Buffer()
{
	if (m_pBuffer != NULL)
	{
		delete []m_pBuffer;
		m_pBuffer = NULL;
		m_nDataLen = 0; 
		m_nPos = 0;
		m_nBufSize = 0;
	}
}

int Buffer::alloc(const int p_nSize)
{
	if (m_pBuffer != NULL)
	{	
		delete [] m_pBuffer;
		m_nBufSize = 0;
		m_pBuffer = NULL;
	}
	
	m_pBuffer = new char[p_nSize];
	assert(m_pBuffer);
	memset(m_pBuffer, 0, p_nSize);
	m_nBufSize = p_nSize;
	m_nDataLen = 0; 
	m_nPos = 0;
	return 0;
}

int Buffer::write(const char* p_pData, const int p_nDataLen)
{
	if (p_pData == NULL || p_nDataLen <= 0)
		return VIOLET_ERROR_BAD_LENGTH;

	if (p_nDataLen <= 0 || p_nDataLen > m_nBufSize - m_nDataLen)
		return VIOLET_ERROR_BAD_LENGTH;

	int nCopyLen = p_nDataLen < (m_nBufSize - m_nDataLen) ? p_nDataLen : (m_nBufSize - m_nDataLen);
	memcpy(m_pBuffer + m_nDataLen, p_pData, nCopyLen);
	m_nDataLen += nCopyLen;
	return nCopyLen;
}

char* Buffer::read(const int p_nWhere, int& p_nDataLen)
{	
	if (p_nWhere == BUF_CUR)
	{
		if (m_nDataLen == 0 || m_nPos >= m_nDataLen)
			return NULL;
	}
	else if (p_nWhere == BUF_BEGIN)
	{
		if (m_nDataLen == 0)
			return NULL;
	}
	p_nDataLen = m_nDataLen - m_nPos;
	return m_pBuffer + m_nPos;
}

int Buffer::seek(const int p_nPos)
{
	m_nPos = p_nPos;
	return VIOLET_SUCCESS;
}

int Buffer::tell()
{
	return m_nPos;
}

int Buffer::getDataLen()
{
	return m_nDataLen;
}

int Buffer::reset()
{
	m_nDataLen = 0; 
	m_nPos = 0;
	if (m_pBuffer != NULL)
	{
		memset(m_pBuffer, 0, m_nBufSize);
	}
	return 0;
}

int Buffer::eraseData(const char* p_pData, const int p_nLen)
{
	if (p_pData == NULL || p_nLen <= 0)
		return VIOLET_ERROR_BAD_LENGTH;

	int nTailLen = m_pBuffer + m_nDataLen - (p_pData + p_nLen);
	memmove((void*)p_pData, p_pData+p_nLen, nTailLen);
	m_nDataLen -= p_nLen;
	return VIOLET_SUCCESS;
}



