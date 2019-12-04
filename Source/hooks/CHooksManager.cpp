#include "CHooksManager.h"
#include "FOVhook.h"
#include "ParticleHook.h"
#include "BloodParticleHook.h"
#include "AnimHooks.h"
#include "../GUI/GUI.h"
#include "../CFirstPersonHandler.h"
#include "../CBodySystem.h"
#include "../structs/CCamera.h"

using namespace plugin;

namespace MyEvents
{
	ThiscallEvent <AddressListMulti<0x46D5FF, GAME_10EN, H_CALL,
		0x46D5DF, GAME_11EN, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void(void*)> CamProcessEvent;

	ThiscallEvent <AddressListMulti<0x5FA520, GAME_10EN, H_CALLBACK,
		0x5FA308, GAME_11EN, H_CALLBACK>, PRIORITY_BEFORE, ArgPickNone, void(void*)> PlayerProcessControl;

	ThiscallEvent <AddressListMulti<0x5FA534, GAME_10EN, H_CALLBACK,
		0x5FA31C, GAME_11EN, H_CALLBACK>, PRIORITY_BEFORE, ArgPickNone, void(void*)> PlayerPender;

	ThiscallEvent <AddressListMulti<0x5FA524, GAME_10EN, H_CALLBACK,
		0x5FA30C, GAME_11EN, H_CALLBACK>, PRIORITY_BEFORE, ArgPickNone, void(void*)> PlayerCollision;

	CdeclEvent <AddressListMulti<0x48E642, GAME_10EN, H_CALL,
		0x48E702, GAME_11EN, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> Render2DStuff;

	ThiscallEvent <AddressListMulti<0x49272F, GAME_10EN, H_CALL,
		0x4927EF, GAME_11EN, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void*)> UpdateMouse;
}

void CHooksManager::InstallHooks()
{
	MyEvents::CamProcessEvent.Add(CFirstPersonHandler::Process, PRIORITY_AFTER);
	MyEvents::PlayerProcessControl.Add(CFirstPersonHandler::RotatePlayerTorso);
	AnimHooks::InitWeaponsAnimsSettings();
	BloodParticleHook::InstallHook();
	ParticleHook::InstallHook();
	AnimHooks::InstallHooks();
	FOVhook::InstallHook();
	InjectGUI();

	MyEvents::PlayerPender.before += [] {
		if (CFirstPersonHandler::IsEnable)
			CBodySystem::RemoveHead(true);
	};

	MyEvents::PlayerPender.after += [] {
		if (CFirstPersonHandler::IsEnable)
			CBodySystem::RemoveHead(false);
	};
	/////////////////////////////////////////////
	MyEvents::PlayerCollision.before += [] {
		if (CFirstPersonHandler::IsEnable)
			ChangeColl(true);
	};

	MyEvents::PlayerCollision.after += [] {
		if (CFirstPersonHandler::IsEnable)
			ChangeColl(false);
	};
}

void CHooksManager::SetPathes(bool Set)
{
	DisableCamera3rdPersonProcess(Set);
	PatchCrossHair(Set);
	DisableClassicAxis(Set);
	ChangeCode(Set);
}

void CHooksManager::InjectGUI()
{
	MyEvents::Render2DStuff += GUI::Process;
	MyEvents::UpdateMouse += GUI::MouseHook;
	Events::initGameEvent += GUI::Init;
	Events::shutdownPoolsEvent += GUI::ShutDown;
}

void CHooksManager::PatchCrossHair(bool Set)
{
	static float CrossHairX, CrossHairY;

	if (Set)
	{
		CrossHairX = TheCamera.m_fCrosshairX;
		CrossHairY = TheCamera.m_fCrosshairY;

		TheCamera.m_fCrosshairX = 0.5f;
		TheCamera.m_fCrosshairY = 0.5f;
	}
	else
	{
		TheCamera.m_fCrosshairX = CrossHairX;
		TheCamera.m_fCrosshairY = CrossHairY;
	}
}

