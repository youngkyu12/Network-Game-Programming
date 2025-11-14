#pragma once
#include "pch.h"

enum
{
	MOVE = 1,


};


struct Packetheader
{
	uint16_t size;
	uint16_t ID; // 프로토콜 ID

};

struct Move
{
	float x;
	float y;
	float z;
};