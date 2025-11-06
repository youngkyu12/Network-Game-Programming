#pragma once
#include "pch.h"
#include "Player.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Update_State();
	void Add_Player(Player* player);
	void Check_PLayer(); /*µð¹ö±ë¿ë*/

private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;
};

