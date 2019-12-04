#pragma once

#include "plugin.h"

class CHooksManager {
public:
	static void InstallHooks();
	static void SetPathes(bool Set);
	static void InjectGUI();
	static void DisableCamera3rdPersonProcess(bool Set);
	static void ChangeCode(bool Set);
	static void DisableClassicAxis(bool Set);
	static void PatchCrossHair(bool Set);
	static void ChangeColl(bool Set);
};