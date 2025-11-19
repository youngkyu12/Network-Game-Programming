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

	EnterCriticalSection(&_cs);
	int32_t playerCount = PlayerManager.size();
	if (playerCount > MAX_PLAYERS)
	{
		playerCount = MAX_PLAYERS;
	}

	updatePkt.numPlayers = playerCount;

	uint16_t myID = player->Player_ID;

	for (int i = 0; i < playerCount; ++i)
	{
		Player* p = PlayerManager[i];
		XMFLOAT3 pos = p->GetPosition();
		uint16_t hp = p->GetHP();

		updatePkt.players[i].playerID = p->Player_ID;
		//if (p->Player_ID == myID) updatePkt.players[i].playerID = 0;	// 나 = 0
		//else if (p->Player_ID != myID) updatePkt.players[i].playerID = 1;	// 나X = 1 2인 기준, 3인 되면 변경
		updatePkt.players[i].x = pos.x;
		updatePkt.players[i].y = pos.y;
		updatePkt.players[i].z = pos.z;
		//updatePkt.players[i].hp = hp;
	}
	LeaveCriticalSection(&_cs);

	uint16_t packetSize = sizeof(Packetheader) + sizeof(int32_t) + (playerCount * sizeof(PlayerState));
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
		cout << "MOVE 스위치문 정상 작동" << endl;
		MovePacket* testpkt = (MovePacket*)buffer;

		if (testpkt->keyW == 1) Move(player, 'W');
		else if (testpkt->keyS == 1) Move(player, 'S');
		//player->SetPosition(testpkt->x, testpkt->y, testpkt->z);

		//cout << "x = " << testpkt->x << endl;
		//cout << "y = " << testpkt->y << endl;
		//cout << "z = " << testpkt->z << endl;

		XMFLOAT3 currentPos = player->GetPosition();//테스트용 임시 저장
		cout << currentPos.x << ", " << currentPos.y << ", " << currentPos.z << endl;
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

/*
HandlePacket 내부에서만 실행될 함수이고
HandlePacket은 이미 Player* player를 알고 있습니다.
플레이어 매니저를 통해서 찾을 필요 없어요.
*/
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
	//UpdateMove(id, xmf3Shift);
	
}

void GameRoom::Rotate ( char id , POINT CursorPos )
{
	/*
	float cxMouseDelta = ( float )( CursorPos.x - OldCursorPos.x ) / 3.0f;
	float cyMouseDelta = ( float )( CursorPos.y - OldCursorPos.y ) / 3.0f;

	if ( cxMouseDelta || cyMouseDelta )
	{
		PlayerManager[id]->Yaw = cxMouseDelta;

		// 밑에 rotate는 클라와 서버의 방향벡터가 같은 지 확인하기 위함
		PlayerManager[id]->Rotate (0.0f , cxMouseDelta , 0.0f);
	}
	*/
}



// 테스트
void GameRoom::UpdateMove(char id, XMFLOAT3 xmf3shift)
{
	UpdateMoveState updateMPKT;
	updateMPKT.header.ID = MOVE;
	updateMPKT.header.size = sizeof(UpdateMoveState);
	updateMPKT.player.x = xmf3shift.x;
	updateMPKT.player.y = xmf3shift.y;
	updateMPKT.player.z = xmf3shift.z;
	EnterCriticalSection(&_cs);
	int32_t playerCount = PlayerManager.size();
	if (playerCount > MAX_PLAYERS)
	{
		playerCount = MAX_PLAYERS;
	}

	for (int i = 0; i < playerCount; ++i) {
		if (PlayerManager[i]->Player_ID == id) {
			updateMPKT.player.playerID = 0; // 자기자신 이동
		}
		else if (PlayerManager[i]->Player_ID != id) {
			updateMPKT.player.playerID = 1; // 상대 이동
		}
		cout << "무브전송" << endl;
		send(PlayerManager[i]->sock, (char*)&updateMPKT, updateMPKT.header.size, 0);
	}
	LeaveCriticalSection(&_cs);
}

void GameRoom::UpdatePlayer(char id)
{
	PlayerManager[id]->SetPosition(0, 0, -50 + (id * 100));
	PlayerManager[id]->SetLook(0, 0, 1 - (id * 2));
	UpdatePlayerState updatePPKT;
	updatePPKT.header.ID = START;
	updatePPKT.header.size = sizeof(UpdatePlayerState);
	for (int i = 0; i < PlayerManager.size(); ++i) {
		for (int j = 0; j < PlayerManager.size(); ++j) {
			XMFLOAT3 pos = PlayerManager[j]->GetPosition();
			XMFLOAT3 Look = PlayerManager[j]->GetLook();
			if (PlayerManager[i]->Player_ID == PlayerManager[j]->Player_ID) {
				updatePPKT.player.playerID = 0; // 자기자신
				updatePPKT.player.x = pos.x;
				updatePPKT.player.y = pos.y;
				updatePPKT.player.z = pos.z;
				updatePPKT.player.LookX = Look.x;
				updatePPKT.player.LookY = Look.y;
				updatePPKT.player.LookZ = Look.z;
			}
			else if (PlayerManager[i]->Player_ID != PlayerManager[j]->Player_ID) {
				updatePPKT.player.playerID = 1;
				updatePPKT.player.x = pos.x;
				updatePPKT.player.y = pos.y;
				updatePPKT.player.z = pos.z;
				updatePPKT.player.LookX = Look.x;
				updatePPKT.player.LookY = Look.y;
				updatePPKT.player.LookZ = Look.z;
			}
			cout << "시작위치 전송" << endl;
			send(PlayerManager[i]->sock, (char*)&updatePPKT, updatePPKT.header.size, 0);
		}
	}
}