#include "AnimHooks.h"
#include "AsmAnimHooks.h"
#include "../FPAnimManager.h"
#include "../CFirstPersonHandler.h"
#include "../structs/CAnimManager.h"
#include "../eAnims.h"
#include "CWeaponInfo.h"
#include "eAnimations.h"
#include "plugin.h"
#include <fstream>

using namespace plugin;

__int8 AnimHooks::AnimHierEx[300*0x28];

unsigned int* AnimHooks::FightMoves[2]
{ 
	(unsigned int*)by_version<0x5F985C, 0x5F9644>(), (unsigned int*)by_version<0x5F98BC, 0x5F96A4>()
};

AnimHooks::stWeaponAnim AnimHooks::WeaponsAnimsSet[13];
AnimHooks::stWeaponAnim AnimHooks::OriginalWeaponsAnimsSet[13];

void AnimHooks::InstallHooks()
{
	CdeclEvent <AddressListMulti<0x48C19C, GAME_10EN, H_CALL,
		0x48C28C, GAME_11EN, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> LoadIFPEvent;
	LoadIFPEvent.Add(FPAnimManager::LoadMyIFP); patch::SetUInt(0x403911, 0xD8); patch::SetUChar(0x403903, 0x1A);
	
	patch::RedirectCall(by_version<0x4D08F1, 0x4D0991>(), ArmedAnimRemove);
	patch::RedirectCall(by_version<0x4C6874, 0x4C6914>(), ArmedAnimRemove);
	patch::RedirectCall(by_version<0x4E68EF, 0x4E699F>(), GrenadeAnimHook);
	patch::RedirectCall(by_version<0x5939CE, 0x593C7E>(), GetPartialAnimHook);
	patch::RedirectCall(by_version<0x593E05, 0x5940B5>(), GetPartialAnimHook_N);
	patch::RedirectCall(by_version<0x4E6CF3, 0x4E6DA3>(), GrenadeThrowU);
	patch::SetPointer(0x4033F2, DestroyAnimsHook);

	patch::RedirectCall(by_version<0x4E6746, 0x4E67F6>(), BlendAnimCallHook); patch::RedirectCall(by_version<0x4E7465, 0x4E7515>(), BlendAnimCallHook);
	patch::RedirectCall(by_version<0x4E6FDC, 0x4E708C>(), BlendAnimCallHook); patch::RedirectCall(by_version<0x4E72D4, 0x4E7384>(), BlendAnimCallHook);
	patch::RedirectCall(by_version<0x4E72A1, 0x4E7351>(), BlendAnimCallHook); patch::RedirectCall(by_version<0x4E6405, 0x4E64B5>(), BlendAnimCallHook);
	patch::RedirectCall(by_version<0x4E772F, 0x4E77DF>(), BlendAnimCallHook); patch::RedirectCall(by_version<0x4E8A4D, 0x4E8AFD>(), BlendAnimCallHook);
	patch::RedirectCall(by_version<0x4E607A, 0x4E612A>(), AddAnimCallHook);

	patch::SetPointer(0x4011A3, AnimHierEx); patch::SetPointer(0x4033C8, AnimHierEx);
	patch::SetPointer(0x403509, AnimHierEx); patch::SetPointer(0x403C7A, AnimHierEx);
	patch::SetPointer(0x40434D, AnimHierEx); patch::SetPointer(0x4044B2, AnimHierEx);
	patch::SetPointer(0x40451D, AnimHierEx);

	patch::RedirectJump(by_version<0x4E6C20, 0x4E6CD0>(), AsmAnimHooks::ReloadHook);
	patch::RedirectJump(by_version<0x4E68AE, 0x4E695E>(), AsmAnimHooks::FinishedAttackHook);
	patch::RedirectJump(by_version<0x4E693C, 0x4E69EC>(), AsmAnimHooks::FinishedAttack2Hook);
	patch::RedirectJump(by_version<0x4E6F55, 0x4E7005>(), AsmAnimHooks::BatHitSound);

	ThiscallEvent <AddressListMulti<0x4EFD68, GAME_10EN, H_CALL, 0x4F01E6, GAME_10EN, H_CALL,
		0x4F03A2, GAME_10EN, H_CALL, 0x4F1731, GAME_10EN, H_CALL, 0x4F1BFF, GAME_10EN, H_CALL,
		0x4EFE18, GAME_11EN, H_CALL, 0x4F0296, GAME_11EN, H_CALL, 0x4F0452, GAME_11EN, H_CALL,
		0x4F17E1, GAME_11EN, H_CALL, 0x4F1CAF, GAME_11EN, H_CALL>,
		PRIORITY_AFTER, ArgPickNone, void(void*)> RealMoveAnim;
	RealMoveAnim.Add(MoveAnimManager);

	ThiscallEvent <AddressListMulti<0x5FA520, GAME_10EN, H_CALLBACK,
		0x5FA308, GAME_11EN, H_CALLBACK>, PRIORITY_BEFORE, ArgPickNone, void(void*)> PlayerControlProcess;
	PlayerControlProcess.before.Add(SetWeaponAnimsSettins);
	PlayerControlProcess.after.Add(ReturnWeaponAnimsSettins);
}

