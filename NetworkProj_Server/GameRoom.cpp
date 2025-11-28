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

void GameRoom::Update_State(Player* player)
{
	UpdateState updatePkt;
	updatePkt.header.ID = MOVE; // 1
	updatePkt.My_ID = player->Player_ID;
	EnterCriticalSection(&_cs);
	int32_t playerCount = PlayerManager.size();
	if (playerCount > MAX_PLAYERS)
	{
		playerCount = MAX_PLAYERS;
	}

	updatePkt.numPlayers = playerCount;

	for (int i = 0; i < playerCount; ++i)
	{
		Player* p = PlayerManager[i];
		XMFLOAT3 pos = p->GetPosition();
		XMFLOAT3 Look = p->GetLook();

		updatePkt.players[i].playerID = p->Player_ID;
		updatePkt.players[i].x = pos.x;
		updatePkt.players[i].y = pos.y;
		updatePkt.players[i].z = pos.z;
		updatePkt.players[i].Look_x = Look.x;
		updatePkt.players[i].Look_y = Look.y;
		updatePkt.players[i].Look_z = Look.z;
		updatePkt.players[i].FireFlag = p->GetFireFlag();
	}
	LeaveCriticalSection(&_cs);

	uint16_t packetSize = sizeof(Packetheader) + sizeof(uint8_t) + sizeof(int32_t) + (playerCount * sizeof(PlayerState));
	updatePkt.header.size = packetSize;
	
	send(player->sock, (char*)&updatePkt, packetSize, 0);
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

	for (vector<Player*>::iterator p = PlayerManager.begin(); p !=PlayerManager.end(); ++p)
	{
		if ((*p)->Player_ID == player->Player_ID)
		{
			//찾았으면 삭제
			PlayerManager.erase(p);
			break;
		}
	}
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
		//cout << "MOVE 스위치문 정상 작동" << endl;
		MovePacket* testpkt = (MovePacket*)buffer;

		if (testpkt->keyW == 1) Move(player, 'W');
		else if (testpkt->keyS == 1) Move(player, 'S');

		if (testpkt->yaw != 0) Rotate(player, testpkt->yaw);

		if (testpkt->FireFlag == 1)// True 이면... 
		{
			player->Fire(); 
		}

		break;
	}
	default:
	{
		cout << player->Player_ID << " : " << header->ID << endl;
		break;
	}
	}
	
}

void GameRoom::Move(Player* player, char key)
{
	float Distance = 0.15f;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

	switch (key)
	{
	case 'W':
		xmf3Shift = Vector3::Add(xmf3Shift, player->GetLook(), Distance);
		break;
	case 'S':
		xmf3Shift = Vector3::Add(xmf3Shift, player->GetLook(), -Distance);
		break;
	}

	player->Move(xmf3Shift);
}

void GameRoom::Rotate (Player* player, float yaw )
{
	//데이터를 수정할떄는 playermanager에 접근하면 안됩니다.
	player->Rotate (0.0f , yaw , 0.0f);
}