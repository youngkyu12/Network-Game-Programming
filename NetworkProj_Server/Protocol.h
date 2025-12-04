#pragma once
#include "pch.h"
#define MAX_PLAYERS 3

#pragma pack(push, 1)
enum EGameState
{
	None = 0,
	Ready = 1,
	Playing = 2,
	GameOver = 3,
};

enum
{
	UPDATE = 1,
};

struct Packetheader
{
	uint16_t size;
	uint16_t ID; // 프로토콜 ID
	uint8_t GameState;

};

struct MovePacket
{
	Packetheader header;
	uint8_t keyW;
	uint8_t keyS;
	float yaw;	// rotate에 마우스 커서 말고 yaw 값 받아옴
	uint8_t FireFlag = 0; // false = 0 true = 1
	uint8_t shield = 0;
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
	uint8_t ShieldFlag;
};


struct UpdateState
{
	Packetheader header;
	uint8_t My_ID;
	int32_t numPlayers;
	PlayerState players[MAX_PLAYERS];
};

#pragma pack(pop)