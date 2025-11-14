#pragma once
#include "pch.h"

#pragma pack(push, 1)
enum
{
	MOVE = 1,
	TEMP = 2,

};


struct Packetheader
{
	uint16_t size;
	uint16_t ID; // 프로토콜 ID

};

struct MovePacket
{
	Packetheader header;
	float x;
	float y;
	float z;
};

#pragma pack(pop)