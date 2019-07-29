#pragma once

#include "plugin.h"
#include "CAnimBlendAssocGroup.h"

class AnimHooks {
public:
	static __int8 AnimHierEx[300*0x28];
	static unsigned int* FightMoves[2];

	struct stWeaponAnim{
		unsigned int Anim, Anim2;
		float StartLoop, FireFrame, EndLoop;
	} static WeaponsAnimsSet[13], OriginalWeaponsAnimsSet[13];

	static void InstallHooks();
	static void InitWeaponsAnimsSettings();
	static void MoveAnimManager();
	static void SetWeaponAnimsSettins();
	static void ReturnWeaponAnimsSettins();
	static unsigned int ReloadAnimHook(unsigned int AnimID);
	static CAnimBlendAssociation* ArmedAnimRemove(RpClump *clump, unsigned int index,
	unsigned int ID, float clumpAssocBlendData);
	static void ApplyMoveAnim(unsigned int AnimID, RpClump* clump, CAnimBlendAssociation* mainAnim,
		CAnimBlendAssociation* partialAnim, CAnimBlendAssociation* fpAnim, unsigned int AnimIDs = 0);
	static CAnimBlendAssociation* GetPartialAnimHook(RpClump* clump);
	static CAnimBlendAssociation* GetPartialAnimHook_N(RpClump* clump, unsigned int Number);
	static CAnimBlendAssociation* GrenadeThrowU(RpClump* clump, unsigned int Number);
	static void __fastcall DestroyAnimsHook(CAnimBlendAssocGroup* AnimGroup);
	static CAnimBlendAssociation* GrenadeAnimHook(RpClump * clump, unsigned int index, unsigned int ID);
	static CAnimBlendAssociation* BlendAnimCallHook(RpClump * clump, unsigned int index, unsigned int ID, float clumpAssocBlendData);
	static CAnimBlendAssociation* AddAnimCallHook(RpClump * clump, unsigned int index, unsigned int ID);
};