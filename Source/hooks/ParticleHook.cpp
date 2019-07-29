#include "ParticleHook.h"
#include "plugin.h"
#include "CDraw.h"

using namespace plugin;

bool ParticleHook::MySprite_CalcScreenCoors(CVector& posn, CVector& out, float& w, float& h, bool checkMaxVisible)
	{
		float InverseZ;
		out = *(CMatrix*)0x7095F0 * posn;
		if (out.z <= 0.2f || out.z >= CDraw::ms_fFarClipZ && checkMaxVisible)
			return false;
		InverseZ = 1.0f / out.z;
		out.x = RsGlobal.screenWidth * InverseZ * out.x;
		out.y = RsGlobal.screenHeight * InverseZ * out.y;
		w = RsGlobal.screenWidth * InverseZ;
		h = RsGlobal.screenHeight * InverseZ;
		w = w / CDraw::ms_fFOV * 70.0f;
		h = h / CDraw::ms_fFOV * 70.0f;
		return true;
	}

void ParticleHook::InstallHook()
{
	patch::RedirectJump(by_version<0x51C3A0, 0x51C5D0>(), MySprite_CalcScreenCoors);
}
