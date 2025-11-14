#pragma once
#include "pch.h"
#include "Player.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Update_State();
	void Add_Player(Player* player); // playermanger�� player ����
	void Remove_Player(Player* player); // playermanager���� player ����
	void Update_State(PlayerData *data);
	void Add_Player(Player* player);
	void Remove_Player(Player* player);
	void Check_PLayer(); /*������*/
	void HandlePacket(Player* player);

	void Move(char id, char key);
	void Rotate ( char id , POINT CursorPos );



private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 }; // ���� ���� �� �����Ǿ� �ִ� ���콺 ��ġ (�����ϸ� Ŭ�� ���� �����ؾ���)
};

