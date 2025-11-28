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
	void Rotate(Player* player, float yaw);

	bool CheckPlayerByPlayerCollisions(Player* mover, const XMFLOAT3& desiredShift, XMFLOAT3& outResolvedPos);


private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 };
};

