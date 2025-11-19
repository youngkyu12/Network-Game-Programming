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
	void Update_State(Player* player);
	void Check_PLayer();
	void HandlePacket(Player* player, BYTE* buffer);

	void Move(Player* player, char key);
	void Rotate ( char id , POINT CursorPos );
	
	// Å×½ºÆ®
	void UpdateMove(char id, XMFLOAT3 xmf3shift);
	void UpdatePlayer(char id);
private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 };
};