void CHooksManager::ChangeColl(bool Set)
{
	if (Set)
		CBodySystem::ExtendCol(CBodySystem::GetCollSphereRadius() * 1.6f);
	else
		CBodySystem::ExtendCol(CBodySystem::GetCollSphereRadius() / 1.6f);
}

void CHooksManager::DisableCamera3rdPersonProcess(bool Set)
{
	if (Set)
		patch::RedirectJump(0x459A6B, (void*)0x459D2A);
	else
	{
		patch::SetUChar(0x459A6B, 0x89);
		patch::SetUChar(0x459A6C, 0xD9);
		patch::SetUChar(0x459A6D, 0xFF);
		patch::SetUChar(0x459A6E, 0x74);
		patch::SetUChar(0x459A6F, 0x24);
	}
}

void CHooksManager::DisableClassicAxis(bool Set)
{
	if (!IsGameVersion10en()) return;

	// Player movements.
	int m_dwAddress0[] = { 0x4F2685,
							0x469292,
							0x4C7673,
							0x4F03D5,
							0x4D76CE,
							0x45DB98
	};
	static int OrigAddress0 = 0x4F2685 + 0x5 + patch::GetInt(0x4F2685 + 0x1);

	// Shooting direction.
	int m_dwAddress1[] = { 0x4E6562,
							0x55D88B,
							0x560C25
	};
	static int OrigAddress1 = 0x4E6562 + 0x5 + patch::GetInt(0x4E6562 + 0x1);
	static int OrigAddress2 = 0x46D500 + 0x5 + patch::GetInt(0x46D500 + 0x1);
	static int OrigAddress3 = 0x505EA9 + 0x5 + patch::GetInt(0x505EA9 + 0x1);

	if (Set)
	{
		// CamControl
		patch::RedirectCall(0x46D500, (void*)0x468250);

		for (int i = 0; i < 6; i++)
			patch::RedirectCall(m_dwAddress0[i], (void*)0x457460);

		for (int i = 0; i < 3; i++)
			patch::RedirectCall(m_dwAddress1[i], (void*)0x457460);

		// Crosshair 
		patch::Set<BYTE>(0x50554C + 1, 0xFF);
		patch::Set<BYTE>(0x505627 + 1, 0xFF);
		patch::RedirectCall(0x505EA9, (void*)0x501D90);

		patch::Set<BYTE>(0x468D09, 0x75);
		patch::Set<BYTE>(0x468D92, 0x75);

		// Jump
		patch::Set<BYTE>(0x4D73DC, 0x74);
	}
	else
	{
		// CamControl
		patch::RedirectCall(0x46D500, (void*)OrigAddress2);

		for (int i = 0; i < 6; i++)
			patch::RedirectCall(m_dwAddress0[i], (void*)OrigAddress0);

		for (int i = 0; i < 3; i++)
			patch::RedirectCall(m_dwAddress1[i], (void*)OrigAddress1);

		// Crosshair 
		patch::Set<BYTE>(0x50554C + 1, 0);
		patch::Set<BYTE>(0x505627 + 1, 0);
		patch::RedirectCall(0x505EA9, (void*)OrigAddress3);
	}
}