CAnimBlendAssociation* AnimHooks::ArmedAnimRemove(RpClump * clump, unsigned int index, unsigned int ID, float clumpAssocBlendData)
{
	if (CFirstPersonHandler::IsEnable && clump == FindPlayerPed()->m_pRwClump) return nullptr;
	else return CAnimManager::BlendAnimation(clump, index, ID, clumpAssocBlendData);
}

void AnimHooks::ApplyMoveAnim(unsigned int AnimID, RpClump* clump, CAnimBlendAssociation* mainAnim,
	CAnimBlendAssociation* partialAnim, CAnimBlendAssociation* fpAnim, unsigned int AnimIDs)
{
	auto block = FindPlayerPed()->m_nAnimGroupId;

	if (AnimIDs && mainAnim->m_nAnimID == ANIM_PLAYER_SPRINT_CIVI && !partialAnim)
	{ 
		if (block != ANIM_GROUP_PLAYERROCKET && block != ANIM_GROUP_PLAYER2ARMED
			&& block != ANIM_GROUP_PLAYERBBBAT && block < ANIM_GROUP_PLAYERBACK)
		{
			if (fpAnim)
			{
				if (fpAnim->m_nAnimID != AnimIDs && fpAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
				{
					fpAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
					fpAnim->m_fBlendDelta = -1000.0f;
					CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, AnimIDs);
				}
			}
			else
				CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, AnimIDs);
			return;
		}
		else
		{
			if (fpAnim)
			{ 
				if (fpAnim->m_nAnimID == AnimIDs && fpAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
				{ 
					fpAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
					fpAnim->m_fBlendDelta = -1000.0f;
				}
			}
		}
	}
	if (!partialAnim)
	{
		if (fpAnim)
		{
			if (fpAnim->m_nAnimID != AnimID && fpAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
			{
				fpAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
				fpAnim->m_fBlendDelta = -1000.0f;
				CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, AnimID);
			}
		}
		else
			CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, AnimID);
	}

}

CAnimBlendAssociation * AnimHooks::GetPartialAnimHook(RpClump * clump)
{
	return FPAnimManager::GetPartialAssotiation(clump, false);
}

CAnimBlendAssociation * AnimHooks::GetPartialAnimHook_N(RpClump * clump, unsigned int Number)
{
	void* link = **(void***)((unsigned int)clump + 0x3C);
	CAnimBlendAssociation* assoc = nullptr, *anim; int n = 0;

	while (link)
	{
		anim = (CAnimBlendAssociation*)((unsigned int)link - 4);

		if (anim->m_nFlags & ANIMATION_PARTIAL && anim->m_nAnimID < 200 && n == Number)
		{ 
			assoc = anim; break;
		}

		link = *(void**)link; n++;
	}

	return assoc;
}

CAnimBlendAssociation * AnimHooks::GrenadeThrowU(RpClump * clump, unsigned int Number)
{
	if (CFirstPersonHandler::IsEnable && clump == FindPlayerPed()->m_pRwClump)
		return RpAnimBlendClumpGetAssociation(clump, FP_ANIM_FIRE_THROWU);
	else
		return RpAnimBlendClumpGetAssociation(clump, ANIM_MAN_WEAPON_THROWU);
}

