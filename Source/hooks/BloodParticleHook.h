#pragma once

#include "CEntity.h"

class BloodParticleHook
{
	static bool __fastcall CheckIsPlayer(CEntity* entity);
public:
	static void InstallHook();
};