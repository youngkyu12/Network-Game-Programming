#pragma once
#include "pch.h"
#include "Player.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Update_State();
	void Add_Player(Player* player); // playermanger에 player 삽입
	void Remove_Player(Player* player); // playermanager에서 player 삭제
	void Check_PLayer(); /*디버깅용*/
	void Move(char id, char key);

private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;
};

