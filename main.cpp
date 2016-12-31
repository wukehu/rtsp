#include "RtspServer.h"
#include "OS_define.h"

int main(int argc, char** argv)
{
	RtspServer* s = new RtspServer();
	s->start();
	getchar();
	s->stop();
	return 0;
}


