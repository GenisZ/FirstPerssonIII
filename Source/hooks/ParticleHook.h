#pragma once

#include "plugin.h"

class ParticleHook {
	static bool MySprite_CalcScreenCoors(CVector& posn, CVector& out, float& w, float& h, bool checkMaxVisible);
public:
	static void InstallHook();
};
