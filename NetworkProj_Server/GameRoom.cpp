#include "GameRoom.h"

GameRoom::GameRoom()
{
	// 생성할때 초기화
	InitializeCriticalSection(&_cs);
}

GameRoom::~GameRoom()
{
	// 종료 시 제거
	DeleteCriticalSection(&_cs);
}

void GameRoom::Update_State()
{
}

void GameRoom::Add_Player(Player* player)
{
	EnterCriticalSection(&_cs);
	PlayerManager.push_back(player);
	LeaveCriticalSection(&_cs);
}

// 디버깅용
void GameRoom::Check_PLayer()
{
	EnterCriticalSection(&_cs);
	cout << PlayerManager.size()<< " 명 접속 완료" << endl;
	LeaveCriticalSection(&_cs);
}