void __fastcall AnimHooks::DestroyAnimsHook(CAnimBlendAssocGroup * AnimGroup)
{
	if (AnimGroup != &CAnimManager::ms_aAnimAssocGroups[25])
	{
		CallMethod<0x4012D0, CAnimBlendAssocGroup*>(AnimGroup);//CAnimBlendAssocGroup::DestroyAssociations
		return;
	}

	for (int i = 200; i < AnimGroup->m_nNumAnimations; i++)
		CallMethod<0x401520, CAnimBlendAssociation*>(&(((CAnimBlendAssociation*)AnimGroup->m_pAssociations)[i]));//CAnimBlendAssociation destructor

	delete[] (char*)((unsigned int)AnimGroup->m_pAssociations - 8);
}

CAnimBlendAssociation * AnimHooks::GrenadeAnimHook(RpClump * clump, unsigned int index, unsigned int ID)
{
	if (CFirstPersonHandler::IsEnable && FindPlayerPed()->m_pRwClump == clump)
	{
		if (ID == ANIM_MAN_WEAPON_THROW) return CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, FP_ANIM_FIRE_THROW);
		else return CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, FP_ANIM_FIRE_THROWU);
	}

	return CAnimManager::AddAnimation(clump, index, ID);
}

CAnimBlendAssociation* AnimHooks::AddAnimCallHook(RpClump * clump, unsigned int index, unsigned int ID)
{
	if (ID >= 200)
	{
		CAnimBlendAssociation* FPAnim = FPAnimManager::GetPartialAssotiation(clump, true);
		if (FPAnim && FPAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
		{
			FPAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
			FPAnim->m_fBlendDelta = -1000.0f;
		}
		return CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, ID);
	}
	else return CAnimManager::AddAnimation(clump, index, ID);
}

