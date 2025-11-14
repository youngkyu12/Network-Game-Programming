#pragma once
#include "pch.h"
#include "Player.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Add_Player(Player* player);
	void Remove_Player(Player* player);
	void Update_State(PlayerData *data);
	void Check_PLayer();
	void HandlePacket(Player* player, BYTE* buffer);

	void Move(char id, char key);
	void Rotate ( char id , POINT CursorPos );



private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 };
};