void CHooksManager::ChangeCode(bool Set)
{
	static float CrossX, CrossY;
	static unsigned int WideScreenFix;

	if (Set)
	{
		//Free Camera Matrix
		patch::RedirectJump(by_version<0x46E715, 0x46E6F5>(), (void*)by_version<0x46E786, 0x46E766>());
		patch::RedirectJump(by_version<0x46E809, 0x46E7E9>(), (void*)by_version<0x46E920, 0x46E900>());

		//Free fixed camera
		patch::RedirectJump(0x459BFA, (void*)0x459D2A);

		//LookBehind
		patch::Nop(by_version<0x493329, 0x4933E9>(), 2);\

		//Near Clip
		patch::SetUChar(by_version<0x5A5070, 0x5A5330>(), 0xC3);

		//M16 Aim
		patch::RedirectJump(by_version<0x4F1F75, 0x4F2025>(), (void*)by_version<0x4F2038, 0x4F20E8>());

		//Radar
		patch::RedirectJump(by_version<0x4A5124, 0x4A5214>(), (void*)by_version<0x4A5207, 0x4A52F7>());
		patch::RedirectJump(by_version<0x4A53D3, 0x4A54C3>(), (void*)by_version<0x4A5520, 0x4A5610>());

		//Particle From Car
		patch::RedirectJump(by_version<0x4A9400, 0x4A94F0>(), (void*)by_version<0x4A9436, 0x4A9526>());

		//Restore Gun Point
		patch::RedirectJump(by_version<0x4C6B39, 0x4C6BD9>(), (void*)by_version<0x4C6B9C, 0x4C6C3C>());

		//Set Target Coords
		CrossX = patch::GetFloat(by_version<0x46BE9B, 0x46BE77>());
		CrossY = patch::GetFloat(by_version<0x46BEA5, 0x46BE81>());

		patch::SetFloat(by_version<0x46BE9B, 0x46BE77>(), 0.5f);
		patch::SetFloat(by_version<0x46BEA5, 0x46BE81>(), 0.5f);

		//WideScreenFix
		WideScreenFix = patch::GetUInt(by_version<0x50550F, 0x5055EF>());
		patch::SetUInt(by_version<0x50550F, 0x5055EF>(), 0x6FAE94);

		//Particle Fix
		patch::Nop(by_version<0x50F0F3, 0x50F2E3>(), 6);
		patch::Nop(by_version<0x50F111, 0x50F301>(), 6);

		//Bullet Impact On the Player Fix
		patch::SetUChar(by_version<0x55FC24, 0x55FD54>(), 0x90);
		patch::SetUChar(by_version<0x55FC25, 0x55FD55>(), 0xE9);

		patch::SetUChar(by_version<0x563C7D, 0x563DAD>(), 0x90);
		patch::SetUChar(by_version<0x563C7E, 0x563DAE>(), 0xE9);
	}
	else
	{
		//Camera Matrix
		patch::SetUChar(by_version<0x46E715, 0x46E6F5>(), 0xD9);
		patch::SetUChar(by_version<0x46E716, 0x46E6F6>(), 0x84);
		patch::SetUChar(by_version<0x46E717, 0x46E6F7>(), 0x24);
		patch::SetUChar(by_version<0x46E718, 0x46E6F8>(), 0x74);
		patch::SetUChar(by_version<0x46E719, 0x46E6F9>(), 0x01);
		
		patch::SetUChar(by_version<0x46E809, 0x46E7E9>(), 0x0F);
		patch::SetUChar(by_version<0x46E80A, 0x46E7EA>(), 0xB7);
		patch::SetUChar(by_version<0x46E80B, 0x46E7EB>(), 0xC0);
		patch::SetUChar(by_version<0x46E80C, 0x46E7EC>(), 0xD9);
		patch::SetUChar(by_version<0x46E80D, 0x46E7ED>(), 0x44);

		//Fixed camera
		patch::SetUChar(0x459BFA, 0x8D);
		patch::SetUChar(0x459BFB, 0x44);
		patch::SetUChar(0x459BFC, 0x24);
		patch::SetUChar(0x459BFD, 0x18);
		patch::SetUChar(0x459BFE, 0x89);

		//LookBehind
		patch::SetUChar(by_version<0x493329, 0x4933E9>(), 0x74);
		patch::SetUChar(by_version<0x49332A, 0x4933EA>(), 0x05);

		//Near Clip
		patch::SetUChar(by_version<0x5A5070, 0x5A5330>(), 0xD9);

		//M16 Aim
		patch::SetUChar(by_version<0x4F1F75, 0x4F2025>(), 0x0F);
		patch::SetUChar(by_version<0x4F1F76, 0x4F2026>(), 0x85);
		patch::SetUChar(by_version<0x4F1F77, 0x4F2027>(), 0xBD);
		patch::SetUChar(by_version<0x4F1F78, 0x4F2028>(), 0x00);
		patch::SetUChar(by_version<0x4F1F79, 0x4F2029>(), 0x00);

		//Radar
		patch::SetUChar(by_version<0x4A5124, 0x4A5214>(), 0x0F);
		patch::SetUChar(by_version<0x4A5125, 0x4A5215>(), 0x84);
		patch::SetUChar(by_version<0x4A5126, 0x4A5216>(), 0xDD);
		patch::SetUChar(by_version<0x4A5127, 0x4A5217>(), 0x00);
		patch::SetUChar(by_version<0x4A5128, 0x4A5218>(), 0x00);

		patch::SetUChar(by_version<0x4A53D3, 0x4A54C3>(), 0x0F);
		patch::SetUChar(by_version<0x4A53D4, 0x4A54C4>(), 0x84);
		patch::SetUChar(by_version<0x4A53D5, 0x4A54C5>(), 0x47);
		patch::SetUChar(by_version<0x4A53D6, 0x4A54C6>(), 0x01);
		patch::SetUChar(by_version<0x4A53D7, 0x4A54C7>(), 0x00);

		//Particle From Car
		patch::SetUChar(by_version<0x4A9400, 0x4A94F0>(), 0x75);
		patch::SetUChar(by_version<0x4A9401, 0x4A94F1>(), 0x4F);
		patch::SetUChar(by_version<0x4A9402, 0x4A94F2>(), 0x83);
		patch::SetUChar(by_version<0x4A9403, 0x4A94F3>(), 0x78);
		patch::SetUChar(by_version<0x4A9404, 0x4A94F4>(), 0x28);

		//Gun Point
		patch::SetUChar(by_version<0x4C6B39, 0x4C6BD9>(), 0x74);
		patch::SetUChar(by_version<0x4C6B3A, 0x4C6BDA>(), 0x35);
		patch::SetUChar(by_version<0x4C6B3B, 0x4C6BDB>(), 0x8D);
		patch::SetUChar(by_version<0x4C6B3C, 0x4C6BDC>(), 0x8B);
		patch::SetUChar(by_version<0x4C6B3D, 0x4C6BDD>(), 0xF0);

		//Target Coords
		patch::SetFloat(by_version<0x46BE9B, 0x46BE77>(), CrossX);
		patch::SetFloat(by_version<0x46BEA5, 0x46BE81>(), CrossY);

		//WideScreenFix
		patch::SetUInt(by_version<0x50550F, 0x5055EF>(), WideScreenFix);

		//Particle Fix
		patch::SetUChar(by_version<0x50F0F3, 0x50F2E3>(), 0x0F);
		patch::SetUChar(by_version<0x50F0F4, 0x50F2E4>(), 0x84);
		patch::SetUChar(by_version<0x50F0F5, 0x50F2E5>(), 0xCD);
		patch::SetUChar(by_version<0x50F0F6, 0x50F2E6>(), 0x05);
		patch::SetUChar(by_version<0x50F0F7, 0x50F2E7>(), 0x00);
		patch::SetUChar(by_version<0x50F0F8, 0x50F2E8>(), 0x00);

		patch::SetUChar(by_version<0x50F111, 0x50F301>(), 0x0F);
		patch::SetUChar(by_version<0x50F112, 0x50F302>(), 0x84);
		patch::SetUChar(by_version<0x50F113, 0x50F303>(), 0xB9);
		patch::SetUChar(by_version<0x50F114, 0x50F304>(), 0x05);
		patch::SetUChar(by_version<0x50F115, 0x50F305>(), 0x00);
		patch::SetUChar(by_version<0x50F116, 0x50F306>(), 0x00);
		
		//Bullet Impact On the Player Fix
		patch::SetUChar(by_version<0x55FC24, 0x55FD54>(), 0x0F);
		patch::SetUChar(by_version<0x55FC25, 0x55FD55>(), 0x83);

		patch::SetUChar(by_version<0x563C7D, 0x563DAD>(), 0x0F);
		patch::SetUChar(by_version<0x563C7E, 0x563DAE>(), 0x86);
	}
}
