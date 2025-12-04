#include "GameRoom.h"



GameRoom::GameRoom()
{
	// �����Ҷ� �ʱ�ȭ
	InitializeCriticalSection(&_cs);

	m_nObjects = 4;
	m_ppObjects = new GameObject * [m_nObjects];

	int cols = 2;            // ���η� 4��
	int rows = 2;            // ���η� 4��
	float spacingX = 80.0f;  // X�� ����
	float spacingZ = 80.0f;  // Z�� ����

	for (int i = 0; i < m_nObjects; ++i) {
		int row = i / cols;
		int col = i % cols;

		// ��� �������� �¿�/�յڷ� �������� ��ġ
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
	// ���� �� ����
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
			//ã������ ����
			player->SetEmptyBoundingBox();
			PlayerManager.erase(p);
			break;
		}
	}
	LeaveCriticalSection(&_cs);

}

// ������
void GameRoom::Check_PLayer()
{
	EnterCriticalSection(&_cs);
	cout << PlayerManager.size()<< " �� ���� �Ϸ�" << endl;
	LeaveCriticalSection(&_cs);
}

void GameRoom::Check_State()
{
	EnterCriticalSection(&_cs);
	if (PlayerManager.size() < 2)
	{
		LeaveCriticalSection(&_cs);
		return;
	}
	
	bool allReady = true;
	for (auto player : PlayerManager)
	{
		cout << "Player " << player->Player_ID << " ���� " << player->GetMyState() << endl;
		if (player->GetMyState() != Ready)
		{
			allReady = false;
			break;
		}
	}

	if (allReady)
	{
		cout << "��� �÷��̾� �غ�. 2�� ����" << endl;
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
	 ���� ���ο��� GameRoom::Move�� ����
	 ������ ������ �ξ��� �Լ���� ó���ϸ� �˴ϴ�.
	---------------------------------------
	*/

	Packetheader* header = (Packetheader*)buffer;
	uint8_t PrevState = player->GetMyState();
	player->SetMyState(header->GameState);
	cout << (int)(player->GetMyState()) << endl;
	if (PrevState == None && header->GameState == Ready)
	{
		// ������ ��ȭ�� �ְ� �ش� ���� 1�϶��� ����.
		Check_State();
	}

	switch (header->ID)
	{
	case MOVE: //�̵���Ŷ
	{
		//cout << "MOVE ����ġ�� ���� �۵�" << endl;
		MovePacket* testpkt = (MovePacket*)buffer;

		if (testpkt->keyW == 1) Move(player, 'W');
		else if (testpkt->keyS == 1) Move(player, 'S');

		if (testpkt->yaw != 0) Rotate(player, testpkt->yaw);

		if (testpkt->FireFlag == 1)// True �̸�... 
		{
			RayHitResult res = ProcessFire(player);
			player->Fire(); 
			// �߻� ó�� �� ���
		}

		if (testpkt->shield == 1) // ���� ��
		{
			cout << "���� �� ó�� ��û" << endl;
			// ���� �� ó��
			// ���� Ÿ�̸� ���� ���
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

	// �浹 �˻� �� ��ġ ����
	XMFLOAT3 resolvedPos;
	if (CheckPlayerByPlayerCollisions(player, xmf3Shift, resolvedPos))
	{
		// �̵� ����: ���� �̵��� = (������ ��ġ - ���� ��ġ)
		XMFLOAT3 cur = player->GetPosition();
		XMFLOAT3 delta(resolvedPos.x - cur.x, resolvedPos.y - cur.y, resolvedPos.z - cur.z);
		player->Move(delta);
		player->UpdateBoundingBox();
	}
	else
	{
		// �̵� �Ұ�: ��ġ ��ȭ ����
	}
}

void GameRoom::Rotate (Player* player, float yaw )
{
	//�����͸� �����ҋ��� playermanager�� �����ϸ� �ȵ˴ϴ�.
	player->Rotate (0.0f , yaw , 0.0f);
}

bool GameRoom::CheckPlayerByPlayerCollisions(Player* mover, const XMFLOAT3& desiredShift, XMFLOAT3& outResolvedPos)
{
	if (!mover)
	{
		return false;
	}

	// ���� ��ġ
	const XMFLOAT3 curPos = mover->GetPosition();
	// �̵� �� ��ǥ ��ġ(�浹 ���� �� ��ȯ�� ��ġ)
	const XMFLOAT3 targetPos(curPos.x + desiredShift.x, curPos.y + desiredShift.y, curPos.z + desiredShift.z);

	// �̵� ��/�� OOBB �غ�
	mover->UpdateBoundingBox();
	BoundingOrientedBox movedBox = mover->GetBoundingBox();
	movedBox.Center.x += desiredShift.x;
	movedBox.Center.y += desiredShift.y;
	movedBox.Center.z += desiredShift.z;

	bool blocked = false;

	if (movedBox.Center.x < -100.0f || movedBox.Center.x > 100.0f ||
		movedBox.Center.z < -100.0f || movedBox.Center.z > 100.0f)
	{
		// �� ��� ������ ����
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
				// ��Ȱ��ȭ�� �浹ü�� ����
				continue;
			}
			const BoundingOrientedBox& otherBox = other->GetBoundingBox();

			if (movedBox.Intersects(otherBox))
			{
				// �浹 �߰�
				cout << "Player " << mover->Player_ID << " �浹 with Player " << other->Player_ID << endl;
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
		// �浹: �̵� �Ұ� -> ���� ��ġ ��ȯ
		outResolvedPos = curPos;
		return false;
	}
	else
	{
		// �浹 ����: �̵� ���� -> ��ǥ ��ġ ��ȯ
		outResolvedPos = targetPos;
		return true;
	}
}

RayHitResult GameRoom::ProcessFire(Player* shooter)
{
	RayHitResult result;
	if (!shooter) return result;

	// Ray ���� �غ� (����, ����)
	const XMFLOAT3 originF = shooter->GetPosition();
	XMFLOAT3 dirF = shooter->GetLook();
	dirF = Vector3::Normalize(dirF); // ������ �� �� �� ����ȭ

	XMVECTOR origin = XMLoadFloat3(&originF);
	XMVECTOR direction = XMLoadFloat3(&dirF);

	// �ִ� ��Ÿ�(�ʿ� �� ����)
	const float kMaxRange = 1000.0f;

	float nearestDist = kMaxRange;
	Player* nearestTarget = nullptr;

	EnterCriticalSection(&_cs);
	// �ֽ� OOBB ����
	for (size_t i = 0; i < PlayerManager.size(); ++i)
	{
		Player* target = PlayerManager[i];
		if (!target || target == shooter) continue;

		target->UpdateBoundingBox();

		if ( !target->HasBoundingBox () ) {
			// ��Ȱ��ȭ�� �浹ü�� ����
			continue;
		}

		// Ray-OOBB ���� �˻�
		float dist = 0.0f;
		if (target->GetBoundingBox().Intersects(origin, direction, dist))
		{
			// ��Ÿ� ���̰� �� ����� ����̸� ����
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

		// Ray-OOBB ���� �˻�
		float dist = 0.0f;
		if (target->GetBoundingBox().Intersects(origin, direction, dist))
		{
			// ��Ÿ� ���̰� �� ����� ����̸� ����
			if (dist >= 0.0f && dist < nearestDist)
			{
				nearestDist = dist;
				nearestTarget = nullptr;
			}
		}
	}
	// �ǰ� ó��
	if (nearestTarget)
	{
		// �ǰ� ��ġ: origin + direction * nearestDist
		XMVECTOR hitPosV = XMVectorMultiplyAdd(direction, XMVectorReplicate(nearestDist), origin);
		XMFLOAT3 hitPosF;
		XMStoreFloat3(&hitPosF, hitPosV);

		// HP ����(��: 10)
		const uint16_t damage = 10;
		uint16_t hp = nearestTarget->GetHP();
		uint16_t newHp = (hp > damage) ? (hp - damage) : 0;
		nearestTarget->SetHP(newHp);
		if (nearestTarget->GetHP() == 0)
		{
			nearestTarget->SetEmptyBoundingBox (); // �浹ü ��Ȱ��ȭ
		}

		// �ǰ� ��� ���
		cout << "Player " << shooter->Player_ID << " �� Player " << nearestTarget->Player_ID
			<< " �� �ǰ�! ���� HP: " << newHp << endl;

		result.hit = true;
		result.hitPos = hitPosF;
		result.hitPlayerId = nearestTarget->Player_ID;
	}
	LeaveCriticalSection(&_cs);

	return result;
}
