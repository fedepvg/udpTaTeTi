#include "TaTeTi.h"

#include "iostream"
#include <vector>
#include <cmath>

using namespace std;

TaTeTi::TaTeTi()
{
	//int playerInput;

	ResetGrid();

	//while (!gameEnded)
	//{
	//	cout << "input number" << endl;
	//	bool validInput = false;
	//	while (!validInput)
	//	{
	//		cin >> playerInput;
	//		validInput = CheckInput(playerInput, currentGamestate);
	//	}
	//	currentGamestate.cellArray[playerInput - 1] = dot;
	//
	//	for (int i = 2; i >= 0; i--)
	//	{
	//		for (int i2 = 0; i2 < 3; i2++)
	//		{
	//			cout << CellToChar(currentGamestate.cellArray[i * 3 + i2]) << "  ";
	//		}
	//		cout << endl;
	//	}
	//	//if (CheckAll(currentGamestate) || turnsLeft == 0)
	//	//	gameEnded = true;
	//	//turnsLeft -= 2;
	//}
}

bool TaTeTi::CheckVertical(Gamestate gamestate)
{
	if (gamestate.cellArray[6] != emptyCell && gamestate.cellArray[6] == gamestate.cellArray[3] && gamestate.cellArray[3] == gamestate.cellArray[0])
		return true;
	else
		if (gamestate.cellArray[7] != emptyCell && gamestate.cellArray[7] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[1])
			return true;
		else
			if (gamestate.cellArray[8] != emptyCell && gamestate.cellArray[8] == gamestate.cellArray[5] && gamestate.cellArray[5] == gamestate.cellArray[2])
				return true;
			else
				return false;
}

bool TaTeTi::CheckHorizontal(Gamestate gamestate)
{
	if (gamestate.cellArray[6] != emptyCell && gamestate.cellArray[6] == gamestate.cellArray[7] && gamestate.cellArray[7] == gamestate.cellArray[8])
		return true;
	else
		if (gamestate.cellArray[3] != emptyCell && gamestate.cellArray[3] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[5])
			return true;
		else
			if (gamestate.cellArray[0] != emptyCell && gamestate.cellArray[0] == gamestate.cellArray[1] && gamestate.cellArray[1] == gamestate.cellArray[2])
				return true;
			else
				return false;
}

bool TaTeTi::CheckCrosses(Gamestate gamestate)
{
	if (gamestate.cellArray[6] != emptyCell && gamestate.cellArray[6] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[2])
		return true;
	else
		if (gamestate.cellArray[8] != emptyCell && gamestate.cellArray[8] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[0])
			return true;
		else
			return false;
}

void TaTeTi::ResetGrid()
{
	gameEnded = false;
	matchResult = stillPlaying;
	turnsLeft = 9;
	for (int i = 0; i < 9; i++)
	{
		currentGamestate.cellArray[i] = emptyCell;
	}
}

void TaTeTi::ResetPlayer(User*& thisPlayer)
{
	if (firstPlayer == thisPlayer)
		firstPlayer = nullptr;
	else
		secondPlayer = nullptr;
}

bool TaTeTi::CheckAll(Gamestate gamestate)
{
	return CheckHorizontal(gamestate) || CheckVertical(gamestate) || CheckCrosses(gamestate);
}

bool TaTeTi::CheckInput(int input, Gamestate gamestate)
{
	if (input > 0 && input <= 9 && gamestate.cellArray[input - 1] == emptyCell)
		return true;
	else
		return false;
}

char TaTeTi::CellToChar(Cell input)
{
	char output;
	switch (input)
	{
	case emptyCell:
		output = '.';
		break;
	case dot:
		output = 'O';
		break;
	case cross:
		output = 'X';
		break;
	}
	return output;
}

bool TaTeTi::MakeMove(int cell)
{
	if (CheckInput(cell, currentGamestate))
	{
		currentGamestate.cellArray[cell - 1] = currentTurnPlayer->cellType;
		turnsLeft--;
		User* playerAux = nextTurnPlayer;
		nextTurnPlayer = currentTurnPlayer;
		currentTurnPlayer = playerAux;
		//if(CheckAll(currentGamestate) || turnsLeft == 0)
		//	gameEnded=true;
		return true;
	}
	else
		return false;
}

MatchResult TaTeTi::GetMatchResult()
{
	if (CheckAll(currentGamestate))
	{
		gameEnded = true;
		return matchResult = win;
	}
	if (turnsLeft == 0)
	{
		gameEnded = true;
		return matchResult = draw;
	}

	return matchResult = stillPlaying;
}

void TaTeTi::AddPlayer(User* &newPlayer)
{
	if (firstPlayer)
	{
		secondPlayer = newPlayer;
		secondPlayer->currentRoom = this;
		return;
	}

	firstPlayer = newPlayer;
	firstPlayer->currentRoom = this;
}

bool TaTeTi::CanRecievePlayer()
{
	if (firstPlayer && secondPlayer)
		return false;

	return true;
}

void TaTeTi::SetupPlayers()
{
	int firstTurn = rand() % 2 + 1;
	if (firstTurn == 1)
	{
		currentTurnPlayer = firstPlayer;
		nextTurnPlayer = secondPlayer;
	}
	else
	{
		currentTurnPlayer = secondPlayer;
		nextTurnPlayer = firstPlayer;
	}
	firstPlayer->cellType = dot;
	secondPlayer->cellType = cross;

	firstPlayer->restart = false;
	secondPlayer->restart = false;
	ResetGrid();
}

User* TaTeTi::GetCurrentTurnPlayer()
{
	return currentTurnPlayer;
}

User* TaTeTi::GetNextTurnPlayer()
{
	return nextTurnPlayer;
}

User* TaTeTi::GetOtherPlayer(User*& thisPlayer)
{
	if (firstPlayer == thisPlayer)
		return secondPlayer;

	return firstPlayer;
}

string TaTeTi::GetCurrentGameBoard()
{
	currentGameBoard = "";
	currentGameBoard += '\n';
	for (int i = 2; i >= 0; i--)
	{
		for (int i2 = 0; i2 < 3; i2++)
		{
			currentGameBoard += CellToChar(currentGamestate.cellArray[i * 3 + i2]);
			currentGameBoard +=  ' ';
		}
		currentGameBoard += '\n';
	}

	return currentGameBoard;
}
