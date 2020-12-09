#include "client.h"

int main()
{
	client *Client = new client;
	Client->init();
	Client->loop();
}
