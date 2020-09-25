#include "Server.h"

#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include "TaTeTi.h"
#include <time.h>

#pragma comment (lib, "ws2_32.lib")

#define NEW_USER '0'
#define SET_ALIAS '1'
#define MATCH_START '2'
#define MAKE_MOVE '3'
#define OTHERS_TURN '4'
#define MY_TURN '5'
#define BAD_MOVE '6'
#define GAME_ENDED '7'
#define NEW_GAME '8'

using namespace std;

struct Message
{
	byte cmd;
	char data[255];
};

void Server::RunServer()
{
	srand(time(NULL));

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	Message msg;
	vector<User*> users;

	vector<TaTeTi*> roomVector;

	int wsOk = WSAStartup(ver, &data);
	if (wsOk != 0)
	{
		cerr << "no pudo iniciar winsock" << endl;
		return;
	}

	SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Invalid socket" << endl;
		return;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8900); 
	inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // ip de loopback

	int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (bindResult == SOCKET_ERROR)
	{
		cerr << "No pudo hacer el bind" << endl;
		return;
	}

	char buf[1024];
	sockaddr_in client;
	int clientSize = sizeof(client);

	do
	{
		memset(buf, 0, sizeof(buf));
		memset(&msg, 0, sizeof(msg));

		//funcion bloqueante
		int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, &clientSize);

		if (bytesIn == SOCKET_ERROR)
		{
			cerr << "Error al recibir data" << endl;
			return;
		}

		switch (msg.cmd)
		{
		case NEW_USER:
		{
			User* u = new User();
			u->id = client;
			users.push_back(u);
			cout << "Nuevo usuario creado" << endl;
			break;
		}
		case SET_ALIAS:
		{
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->id;

				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					cout << "Alias del usuario " << i << " :" << msg.data << endl;
					strcpy_s(users[i]->alias, msg.data);
					string toSend = "Hola ";
					toSend += (char*)&msg.data;
					sendto(listening, toSend.c_str(), sizeof(toSend), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

					bool roomAvailable;
					roomAvailable = false;

					Message inLobbyMsg; 
					strcpy_s(inLobbyMsg.data, "Esperando a encontrar un rival");
					sendto(listening, inLobbyMsg.data, sizeof(inLobbyMsg.data), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

					if (roomVector.size() != 0)
					{
						for (int j = 0; j < roomVector.size(); j++)
						{
							if (roomVector[j]->CanRecievePlayer())
							{
								roomVector[j]->AddPlayer(users[i]);
								roomAvailable = true;

								string msg = "Iniciando partida contra ";
								inLobbyMsg.cmd = MATCH_START;
								strcpy_s(inLobbyMsg.data, msg.c_str());
								strcat_s(inLobbyMsg.data, roomVector[j]->GetCurrentTurnPlayer()->alias);
								sendto(listening, (char*)&inLobbyMsg, sizeof(Message), 0, (sockaddr*)&roomVector[j]->GetNextTurnPlayer()->id, sizeof(users[i]->id));

								strcpy_s(inLobbyMsg.data, msg.c_str());
								strcat_s(inLobbyMsg.data, roomVector[j]->GetNextTurnPlayer()->alias);
								sendto(listening, (char*)&inLobbyMsg, sizeof(Message), 0, (sockaddr*)& roomVector[j]->GetCurrentTurnPlayer()->id, sizeof(users[i]->id));


								Message matchStartMsg;
								matchStartMsg.cmd = OTHERS_TURN;
								strcpy_s(matchStartMsg.data, " ");
								sendto(listening, (char*)&matchStartMsg, sizeof(Message), 0, (sockaddr*)&roomVector[j]->GetNextTurnPlayer()->id, sizeof(users[i]->id));
								matchStartMsg.cmd = MY_TURN;
								strcpy_s(matchStartMsg.data, " ");
								sendto(listening, (char*)&matchStartMsg, sizeof(Message), 0, (sockaddr*)&roomVector[j]->GetCurrentTurnPlayer()->id, sizeof(users[i]->id));
								break;
							}
						}
						if (roomAvailable)
							break;
					}

					if (!roomAvailable)
					{
						TaTeTi* room = new TaTeTi();
						roomVector.push_back(room);
						room->AddPlayer(users[i]);
					}

					break;
				}
			}
			break;
		}
		case MAKE_MOVE:
		{
			int clientMove = (int)msg.data[0] - '0';
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->id;

				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					TaTeTi* currentRoom = users[i]->currentRoom;
					if (users[i] == currentRoom->GetCurrentTurnPlayer())
					{
						if (users[i]->currentRoom->MakeMove(clientMove))
						{
							User* nextTurnPlayer = currentRoom->GetCurrentTurnPlayer();
							Message board;
							strcpy_s(board.data, currentRoom->GetCurrentGameBoard().c_str());
							if (currentRoom->GetGameEnded())
							{
								Message secondBoard = board;
								strcat_s(board.data, "\n Ganaste");

								board.cmd = GAME_ENDED;
								sendto(listening, (char*)&board, sizeof(Message), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

								strcat_s(secondBoard.data, "\n Perdiste");
								secondBoard.cmd = GAME_ENDED;
								sendto(listening, (char*)&secondBoard, sizeof(Message), 0, (sockaddr*)&nextTurnPlayer->id, sizeof(nextTurnPlayer->id));

								break;
							}
							board.cmd = OTHERS_TURN;
							sendto(listening, (char*)&board, sizeof(Message), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

							board.cmd = MY_TURN;
							sendto(listening, (char*)&board, sizeof(Message), 0, (sockaddr*)&nextTurnPlayer->id, sizeof(nextTurnPlayer->id));
							break;
						}
					}
					Message invalid;
					invalid.cmd = BAD_MOVE;
					strcpy_s(invalid.data, " ");
					sendto(listening, (char*)&invalid, sizeof(Message), 0, (sockaddr*)&client, sizeof(client));
					break;
				}
			}
			break;
		}
		case GAME_ENDED:
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->id;

				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					if (users[i]->currentRoom->GetOtherPlayer(users[i]) == nullptr)
					{
						for (int j = 0; j < roomVector.size(); j++)
						{
							if (roomVector[j] == users[i]->currentRoom)
							{
								delete roomVector[j];
								vector<TaTeTi*>::iterator iter = find(roomVector.begin(), roomVector.end(), roomVector[j]);
								roomVector.erase(iter);
							}
						}
					}
					if (users[i] != nullptr)
					{
						users[i]->currentRoom->ResetPlayer(users[i]);
						delete users[i];
						vector<User*>::iterator iter = find(users.begin(), users.end(), users[i]);
						users.erase(iter);
					}
				}

			}
			break;
		case NEW_GAME:
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->id;

				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					users[i]->restart = true;
					Message inLobbyMsg;
					inLobbyMsg.cmd = MATCH_START;
					strcpy_s(inLobbyMsg.data, "Esperando a tu rival");
					sendto(listening, (char*)&inLobbyMsg, sizeof(Message), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

					if (users[i]->currentRoom->GetOtherPlayer(users[i]) && users[i]->currentRoom->GetOtherPlayer(users[i])->restart)
					{
						users[i]->currentRoom->SetupPlayers();

						string msg = "Iniciando partida contra ";
						inLobbyMsg.cmd = MATCH_START;
						strcpy_s(inLobbyMsg.data, msg.c_str());
						strcat_s(inLobbyMsg.data, users[i]->currentRoom->GetOtherPlayer(users[i])->alias);
						sendto(listening, (char*)&inLobbyMsg, sizeof(Message), 0, (sockaddr*)&users[i]->id, sizeof(users[i]->id));

						strcpy_s(inLobbyMsg.data, msg.c_str());
						strcat_s(inLobbyMsg.data, users[i]->alias);
						sendto(listening, (char*)&inLobbyMsg, sizeof(Message), 0, (sockaddr*)&users[i]->currentRoom->GetOtherPlayer(users[i])->id, sizeof(users[i]->id));


						Message matchStartMsg;
						matchStartMsg.cmd = OTHERS_TURN;
						strcpy_s(matchStartMsg.data, " ");
						sendto(listening, (char*)&matchStartMsg, sizeof(Message), 0, (sockaddr*)&users[i]->currentRoom->GetNextTurnPlayer()->id, sizeof(users[i]->id));
						matchStartMsg.cmd = MY_TURN;
						strcpy_s(matchStartMsg.data, " ");
						sendto(listening, (char*)&matchStartMsg, sizeof(Message), 0, (sockaddr*)&users[i]->currentRoom->GetCurrentTurnPlayer()->id, sizeof(users[i]->id));
					}
				}
			}
			break;
		default:
			sendto(listening, (char*)&"invalido", sizeof(Message), 0, (sockaddr*)&client, sizeof(client));
			break;
		}

	} while ((string)msg.data != "close");

	closesocket(listening);
	WSACleanup();

}
