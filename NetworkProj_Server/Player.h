#pragma once
#include "pch.h"


class Player
{
public:
	Player();
	~Player();
public:
	int Player_ID;
	SOCKET sock = INVALID_SOCKET;

	XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3 Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);

	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;

	float Friction = 125.0f;

	int HP;
	bool Fire_Flag;

public:
	void SetPosition(float x, float y, float z);

	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(float fTimeElapsed = 0.016f);
};

