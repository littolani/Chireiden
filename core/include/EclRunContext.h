#pragma once
#include "Chireiden.h"
#include "EclInstruction.h"
#include "EclStack.h"

class Enemy;
struct EclRunContext
{
	float time;
	EclInstruction* currentInstruction;
	EclStack stack;
	int asyncId;
	Enemy* enemy;
	int setByIns20;
	uint8_t difficultyMask;
	int flags_setByIns1819;

	static void run(EclRunContext* This, float gameSpeed);
};