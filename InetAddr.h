#ifndef __INET_ADDR_H__
#define __INET_ADDR_H__

typedef struct tag_InetAddr
{
	unsigned long lIp;
	unsigned short nPort;
}InetAddr;

#ifndef WIN32
#include <netinet/in.h>
#endif

#endif /*__INET_ADDR_H__*/

