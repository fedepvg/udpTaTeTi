#include "Client.h"


using namespace std;

#define NEW_USER '0'
#define SET_ALIAS '1'
#define MAKE_MOVE '2'
#define OTHERS_TURN '3'
#define MY_TURN '4'
#define BAD_MOVE '5'
#define GAME_ENDED '6'

struct Message
{
	byte cmd;
	char data[255];
};

struct User
{
	char alias[255];
	sockaddr_in id;
};

void Client::RunClient()
{

	// inicializar winsock

	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	Message msg;

	if (WSAStartup(version, &data) == 0)
	{
		string ipToUse("127.0.0.1");
		int portToUse = 8900;

		//cout << "Escribe la ip a usar: ";
		//cin >> ipToUse;
		//cout << endl;
		//cout << "Escribe el puerto a usar: ";
		//cin >> portToUse;
		//cout << endl;
		//
		//cin.ignore();

		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(portToUse);
		inet_pton(AF_INET, ipToUse.c_str(), &server.sin_addr);

		SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

		string msgtest = "";

		char buf[1024];
		int serverSize = sizeof(server);

		msg.cmd = (byte)NEW_USER;
		memset(msg.data, 0, sizeof(msg.data));
		sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));

		memset(&msg, 0, sizeof(msg));

		cout << "Escribe tu alias: ";

		getline(cin, msgtest);
		strcpy_s(msg.data, msgtest.c_str());

		msg.cmd = (byte)SET_ALIAS;
		sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));

		RecieveString(out, server);
		RecieveString(out, server);
		RecieveString(out, server);

		do
		{
			memset(buf, 0, sizeof(buf));
			memset(&msg, 0, sizeof(msg));

			int bytesIn = recvfrom(out, (char*)&msg, sizeof(msg), 0, (sockaddr*)&server, &serverSize);

			if (bytesIn == SOCKET_ERROR)
			{
				cerr << "Error al recibir data" << endl;
				return;
			}
			if(msg.data != " ")
				cout << msg.data << endl;

			bool skipTurn = false;
			bool gameEnded = false;
			switch (msg.cmd)
			{
			case OTHERS_TURN:
				cout << "Turno del Rival" << endl;
				skipTurn = true;
				break;
			case MY_TURN:
				cout << "Tu Turno" << endl;
				break;
			case BAD_MOVE:
				cout << "Jugada Invalida, vuelva a intentar" << endl;
				break;
			case GAME_ENDED:
				gameEnded = true;
				memset(&msg, 0, sizeof(msg));
				strcpy_s(msg.data, " ");
				msg.cmd = GAME_ENDED;
				sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));
			default:
				break;
			}

			if (skipTurn)
				continue;

			//cout << "echo: " << buf << endl;

			memset(&msg, 0, sizeof(msg));

			if (!gameEnded)
			{
				cout << "Escribe tu jugada: ";

				getline(cin, msgtest);
				strcpy_s(msg.data, msgtest.c_str());
				msg.cmd = MAKE_MOVE;

				sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));
			}
			else
				msgtest = "close";
		} while (msgtest != "close");

		// cerrar el socket
		closesocket(out);
	}
	else
	{
		return;
	}

	// cerrar winsock
	WSACleanup();
}

void Client::RecieveString(SOCKET &socket, sockaddr_in &server)
{
	char buf[1024];
	int serverSize = sizeof(server);
	int bytesIn = recvfrom(socket, buf, sizeof(buf), 0, (sockaddr*)&server, &serverSize);

	if (bytesIn == SOCKET_ERROR)
	{
		cerr << "Error al recibir data" << endl;
		return;
	}

	cout << buf << endl;
}
