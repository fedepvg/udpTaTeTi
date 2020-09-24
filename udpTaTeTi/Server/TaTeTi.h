#pragma once

#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

enum Cell
{
	emptyCell, dot, cross, count
};

struct Gamestate
{
	Cell cellArray[9];
};

struct Node
{
	Gamestate state;
	int value;
	//vector<Node>Childs;
};

struct User
{
	sockaddr_in id;
	char alias[255];
	class TaTeTi* currentRoom;
	Cell cellType;
};


class TaTeTi
{
private:
	bool gameEnded = false;
	Gamestate currentGamestate;
	int turnsLeft = 8;

	User* firstPlayer = nullptr;
	User* secondPlayer = nullptr;
	User* currentTurnPlayer;
	User* nextTurnPlayer;
	string currentGameBoard;

	bool CheckVertical(Gamestate gamestate);
	bool CheckHorizontal(Gamestate gamestate);
	bool CheckCrosses(Gamestate gamestate);
public:
	TaTeTi();

	bool CheckAll(Gamestate gamestate);
	bool CheckInput(int input, Gamestate gamestate);
	char CellToChar(Cell input);
	bool MakeMove(int cell);
	bool GetGameEnded() { return gameEnded; }
	void AddPlayer(User* &newPlayer);
	bool CanRecievePlayer();
	void SetupPlayers();
	User* GetCurrentTurnPlayer();
	User* GetNextTurnPlayer();
	string GetCurrentGameBoard();
};