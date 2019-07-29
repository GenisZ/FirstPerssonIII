#include "BloodParticleHook.h"
#include "../CFirstPersonHandler.h"
#include "plugin.h"

using namespace plugin;

bool __fastcall BloodParticleHook::CheckIsPlayer(CEntity* entity)
{
	if (CFirstPersonHandler::IsEnable && entity == FindPlayerPed())
		return false;
	else
		return entity->GetIsOnScreen();
}

void BloodParticleHook::InstallHook()
{
	patch::RedirectCall(by_version<0x55FE66, 0x55FF96>(), CheckIsPlayer);
	patch::RedirectCall(by_version<0x55FDB4, 0x55FEE4>(), CheckIsPlayer);
	patch::RedirectCall(by_version<0x5611DB, 0x56130B>(), CheckIsPlayer);
	patch::RedirectCall(by_version<0x563D23, 0x563E53>(), CheckIsPlayer);
}
