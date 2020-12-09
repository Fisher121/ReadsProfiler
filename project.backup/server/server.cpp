#include "serverClass.h"

int main()
{
	Server* serv = new Server;
	serv->init();
	serv->loop();
}

