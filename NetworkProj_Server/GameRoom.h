#pragma once
#include "pch.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Update_State();

private:
	CRITICAL_SECTION _cs;
};

