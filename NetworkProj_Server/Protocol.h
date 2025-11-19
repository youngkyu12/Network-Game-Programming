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
	float yaw;	// rotate에 마우스 커서 말고 yaw 값 받아옴
};

struct PlayerState // 한명분 데이터
{
	int32_t playerID;
	float x;
	float y;
	float z;
	float Look_x;
	float Look_y;
	float Look_z;
	//uint16_t hp;
};


struct UpdateState
{
	Packetheader header;
	int32_t numPlayers;
	PlayerState players[MAX_PLAYERS];
};

#pragma pack(pop)