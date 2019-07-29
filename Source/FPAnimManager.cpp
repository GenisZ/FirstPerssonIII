#include "FPAnimManager.h"
#include "eAnims.h"
#include "structs/CAnimManager.h"
#include "CModelInfo.h"
#include "plugin.h"

using namespace plugin;

void FPAnimManager::LoadMyIFP()
{
	CAnimManager::LoadAnimFile("ANIM\\FPS.IFP");

	auto PrepareClump = CallMethodAndReturnDyn<RpClump*, CBaseModelInfo*>
		(by_version<0x4F8920, 0x4F8A00>(), CModelInfo::GetModelInfo(1)); //CClumpModelInfo::CreateInstance()
	RpAnimBlendClumpInit(PrepareClump);
	CreateMyAssociations(&CAnimManager::ms_aAnimAssocGroups[FP_ANIM_GROUP], "fps", PrepareClump, aNames, 32);
	RpClumpDestroy(PrepareClump);
}

void FPAnimManager::CreateMyAssociations(CAnimBlendAssocGroup* AssocGroup, char* blockName,
	RpClump* clump, char** AnimNames, int AnimNums)
{
	AnimNums += 200;
	char* Mem = new char[64 * AnimNums + 8];
	AssocGroup->m_pAssociations = CallAndReturnDyn<CAnimBlendStaticAssociation*, char*, int, int, int, int>
		(by_version<0x59CCB0, 0x59CF70>(), Mem, 0x401460, 0x401520, 64, AnimNums); //construct_new_array
	AssocGroup->m_nNumAnimations = AnimNums;

	auto anBlock = CAnimManager::GetAnimationBlock(blockName);

	for (int i = 200; i < AnimNums; i++)
	{
		auto animH = CAnimManager::GetAnimation(AnimNames[i - 200], anBlock);
		((CAnimBlendAssociation*)AssocGroup->m_pAssociations)[i].Init(clump, animH);
		((CAnimBlendAssociation*)AssocGroup->m_pAssociations)[i].m_nAnimID = i;
		
		if (i <= FP_ANIM_SPRINT_GRENADE)
			((CAnimBlendAssociation*)AssocGroup->m_pAssociations)[i].m_nFlags = ANIMATION_PARTIAL | ANIMATION_LOOPED | ANIMATION_FLAG200;
		else
			((CAnimBlendAssociation*)AssocGroup->m_pAssociations)[i].m_nFlags = ANIMATION_PARTIAL | ANIMATION_FLAG200 | ANIMATION_FLAG8;
	}
}

void FPAnimManager::RemoveFPAnims()
{
	auto anim = GetPartialAssotiation(FindPlayerPed()->m_pRwClump, true);
	
	if (anim)
	{
		anim->m_fBlendDelta = -1000.0f;
		anim->m_nFlags |= ANIMATION_HASPARTIALBLEND;
	}
}

CAnimBlendAssociation * FPAnimManager::GetMainAssotiation(RpClump * clump)
{
	void* link = **(void***)((unsigned int)clump + 0x3C);
	CAnimBlendAssociation* assoc = nullptr, *anim; float blendamount = 0.0f;

	while (link)
	{
		anim = (CAnimBlendAssociation*)((unsigned int)link - 4);

		if (anim->m_fBlendAmount > blendamount && !(anim->m_nFlags & ANIMATION_PARTIAL))
		{
			blendamount = anim->m_fBlendAmount;
			assoc = anim;
		}

		link = *(void**)link;
	}

	return assoc;
}

CAnimBlendAssociation * FPAnimManager::GetPartialAssotiation(RpClump * clump, bool fpanim)
{
	void* link = **(void***)((unsigned int)clump + 0x3C);
	CAnimBlendAssociation* assoc = nullptr, *anim; float amount = 0.0f;

	while (link)
	{
		anim = (CAnimBlendAssociation*)((unsigned int)link - 4);

		if (anim->m_fBlendAmount > amount && anim->m_nFlags & ANIMATION_PARTIAL)
			if (!fpanim && anim->m_nAnimID < 200
				|| fpanim && anim->m_nAnimID >= 200)
			{ 
				amount = anim->m_fBlendAmount;
				assoc = anim;
			}

		link = *(void**)link;
	}

	return assoc;
}

char* FPAnimManager::aNames[] = {
	"idle_pistol", "idle_uzi", "idle_shotgun", "idle_ak", "idle_m16", "idle_sniper",
	"idle_rpg", "idle_flame", "idle_grenade", "idle_bat", "sprint_pistol", "sprint_uzi",
	"sprint_grenade", "fire_pistol", "reload_pistol", "fire_uzi", "reload_uzi",
	"fire_shotgun", "fire_ak", "reload_ak", "fire_m16", "reload_m16", "fire_flame",
	"grenade_start", "grenade_throw", "grenade_throwu", "detonator", "bat_hit", 
	"bat_hit_floor", "punch_right", "punch_run", "punch_left"
};