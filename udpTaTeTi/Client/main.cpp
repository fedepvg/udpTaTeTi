#include "Client.h"

void main()
{
	Client* client = new Client();
	client->RunClient();
	delete client;
}