#pragma once
#include "pch.h"
#include "Player.h"

// 발사 처리 결과
struct RayHitResult
{
	bool hit = false;            // 충돌 여부
	XMFLOAT3 hitPos = { 0,0,0 };   // 충돌 위치(월드)
	int hitPlayerId = -1;        // 피격된 플레이어 ID (없으면 -1)
};


class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Add_Player(Player* player);
	void Remove_Player(Player* player);
	void Update_State(Player* player);
	void Check_PLayer();
	void Check_State();
	void HandlePacket(Player* player, BYTE* buffer);

	void Move(Player* player, char key);
	void Rotate(Player* player, float yaw);

	bool CheckPlayerByPlayerCollisions(Player* mover, const XMFLOAT3& desiredShift, XMFLOAT3& outResolvedPos);

	// 플레이어 발사 처리: 슈터의 위치/시선으로 Ray 발사하여
	// - 다른 플레이어 OOBB와의 충돌 검사
	// - 가장 가까운 피격 대상 선택
	// - 피격 시 HP 감소
	// 반환: RayHitResult
	RayHitResult ProcessFire(Player* shooter);

private:
	CRITICAL_SECTION _cs;
	vector<Player*> PlayerManager;

	POINT OldCursorPos = { 500,500 };
};

