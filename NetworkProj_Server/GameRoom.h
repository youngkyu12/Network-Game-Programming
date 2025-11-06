#pragma once
#include "pch.h"

class GameRoom
{
public:
	GameRoom();
	~GameRoom();

	void Update_State();
	void Add_Player();

private:
	CRITICAL_SECTION _cs;
};

