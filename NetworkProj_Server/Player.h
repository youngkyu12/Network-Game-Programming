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

	XMFLOAT3 GetPosition(); // 포지션 데이터 가져오기
	XMFLOAT3 GetVelocity();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	uint16_t GetHP(); // HP데이터 가져오기
	uint8_t GetFireFlag();
	void CheckFireFlag();


	void Move(XMFLOAT3& xmf3Shift);
	void Fire();

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void LookAt(XMFLOAT3& Look, XMFLOAT3& Up);

	int check = 0;

private:
	uint16_t HP;

	XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);


	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;
	float Friction = 125.0f;

	uint8_t is_Firing = 0;// false = 0 true = 1
	Timer FireTimer;

};

