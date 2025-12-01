#pragma once
#include "pch.h"
#define MAX_PLAYERS 10

#pragma pack(push, 1)
enum
{
	MOVE = 1,
	FIRE = 2,
	TEMP = 3,
	UPDATE = 4,

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
	float yaw;	// rotate에 마우스 커서 말고 yaw 값 받아옴
	uint8_t FireFlag = 0; // false = 0 true = 1
};

struct PlayerState // 한명분 데이터
{
	int32_t playerID;
	int32_t hp;
	float x;
	float y;
	float z;
	float Look_x;
	float Look_y;
	float Look_z;
	uint8_t FireFlag;
};


struct UpdateState
{
	Packetheader header;
	uint8_t My_ID;
	int32_t numPlayers;
	PlayerState players[MAX_PLAYERS];
};

#pragma pack(pop)