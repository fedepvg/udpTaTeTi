#include "Server.h"

void main()
{
	Server* server = new Server();
	server->RunServer();
	delete server;
}