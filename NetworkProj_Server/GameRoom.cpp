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

void GameRoom::Remove_Player(Player* player)
{
	EnterCriticalSection(&_cs);
	Player* it;
	for (Player* p : PlayerManager)
	{
		if (p->Player_ID == player->Player_ID)
		{
			//찾음
			it = p;
			break;
			
		}
		
	}
	auto target = find(PlayerManager.begin(), PlayerManager.end(), it);
	PlayerManager.erase(target);
	LeaveCriticalSection(&_cs);

}

// 디버깅용
void GameRoom::Check_PLayer()
{
	EnterCriticalSection(&_cs);
	cout << PlayerManager.size()<< " 명 접속 완료" << endl;
	LeaveCriticalSection(&_cs);
}

void GameRoom::HandlePacket(Player* player)
{
	
	//BYTE* buffer = player->recvBuffer;
	//int32_t& bufferSize = player->recvByte; // 원본 참조

	//while (1)
	//{
	//	if (bufferSize < sizeof(/*Packetheader*/))
	//	{
	//		break;

	//	}
	//	//Pakcetheader* header = (Packetheader*)buffer;
	//	//packetSize = header->size;
	//	if (bufferSize < /*packetSize*/)
	//	{
	//		break;
	//	}

	//	switch (/*header->type*/)
	//	{
	//	case 1:
	//	{

	//	}
	//	case 2:
	//	{

	//	}
	//	default:
	//		break;

	//	}
	//}

	//int32_t remainingBytes = bufferSize - /*packetSize*/;
	//if (remainingBytes > 0)
	//{
	//	memmove(buffer, buffer + /*packetSize*/, remainingBytes);
	//}
	//bufferSize = remainingBytes;
	
}

void GameRoom::Move(char id, char key)
{
	float Distance = 0.15f;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

	switch (key)
	{
	case 'W':
		xmf3Shift = Vector3::Add(xmf3Shift, PlayerManager[id]->Look, Distance);
		break;
	case 'S':
		xmf3Shift = Vector3::Add(xmf3Shift, PlayerManager[id]->Look, -Distance);
		break;
	}

	PlayerManager[id]->Move(xmf3Shift, true);
}

void GameRoom::Rotate ( char id , POINT CursorPos )
{
	float cxMouseDelta = ( float )( CursorPos.x - OldCursorPos.x ) / 3.0f;
	float cyMouseDelta = ( float )( CursorPos.y - OldCursorPos.y ) / 3.0f;

	if ( cxMouseDelta || cyMouseDelta )
	{
		PlayerManager[id]->Rotate (0.0f , cxMouseDelta , 0.0f);
	}
}