CAnimBlendAssociation* AnimHooks::BlendAnimCallHook(RpClump * clump, unsigned int index, unsigned int ID, float clumpAssocBlendData)
{
	if (ID >= 200)
	{
		CAnimBlendAssociation* FPAnim = FPAnimManager::GetPartialAssotiation(clump, true);
		if (FPAnim && FPAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
		{
			FPAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
			FPAnim->m_fBlendDelta = -1000.0f;
		}
		return CAnimManager::AddAnimation(clump, FP_ANIM_GROUP, ID);
	}
	else return CAnimManager::BlendAnimation(clump, index, ID, clumpAssocBlendData);
}

void AnimHooks::MoveAnimManager()
{
	if (!CFirstPersonHandler::IsEnable) return;

	auto player = FindPlayerPed(); if (!player) return;
	auto clump = player->m_pRwClump; CWeapon Weapon = player->m_aWeapons[player->m_bWeaponSlot];
	
	CAnimBlendAssociation* MainAnim = FPAnimManager::GetMainAssotiation(clump);
	CAnimBlendAssociation* PartialAnim = FPAnimManager::GetPartialAssotiation(clump, false);
	CAnimBlendAssociation* FPAnim = FPAnimManager::GetPartialAssotiation(clump, true);

	if (FPAnim && FPAnim->m_nAnimID >= FP_ANIM_FIRE_PISTOL && FPAnim->m_fBlendDelta >= -8.0f && FPAnim->m_fBlendDelta < 0.0f)
	{ 
		FPAnim->m_fBlendDelta = -1000.0f;
		FPAnim = nullptr;
	}

	switch (Weapon.m_nType) {
	case WEAPONTYPE_BASEBALLBAT:
		ApplyMoveAnim(FP_ANIM_IDLE_BAT, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_COLT45:
		ApplyMoveAnim(FP_ANIM_IDLE_PISTOL, clump, MainAnim, PartialAnim, FPAnim, FP_ANIM_SPRINT_PISTOL);
		break;
	case WEAPONTYPE_UZI:
		ApplyMoveAnim(FP_ANIM_IDLE_UZI, clump, MainAnim, PartialAnim, FPAnim, FP_ANIM_SPRINT_UZI);
		break;
	case WEAPONTYPE_SHOTGUN:
		ApplyMoveAnim(FP_ANIM_IDLE_SHOTGUN, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_AK47:
		ApplyMoveAnim(FP_ANIM_IDLE_AK, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_M16:
		ApplyMoveAnim(FP_ANIM_IDLE_M16, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_SNIPERRIFLE:
		ApplyMoveAnim(FP_ANIM_IDLE_SNIPER, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_ROCKETLAUNCHER:
		ApplyMoveAnim(FP_ANIM_IDLE_ROCKET, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_FLAMETHROWER:
		ApplyMoveAnim(FP_ANIM_IDLE_FLAME, clump, MainAnim, PartialAnim, FPAnim);
		break;
	case WEAPONTYPE_GRENADE: case WEAPONTYPE_MOLOTOV: case WEAPONTYPE_DETONATOR:
		ApplyMoveAnim(FP_ANIM_IDLE_GRENADE, clump, MainAnim, PartialAnim, FPAnim, FP_ANIM_SPRINT_GRENADE);
		break;
	default:
		if (FPAnim)
			if (FPAnim->m_nAnimID < FP_ANIM_FIRE_PISTOL)
			{ 
				FPAnim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
				FPAnim->m_fBlendDelta = -1000.0f;
			}
		break;
	}
}

void AnimHooks::SetWeaponAnimsSettins()
{
	for (int i = 0; i < 13; i++)
	{
		OriginalWeaponsAnimsSet[i].Anim = aWeaponInfo[i].m_AnimToPlay;
		OriginalWeaponsAnimsSet[i].Anim2 = aWeaponInfo[i].m_Anim2ToPlay;
		OriginalWeaponsAnimsSet[i].StartLoop = aWeaponInfo[i].m_fAnimLoopStart;
		OriginalWeaponsAnimsSet[i].EndLoop = aWeaponInfo[i].m_fAnimLoopEnd;
		OriginalWeaponsAnimsSet[i].FireFrame = aWeaponInfo[i].m_fAnimFrameFire;
	}

	if (CFirstPersonHandler::IsEnable)
	{
		for (auto i = 0; i < 13; i++)
		{
			if (i == WEAPONTYPE_ROCKETLAUNCHER || i == WEAPONTYPE_SNIPERRIFLE) continue;
			aWeaponInfo[i].m_AnimToPlay = WeaponsAnimsSet[i].Anim;
			aWeaponInfo[i].m_fAnimLoopStart = WeaponsAnimsSet[i].StartLoop;
			aWeaponInfo[i].m_fAnimLoopEnd = WeaponsAnimsSet[i].EndLoop;
			aWeaponInfo[i].m_fAnimFrameFire = WeaponsAnimsSet[i].FireFrame;
		}

		aWeaponInfo[WEAPONTYPE_BASEBALLBAT].m_Anim2ToPlay = WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].Anim2;
		aWeaponInfo[WEAPONTYPE_GRENADE].m_Anim2ToPlay = WeaponsAnimsSet[WEAPONTYPE_GRENADE].Anim2;
		aWeaponInfo[WEAPONTYPE_MOLOTOV].m_Anim2ToPlay = WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].Anim2;

		*FightMoves[0] = FP_ANIM_PUNCH;
		*FightMoves[1] = FP_ANIM_PUNCH_LEFT;
	}
}

void AnimHooks::ReturnWeaponAnimsSettins()
{
	for (int i = 0; i < 13; i++)
	{
		aWeaponInfo[i].m_AnimToPlay = OriginalWeaponsAnimsSet[i].Anim;
		aWeaponInfo[i].m_Anim2ToPlay = OriginalWeaponsAnimsSet[i].Anim2;
		aWeaponInfo[i].m_fAnimLoopStart = OriginalWeaponsAnimsSet[i].StartLoop;
		aWeaponInfo[i].m_fAnimLoopEnd = OriginalWeaponsAnimsSet[i].EndLoop;
		aWeaponInfo[i].m_fAnimFrameFire = OriginalWeaponsAnimsSet[i].FireFrame;
	}
	*FightMoves[0] = ANIM_MAN_PUNCHR;
	*FightMoves[1] = ANIM_MAN_FIGHTHEAD;
}

unsigned int AnimHooks::ReloadAnimHook(unsigned int AnimID)
{
	switch (AnimID) {
	case ANIM_MAN_WEAPON_HGUN_BODY: return ANIM_MAN_WEAPON_HGUN_RLOAD;
	case ANIM_MAN_WEAPON_AK_BODY: return ANIM_MAN_WEAPON_AK_RLOAD;
	case FP_ANIM_FIRE_PISTOL: return FP_ANIM_RELOAD_PISTOL;
	case FP_ANIM_FIRE_UZI: return FP_ANIM_RELOAD_UZI;
	case FP_ANIM_FIRE_AK: return FP_ANIM_RELOAD_AK;
	case FP_ANIM_FIRE_M16: return FP_ANIM_RELOAD_M16;
	default: return 173;
	}
}

void AnimHooks::InitWeaponsAnimsSettings()
{
	WeaponsAnimsSet[WEAPONTYPE_UNARMED].Anim = FP_ANIM_PUNCH_RUN;
	WeaponsAnimsSet[WEAPONTYPE_UNARMED].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_UNARMED].FireFrame = 12 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_UNARMED].EndLoop = 99 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].Anim = FP_ANIM_BAT_HIT;
	WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].Anim2 = FP_ANIM_BAT_HIT_FLOOR;
	WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].StartLoop = 5 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].FireFrame = 14 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_BASEBALLBAT].EndLoop = 20 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_COLT45].Anim = FP_ANIM_FIRE_PISTOL;
	WeaponsAnimsSet[WEAPONTYPE_COLT45].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_COLT45].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_COLT45].EndLoop = 7 * (0.1f/3.0f);

	WeaponsAnimsSet[WEAPONTYPE_UZI].Anim = FP_ANIM_FIRE_UZI;
	WeaponsAnimsSet[WEAPONTYPE_UZI].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_UZI].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_UZI].EndLoop = 3 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_SHOTGUN].Anim = FP_ANIM_FIRE_SHOTGUN;
	WeaponsAnimsSet[WEAPONTYPE_SHOTGUN].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_SHOTGUN].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_SHOTGUN].EndLoop = 36 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_AK47].Anim = FP_ANIM_FIRE_AK;
	WeaponsAnimsSet[WEAPONTYPE_AK47].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_AK47].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_AK47].EndLoop = 2 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_M16].Anim = FP_ANIM_FIRE_M16;
	WeaponsAnimsSet[WEAPONTYPE_M16].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_M16].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_M16].EndLoop = 1 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_FLAMETHROWER].Anim = FP_ANIM_FIRE_FLAME;
	WeaponsAnimsSet[WEAPONTYPE_FLAMETHROWER].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_FLAMETHROWER].FireFrame = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_FLAMETHROWER].EndLoop = 1 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].Anim = FP_ANIM_GRENADE_START;
	WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].Anim2 = FP_ANIM_FIRE_THROW;
	WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].FireFrame = 99 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_MOLOTOV].EndLoop = 99 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_GRENADE].Anim = FP_ANIM_GRENADE_START;
	WeaponsAnimsSet[WEAPONTYPE_GRENADE].Anim2 = FP_ANIM_FIRE_THROW;
	WeaponsAnimsSet[WEAPONTYPE_GRENADE].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_GRENADE].FireFrame = 99 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_GRENADE].EndLoop = 99 * (0.1f / 3.0f);

	WeaponsAnimsSet[WEAPONTYPE_DETONATOR].Anim = FP_ANIM_DETONATOR;
	WeaponsAnimsSet[WEAPONTYPE_DETONATOR].StartLoop = 0.0f;
	WeaponsAnimsSet[WEAPONTYPE_DETONATOR].FireFrame = 12 * (0.1f / 3.0f);
	WeaponsAnimsSet[WEAPONTYPE_DETONATOR].EndLoop = 20 * (0.1f / 3.0f);

	std::ifstream wdat("fp_weapons.dat");

	if (wdat)
	{
		std::string buff; int val[3];

		while (true)
		{
			wdat >> buff;
			if (buff[0] == '#') std::getline(wdat, buff);
			else break;
		}
		
		for (int i : {0, 1, 2, 3, 4, 5, 6, 9, 10, 11, 12})
		{
			wdat >> val[0] >> val[1] >> val[2];
			WeaponsAnimsSet[i].StartLoop = val[0] * (0.1f / 3.0f);
			WeaponsAnimsSet[i].FireFrame = val[2] * (0.1f / 3.0f);
			WeaponsAnimsSet[i].EndLoop = val[1] * (0.1f / 3.0f);
			wdat >> buff;
		}

		wdat.close();
	}
}