#include "FOVhook.h"
#include "../CFirstPersonHandler.h"
#include "plugin.h"

using namespace plugin;

unsigned int FOVhook::AdressFOV;

void FOVhook::SetFOV(float FOV)
{
	if (CFirstPersonHandler::IsEnable)
		FOV = CFirstPersonHandler::CurrentZoom;

	((void(__cdecl*)(float))AdressFOV)(FOV); //CDraw::SetFOV(float)
}

void FOVhook::InstallHook()
{
	AdressFOV = by_version<0x46E92E, 0x46E90E>() + 0x5 + patch::GetInt(by_version<0x46E92E, 0x46E90E>() + 0x1);

	patch::RedirectCall(by_version<0x46E92E, 0x46E90E>(), SetFOV);
	patch::RedirectCall(by_version<0x46EACE, 0x46EAAE>(), SetFOV);
	patch::RedirectCall(by_version<0x48C0DA, 0x48C1CA>(), SetFOV);
	patch::RedirectCall(by_version<0x48C50B, 0x48C60B>(), SetFOV);
}
