#ifndef __CHARSET_CODEC_H__
#define __CHARSET_CODEC_H__

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define U2W_LOBYTE(w)           ((unsigned char)(((unsigned short)(w)) & 0xff))
#define U2W_HIBYTE(w)           ((unsigned char)((((unsigned short)(w)) >> 8) & 0xff))

class CharSetCodec
{
private:
	CharSetCodec();
	~CharSetCodec();
public:
	static int convertGBKToUnicode(const char *p_pGBK, const int p_nGBKLen, char *p_pUnicode, const int p_nOutSize);

	static int convertUtf8ToUnicode(const char *p_pUtf8, const int p_nUtf8Len, unsigned short *pUniCode, const int p_nOutSize);

	static int convertUnicodeToUtf8(const char *p_pUnicode, const int p_nUnicodeLen, char *p_pUtf8, const int p_nOutSize);

	static int convertUnicodeToGBK(const char* p_pUnicode, const int p_nUnicodeLen, char *p_pGBK, const int p_nOutSize);

	static int convertGBKToUtf8(const char* p_pszGBK, const int p_nGBKLen, char* p_pszUtf8, int p_nUtf8Size);

};

#endif /*__CHARSET_CODEC_H__*/

