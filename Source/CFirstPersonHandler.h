#pragma once

#include "CMatrix.h"
#include "CVector2D.h"

class CFirstPersonHandler{
public:
	//members
	static bool IsEnable;
	static bool WasEnable;

	static float CurrentZoom;
	static float RotationProgress;
	
	static CMatrix CameraMatrix;

	static CVector2D BodyRotation;
	static CVector2D InCarRotation;

	static enum eProcessMode : __int8
	{
		MODE_NONE,
		MODE_ON_FOOT,
		MODE_ANIM,
		MODE_IN_CAR
	}ProcessMode, PrevProcessMode;

	//funcs
	static void Activate();
	static void TurnOff(bool wasActivate);
	static void TurnOn(bool wasActivate);
	static void RotatePlayerTorso();
	static void SwitchMode();
	static void Process();
	static void ProcessOnFoot();
	static void ProcessAnimation();
	static void ProcessInCar();
	static void ProcessZoom();
};