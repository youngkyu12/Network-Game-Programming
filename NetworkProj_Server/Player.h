#pragma once
#include "pch.h"

const int32_t BUF_SIZE = 1024;

class Timer
{
public:
	Timer();
	~Timer();
	void Start()
	{
		isActive = true;
		startTime = GetTickCount();
	}

	void Stop()
	{
		isActive = false;
		startTime = 0;
	}

	bool IsElapsed(DWORD ms)
	{
		if (!isActive)
		{
			return false;
		}

		ULONGLONG current = GetTickCount();
		if (current - startTime >= ms)
		{
			return true;
		}
		return false;
	}

private:
	ULONGLONG startTime;
	bool isActive;
};

/*
데이터는 private 영역에서 관리하는게 좋을꺼 같습니다.
*/
class Player
{
public:
	Player();
	~Player();

	//-----------------------------------
	int Player_ID = 0;
	SOCKET sock = INVALID_SOCKET;

	BYTE recvBuffer[BUF_SIZE] = {};
	int32_t recvByte = 0;

	BYTE sendBuffer[BUF_SIZE] = {};
	int32_t sendByte = 0;
	//-----------------------------------

public:
	void SetPosition(float x, float y, float z);
	void SetLook(float x, float y, float z);
	void SetHP(uint16_t value) { HP = value; }

	XMFLOAT3 GetPosition(); // 포지션 데이터 가져오기
	XMFLOAT3 GetLook();
	uint16_t GetHP(); // HP데이터 가져오기
	uint8_t GetFireFlag();
	uint8_t GetShieldFlag() { return ShieldOn; }
	uint8_t GetCooldownFlag() { return cooldown; }
	uint8_t GetMyState();
	void SetMyState(uint8_t newState);
	void CheckFireFlag();

	void CheckShieldFlag();

	void UpdateBoundingBox();

	void Move(XMFLOAT3& xmf3Shift);
	void Fire();

	void Shield();

	const BoundingOrientedBox& GetBoundingBox() const { return m_xmOOBB; }

	bool HasBoundingBox () const { return m_hasOOBB; }

	void SetEmptyBoundingBox ();

	// 다시 활성화할 때 사용할 API 예시
	void SetBoundingBox ( const XMFLOAT3& center , const XMFLOAT3& extents , const XMFLOAT4& orientation );


	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void LookAt(XMFLOAT3& Look, XMFLOAT3& Up);

	int check = 0;

private:
	uint16_t HP = 100;

	/* 행렬로 대체
	XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	*/

	// World Matrix와 OOBB 충돌체
	// 충돌검사 일단 Move에서만 수행하겠습니다.
	XMFLOAT4X4	m_xmf4x4World = Matrix4x4::Identity();
	BoundingOrientedBox	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.0, 3.0, 3.0), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//
	uint8_t MyState = None;

	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;
	float Friction = 125.0f;

	uint8_t is_Firing = 0;// false = 0 true = 1
	uint8_t ShieldOn = 0; // false = 0 true = 1
	uint8_t cooldown = 0; // 쿨타임 여부 false = 0 true = 1
	Timer FireTimer;
	Timer ShieldTimer;
	Timer CooldownTimer;

	bool m_hasOOBB = true; // 충돌체 활성화 여부
};

