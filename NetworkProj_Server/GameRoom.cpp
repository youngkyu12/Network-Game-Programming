#include "GameRoom.h"



GameRoom::GameRoom()
{
	// 생성할때 초기화
	InitializeCriticalSection(&_cs);

	m_nObjects = 4;
	m_ppObjects = new GameObject * [m_nObjects];

	int cols = 2;            // 가로로 4개
	int rows = 2;            // 세로로 4개
	float spacingX = 80.0f;  // X축 간격
	float spacingZ = 80.0f;  // Z축 간격

	for (int i = 0; i < m_nObjects; ++i) {
		int row = i / cols;
		int col = i % cols;

		// 가운데 기준으로 좌우/앞뒤로 벌어지게 배치
		float x = (col - (cols - 1) / 2.0f) * spacingX;
		float z = (row - (rows - 1) / 2.0f) * spacingZ;
		m_ppObjects[i] = new GameObject();
		m_ppObjects[i]->SetPosition(x, 2.0f, z);
		m_ppObjects[i]->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(10.0f, 5.0f, 2.0f),
			XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

GameRoom::~GameRoom()
{
	// 종료 시 제거
	DeleteCriticalSection(&_cs);
	for ( int i = 0; i < m_nObjects; i++ ) if ( m_ppObjects[i] ) delete m_ppObjects[i];
	if ( m_ppObjects ) delete[] m_ppObjects;
}

void GameRoom::Update_State(Player* player)
{
	UpdateState updatePkt;
	updatePkt.header.ID = MOVE; // 1
	updatePkt.My_ID = player->Player_ID;
	updatePkt.header.GameState = player->GetMyState();
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
		updatePkt.players[i].hp = p->GetHP ();
		updatePkt.players[i].x = pos.x;
		updatePkt.players[i].y = pos.y;
		updatePkt.players[i].z = pos.z;
		updatePkt.players[i].Look_x = Look.x;
		updatePkt.players[i].Look_y = Look.y;
		updatePkt.players[i].Look_z = Look.z;
		updatePkt.players[i].FireFlag = p->GetFireFlag();
		updatePkt.players[i].ShieldFlag = p->GetShieldFlag();
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
			player->SetEmptyBoundingBox();
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

void GameRoom::Check_State()
{
	if (PlayerManager.size() < 2)
	{
		/*
		원래는 여기도 락을 걸어주는게 맞지만,
		현재 규모에서는 문제 없을꺼 같아서 제외.
		추후에 버그 발생시 의심해야할 부분.
		*/
		return;
	}
	EnterCriticalSection(&_cs);
	bool allReady = true;
	for (auto player : PlayerManager)
	{
		if (player->GetMyState() != Ready)
		{
			allReady = false;
			break;
		}
	}

	if (allReady)
	{
		cout << "모든 플레이어 준비. 2로 변경" << endl;
		for (auto player : PlayerManager)
		{
			player->SetMyState(Playing);
		}
	}

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
	uint8_t PrevState = player->GetMyState();
	player->SetMyState(header->GameState);
	cout << (int)(player->GetMyState()) << endl;
	if (PrevState == 0 && header->GameState == 1)
	{
		// 상태의 변화가 있고 해당 값이 1일때만 실행.
		Check_State();
	}

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
			RayHitResult res = ProcessFire(player);
			player->Fire(); 
			// 발사 처리 및 결과
		}

		if (testpkt->shield == 1) // 쉴드 온
		{
			cout << "쉴드 온 처리 요청" << endl;
			// 쉴드 온 처리
			// 쉴드 타이머 시작 등등
			if (player->GetCooldownFlag() == 0) {
				player->Shield();
			}
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

	// 충돌 검사 후 위치 결정
	XMFLOAT3 resolvedPos;
	if (CheckPlayerByPlayerCollisions(player, xmf3Shift, resolvedPos))
	{
		// 이동 가능: 실제 이동량 = (결정된 위치 - 현재 위치)
		XMFLOAT3 cur = player->GetPosition();
		XMFLOAT3 delta(resolvedPos.x - cur.x, resolvedPos.y - cur.y, resolvedPos.z - cur.z);
		player->Move(delta);
		player->UpdateBoundingBox();
	}
	else
	{
		// 이동 불가: 위치 변화 없음
	}
}

void GameRoom::Rotate (Player* player, float yaw )
{
	//데이터를 수정할떄는 playermanager에 접근하면 안됩니다.
	player->Rotate (0.0f , yaw , 0.0f);
}

bool GameRoom::CheckPlayerByPlayerCollisions(Player* mover, const XMFLOAT3& desiredShift, XMFLOAT3& outResolvedPos)
{
	if (!mover)
	{
		return false;
	}

	// 현재 위치
	const XMFLOAT3 curPos = mover->GetPosition();
	// 이동 후 목표 위치(충돌 없을 때 반환할 위치)
	const XMFLOAT3 targetPos(curPos.x + desiredShift.x, curPos.y + desiredShift.y, curPos.z + desiredShift.z);

	// 이동 전/후 OOBB 준비
	mover->UpdateBoundingBox();
	BoundingOrientedBox movedBox = mover->GetBoundingBox();
	movedBox.Center.x += desiredShift.x;
	movedBox.Center.y += desiredShift.y;
	movedBox.Center.z += desiredShift.z;

	bool blocked = false;

	if (movedBox.Center.x < -100.0f || movedBox.Center.x > 100.0f ||
		movedBox.Center.z < -100.0f || movedBox.Center.z > 100.0f)
	{
		// 맵 경계 밖으로 나감
		blocked = true;
	}

	EnterCriticalSection(&_cs);
	const size_t count = PlayerManager.size();
	if (mover->GetMyState() == Playing) {
		for (size_t i = 0; i < count; ++i)
		{
			Player* other = PlayerManager[i];
			if (!other || other == mover) continue;

			other->UpdateBoundingBox();
			if (!other->HasBoundingBox()) {
				// 비활성화된 충돌체는 무시
				continue;
			}
			const BoundingOrientedBox& otherBox = other->GetBoundingBox();

			if (movedBox.Intersects(otherBox))
			{
				// 충돌 발견
				cout << "Player " << mover->Player_ID << " 충돌 with Player " << other->Player_ID << endl;
				blocked = true;
				break;
			}
		}
	}
	for (size_t i = 0; i < m_nObjects; ++i)
	{
		GameObject* other = m_ppObjects[i];

		other->UpdateBoundingBox();
		const BoundingOrientedBox& otherBox = other->GetBoundingBox();

		if (movedBox.Intersects(otherBox))
		{
			blocked = true;
			break;
		}
	}
	LeaveCriticalSection(&_cs);

	if (blocked)
	{
		// 충돌: 이동 불가 -> 현재 위치 반환
		outResolvedPos = curPos;
		return false;
	}
	else
	{
		// 충돌 없음: 이동 가능 -> 목표 위치 반환
		outResolvedPos = targetPos;
		return true;
	}
}

RayHitResult GameRoom::ProcessFire(Player* shooter)
{
	RayHitResult result;
	if (!shooter) return result;

	// Ray 정보 준비 (원점, 방향)
	const XMFLOAT3 originF = shooter->GetPosition();
	XMFLOAT3 dirF = shooter->GetLook();
	dirF = Vector3::Normalize(dirF); // 안전상 한 번 더 정규화

	XMVECTOR origin = XMLoadFloat3(&originF);
	XMVECTOR direction = XMLoadFloat3(&dirF);

	// 최대 사거리(필요 시 조정)
	const float kMaxRange = 1000.0f;

	float nearestDist = kMaxRange;
	Player* nearestTarget = nullptr;

	EnterCriticalSection(&_cs);
	// 최신 OOBB 갱신
	for (size_t i = 0; i < PlayerManager.size(); ++i)
	{
		Player* target = PlayerManager[i];
		if (!target || target == shooter) continue;

		target->UpdateBoundingBox();

		if ( !target->HasBoundingBox () ) {
			// 비활성화된 충돌체는 무시
			continue;
		}

		// Ray-OOBB 교차 검사
		float dist = 0.0f;
		if (target->GetBoundingBox().Intersects(origin, direction, dist))
		{
			// 사거리 내이고 더 가까운 대상이면 갱신
			if (dist >= 0.0f && dist < nearestDist)
			{
				if (target->GetShieldFlag() != 1) {
					nearestDist = dist;
					nearestTarget = target;
				}
			}
		}
	}
	for (size_t i = 0; i < m_nObjects; ++i)
	{
		GameObject* target = m_ppObjects[i];

		target->UpdateBoundingBox();

		// Ray-OOBB 교차 검사
		float dist = 0.0f;
		if (target->GetBoundingBox().Intersects(origin, direction, dist))
		{
			// 사거리 내이고 더 가까운 대상이면 갱신
			if (dist >= 0.0f && dist < nearestDist)
			{
				nearestDist = dist;
				nearestTarget = nullptr;
			}
		}
	}
	// 피격 처리
	if (nearestTarget)
	{
		// 피격 위치: origin + direction * nearestDist
		XMVECTOR hitPosV = XMVectorMultiplyAdd(direction, XMVectorReplicate(nearestDist), origin);
		XMFLOAT3 hitPosF;
		XMStoreFloat3(&hitPosF, hitPosV);

		// HP 감소(예: 10)
		const uint16_t damage = 10;
		uint16_t hp = nearestTarget->GetHP();
		uint16_t newHp = (hp > damage) ? (hp - damage) : 0;
		nearestTarget->SetHP(newHp);
		if (nearestTarget->GetHP() == 0)
		{
			nearestTarget->SetEmptyBoundingBox (); // 충돌체 비활성화
		}

		// 피격 결과 출력
		cout << "Player " << shooter->Player_ID << " 가 Player " << nearestTarget->Player_ID
			<< " 를 피격! 남은 HP: " << newHp << endl;

		result.hit = true;
		result.hitPos = hitPosF;
		result.hitPlayerId = nearestTarget->Player_ID;
	}
	LeaveCriticalSection(&_cs);

	return result;
}
