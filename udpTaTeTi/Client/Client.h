#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class Client 
{
	bool shouldClose = false;
public:
	void RunClient();
	void RecieveString(SOCKET &socket, sockaddr_in &server);
};