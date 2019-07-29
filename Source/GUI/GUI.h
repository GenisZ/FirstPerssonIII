#pragma once

#include "plugin.h"
#include "CSprite2D.h"

class GUI {
public:
	static bool isMenuActive;

	struct stSettings {
		unsigned int Key1, Key2;
		float FOV, LowZoom, HighZoom, MouseSens, GamepadSens;
		char InvertMouseVert, InvertGamepadVert;
		CVector CameraOffset, HeadOffsetInCar[70];
	}static Settings;

	static void Process();
	static void Init();
	static void ShutDown();
	static void CfgRead();
	static void CfgWrite();
	static void MouseHook();
	static void SetInVehicleOffset(RwV3d* Bone);
};