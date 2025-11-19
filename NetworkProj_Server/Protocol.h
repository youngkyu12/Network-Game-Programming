#pragma once
#include "pch.h"
#define MAX_PLAYERS 3

#pragma pack(push, 1)
enum
{
	START = 0,
	MOVE = 1,
	TEMP = 2,
	UPDATE = 3,

};

struct Packetheader
{
	uint16_t size;
	uint16_t ID; // 프로토콜 ID

};

struct MovePacket
{
	Packetheader header;
	uint16_t keyW;
	uint16_t keyS;
	uint32_t mouseX;
	uint32_t mouseY;
};

struct PlayerState // 한명분 데이터
{
	int32_t playerID;
	float x;
	float y;
	float z;
	//uint16_t hp;
};


struct UpdateState
{
	Packetheader header;
	int32_t numPlayers;
	PlayerState players[MAX_PLAYERS];
};


// 테스트

struct PlayerMoveState	// 움직임만을 위한 정보
{
	int32_t playerID;
	float x;
	float y;
	float z;
};

struct UpdateMoveState	// 움직임만을 위한 패킷
{
	Packetheader header;
	PlayerMoveState player;
};

struct StartPlayerState	// 플레이어 접속 했을 때 보내는 정보 (시작위치, 방향)
{
	int32_t playerID;
	float x;
	float y;
	float z;
	float LookX;
	float LookY;
	float LookZ;
};

struct UpdatePlayerState
{
	Packetheader header;
	StartPlayerState player;
};

#pragma pack(pop)