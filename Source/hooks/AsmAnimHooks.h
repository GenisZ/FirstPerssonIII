#pragma once

class AsmAnimHooks {
public:
	static unsigned int ReturnJmps[7];

	static void ReloadHook();
	static void FinishedAttackHook();
	static void FinishedAttack2Hook();
	static void BatHitSound();
};