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

void GameRoom::Update_State( PlayerData* data )
{
	for ( int i = 0; i < PlayerManager.size (); ++i ) {
		data->x = PlayerManager[i]->Position.x;
		data->y = PlayerManager[i]->Position.y;
		data->z = PlayerManager[i]->Position.z;
		data->yaw = PlayerManager[i]->Yaw;
	}
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

void GameRoom::HandlePacket(Player* player, BYTE* buffer)
{
	/*
	---------------------------------------
	 여기 내부에서 GameRoom::Move와 같은
	 기존에 설계해 두었던 함수들로 처리하면 됩니다.
	---------------------------------------
	*/

	Packetheader* header = (Packetheader*)buffer;

	switch (header->ID)
	{
	case MOVE: //이동패킷
	{
		cout << "MOVE 스위치문 정상 작동" << endl;
		MovePacket* testpkt = (MovePacket*)buffer;
		cout << "x = " << testpkt->x << endl;
		cout << "y = " << testpkt->y << endl;
		cout << "z = " << testpkt->z << endl;
		break;
	}
	case TEMP:
	{
		cout << "TEMP 스위치문 작동" << endl;
		break;
	}
	default:
	{
		cout << player->Player_ID << " : " << header->ID << endl;
		break;
	}
	}
	
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
		PlayerManager[id]->Yaw = cxMouseDelta;

		// 밑에 rotate는 클라와 서버의 방향벡터가 같은 지 확인하기 위함
		PlayerManager[id]->Rotate (0.0f , cxMouseDelta , 0.0f);
	}
}