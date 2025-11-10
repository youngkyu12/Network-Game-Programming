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
	void Remove_Player(Player* player);
	void Check_PLayer(); /*디버깅용*/
	void Move(char id, char key);
	void Rotate ( char id , POINT CursorPos );
private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 }; // 게임 진행 중 고정되어 있는 마우스 위치 (변경하면 클라도 같이 변경해야함)
};

