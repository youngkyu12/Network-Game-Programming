#include "pch.h"

enum
{
	Move = 1,


};


struct Packetheader
{
	uint16_t size;
	uint16_t type;

};

struct Move
{
	float x;
	float y;
	float z;
};