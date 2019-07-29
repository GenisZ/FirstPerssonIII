#include "AsmAnimHooks.h"
#include "AnimHooks.h"
#include "../eAnims.h"
#include "eAnimations.h"

unsigned int AsmAnimHooks::ReturnJmps[7]
{ 
	plugin::by_version<0x4E6C3D, 0x4E6CED>(), plugin::by_version<0x4E6910, 0x4E69C0>(),
	plugin::by_version<0x4E68B4, 0x4E6964>(), plugin::by_version<0x4E6976, 0x4E6A26>(),
	plugin::by_version<0x4E6941, 0x4E69F1>(), plugin::by_version<0x4E6F5B, 0x4E700B>(),
	plugin::by_version<0x4E6F70, 0x4E7020>()
};

void __declspec(naked) AsmAnimHooks::ReloadHook()
{
	__asm {
		push	eax
		call	AnimHooks::ReloadAnimHook
		add		esp, 4h
		mov		[esp + 14h], eax
		jmp		ReturnJmps
	}
}

void  __declspec(naked) AsmAnimHooks::FinishedAttackHook()
{
	__asm {
		cmp     dword ptr[ebp + 2Ch], ANIM_MAN_WEAPON_START_THROW
		jz      truejmp
		cmp     dword ptr[ebp + 2Ch], FP_ANIM_GRENADE_START
		jz      truejmp
		jmp		ReturnJmps[4*1]

		truejmp:
		jmp		ReturnJmps[4*2]
	}
}

void  __declspec(naked) AsmAnimHooks::FinishedAttack2Hook()
{
	__asm {
		cmp     eax, ANIM_MAN_WEAPON_THROWU
		jz      truejmp
		cmp     eax, FP_ANIM_FIRE_THROW
		jz      truejmp
		cmp     eax, FP_ANIM_FIRE_THROWU
		jz      truejmp
		jmp		ReturnJmps[4*3]

		truejmp:
		jmp		ReturnJmps[4*4]
	}
}

void __declspec(naked) AsmAnimHooks::BatHitSound()
{
	__asm {
		cmp     eax, FP_ANIM_BAT_HIT
		jz      truejmp
		cmp     eax, FP_ANIM_BAT_HIT_FLOOR
		jz      truejmp

		sub		eax, ANIM_MAN_WEAPON_BAT_H
		cmp     eax, 1
		jmp		ReturnJmps[4 * 5]

		truejmp:
		jmp		ReturnJmps[4 * 6]
	}
}
