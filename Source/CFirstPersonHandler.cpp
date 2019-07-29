#include "CFirstPersonHandler.h"
#include "MyMatrices.h"
#include "CBodySystem.h"
#include "FPAnimManager.h"
#include "hooks/CHooksManager.h"
#include "structs/CCamera.h"
#include "GUI/GUI.h"
#include "CTimer.h"
#include "CPad.h"
#include "eBodyParts.h"
#include "eAnimations.h"

#define M_PI 3.14159265f

bool CFirstPersonHandler::IsEnable = false;
bool CFirstPersonHandler::WasEnable = false;
float CFirstPersonHandler::CurrentZoom;
float CFirstPersonHandler::RotationProgress;
CMatrix CFirstPersonHandler::CameraMatrix;
CVector2D CFirstPersonHandler::BodyRotation;
CVector2D CFirstPersonHandler::InCarRotation;
CFirstPersonHandler::eProcessMode CFirstPersonHandler::ProcessMode = MODE_NONE;
CFirstPersonHandler::eProcessMode CFirstPersonHandler::PrevProcessMode = MODE_NONE;

//funcs
void CFirstPersonHandler::TurnOn(bool wasActivate)
{
	IsEnable = true;
	CHooksManager::SetPathes(true);

	if (wasActivate)
	{
		WasEnable = false;
	}
	else
	{
		TheCamera.m_fModeOnPed = 1.0f;
		TheCamera.m_wCarMode = 0x4000;
	}
	BodyRotation.x = 0.0f; CurrentZoom = GUI::Settings.FOV;
}

void CFirstPersonHandler::TurnOff(bool wasActivate)
{
	IsEnable = false;
	CHooksManager::SetPathes(false);

	if (wasActivate)
	{
		WasEnable = true;
	}
	else
	{
		TheCamera.m_fModeOnPed = 1.0f;
		if (FindPlayerPed()->m_bInVehicle)
			TheCamera.m_wCarMode = 0x40A0;
		else
			TheCamera.m_wCarMode = 0x4000;
	}
	FPAnimManager::RemoveFPAnims();
	PrevProcessMode = ProcessMode = MODE_NONE;
}

void CFirstPersonHandler::RotatePlayerTorso()
{
	if (IsEnable && ProcessMode == MODE_ON_FOOT)
		CBodySystem::SetTorsoMatrix(CFirstPersonHandler::CameraMatrix);
}

void CFirstPersonHandler::Activate()
{
	static bool wasLookDown{ false };

	if (TheCamera.m_fModeOnPed == 4.0f)
		wasLookDown = true;

	if (!TheCamera.m_bVectorOnPed || TheCamera.m_asCams[0].m_wCamMode == 7
		 || TheCamera.m_asCams[0].m_wCamMode == 8 || TheCamera.m_asCams[0].m_wCamMode == 34)
	{
		if (IsEnable)
			TurnOff(true);
		return;
	}
	else if (WasEnable)
		TurnOn(true);

	if (*(bool*)plugin::by_version<0x8F5F7C, 0x8F6030>()) //CMenuManager::m_ControlMethod
	{
		if (IsEnable) TurnOff(false);
		return;
	}

	if (FindPlayerPed()->m_bInVehicle)
	{ 
		if (!IsEnable && TheCamera.m_wCarMode == 0x0)
			TurnOn(false);
		else if (IsEnable && (TheCamera.m_wCarMode != 0x4000 || TheCamera.m_fModeOnPed == 4.0f))
		{
			wasLookDown = false;
			TurnOff(false);
		}
	}
	else
	{
		if (!IsEnable && wasLookDown && TheCamera.m_fModeOnPed == 1.0f)
			TurnOn(false);
		else if (IsEnable && TheCamera.m_fModeOnPed == 4.0f)
		{
			wasLookDown = false;
			TurnOff(false);
		}
	}
}

void CFirstPersonHandler::SwitchMode()
{
	auto player = FindPlayerPed();
	PrevProcessMode = ProcessMode;

	if (FindPlayerPed()->m_bInVehicle)
	{
		if (player->m_nAction == 51 || (player->m_pRwClump 
			&& (RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_MAN_CAR_CRAWLOUTRHS) 
				|| RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_MAN_CAR_LJACKEDLHS)
				|| RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_MAN_CAR_LJACKEDRHS))))
			ProcessMode = MODE_ANIM;
		else
			ProcessMode = MODE_IN_CAR;
	}
	else
	{ 
		if (player->m_nAction == 50 || player->m_nAction == 52 
			|| player->m_nAction == 36 || player->m_nAction == 37
			|| player->m_nAction == 39 || player->m_nAction == 48 
			|| player->m_nAction == 49 || player->m_nAction == 56
			|| player->m_pRwClump && RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_MAN_FALL_COLLAPSE))
			ProcessMode = MODE_ANIM;
		else ProcessMode = MODE_ON_FOOT;
	}

	if (ProcessMode == MODE_ON_FOOT && PrevProcessMode != MODE_ON_FOOT)
	{
		if (PrevProcessMode == MODE_IN_CAR)
		{
			CVector CamRot = MyMatrices::GetMatrixRotation(TheCamera.parent.m_matrix);
			BodyRotation.x = CamRot.x; BodyRotation.y = CamRot.z;
			if (BodyRotation.x > 180.0f) BodyRotation.x = -(360.0f - BodyRotation.x);
		}
		else BodyRotation.x = 0.0f;
	}
	else if (ProcessMode == MODE_IN_CAR && PrevProcessMode != MODE_IN_CAR)
			InCarRotation.x = InCarRotation.y = 0.0f;
}

void CFirstPersonHandler::Process()
{
	MyMatrices::TimerUpdate();
	Activate();

	if (IsEnable)
	{
		SwitchMode();

		switch (ProcessMode)
		{
			case MODE_ON_FOOT:
				ProcessOnFoot();
				break;
			case MODE_IN_CAR:
				ProcessInCar();
				break;
			case MODE_ANIM:
				ProcessAnimation();
		}

		CameraMatrix = CBodySystem::GetHeadMatrix();
		MyMatrices::GetOffsetFromMatrix(&CameraMatrix, &GUI::Settings.CameraOffset, &CameraMatrix.pos);
		MyMatrices::SetCameraMatrix(CameraMatrix);
		TheCamera.m_pRwCamera->nearPlane = 0.01f;
		ProcessZoom();
	}
}

void CFirstPersonHandler::ProcessOnFoot()
{
	auto player = FindPlayerPed();
	auto pad = CPad::GetPad(0);

	if (PrevProcessMode != MODE_IN_CAR)
		BodyRotation.y = player->GetHeading();

	if (!pad->m_bDisablePlayerControls)
	{
		BodyRotation.x += CPad::NewMouseControllerState.Y * 0.1f * (-1 * GUI::Settings.InvertMouseVert) * GUI::Settings.MouseSens;
		BodyRotation.x += pad->LookAroundUpDown() * 0.025f * (-1 * GUI::Settings.InvertGamepadVert) * GUI::Settings.GamepadSens;
		if (player->m_nAction != 24 && player->m_nAction != 6)
		{
			BodyRotation.y += CPad::NewMouseControllerState.X *-0.1f * GUI::Settings.MouseSens;
			BodyRotation.y += pad->LookAroundLeftRight() * -0.025f * GUI::Settings.GamepadSens;
		}
	}

	if (BodyRotation.x > 70.0f) BodyRotation.x = 70.0f;
	if (BodyRotation.x < -70.0f) BodyRotation.x = -70.0f;

	BodyRotation.y = MyMatrices::AbsDegrees(BodyRotation.y);

	if (player->m_nAction != 24 && player->m_nAction != 6 && !pad->m_bDisablePlayerControls)
		CBodySystem::SetAngleZ(BodyRotation.y);

	CMatrix headMatrix;
	MyMatrices::SetRotate(headMatrix, BodyRotation.x, 0.0f, BodyRotation.y);
	CBodySystem::SetTorsoMatrix(headMatrix);
}

void CFirstPersonHandler::ProcessInCar()
{
	auto player = FindPlayerPed();
	auto pad = CPad::GetPad(0);
	auto PlayerVehicle = player->m_pVehicle;

	if (pad->GetLookBehindForCar() || pad->GetLookLeft() || pad->GetLookRight() 
		|| RotationProgress && player->m_nAction != 54)
	{
		if (pad->GetLookBehindForCar())
		{
			InCarRotation.y -= MyMatrices::GetResValue(720.0f);
			if (InCarRotation.y <= -180.0f) InCarRotation.y = -180.0f;

			if (InCarRotation.x > 0.0f)
			{
				InCarRotation.x -= MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x <= 0.0f) InCarRotation.x = 0.0f;
			}
			if (InCarRotation.x < 0.0f)
			{
				InCarRotation.x += MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x >= 0.0f) InCarRotation.x = 0.0f;
			}

			RotationProgress = 1.0f;
		}
		else if (pad->GetLookLeft())
		{
			if (InCarRotation.y > 90.0f)
			{
				InCarRotation.y -= MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y <= 90.0f) InCarRotation.y = 90.0f;
			}
			if (InCarRotation.y < 90.0f)
			{
				InCarRotation.y += MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y >= 90.0f) InCarRotation.y = 90.0f;
			}

			if (InCarRotation.x > 0.0f)
			{
				InCarRotation.x -= MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x <= 0.0f) InCarRotation.x = 0.0f;
			}
			if (InCarRotation.x < 0.0f)
			{
				InCarRotation.x += MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x >= 0.0f) InCarRotation.x = 0.0f;
			}

			RotationProgress = 0.5f;
		}
		else if (pad->GetLookRight())
		{
			if (InCarRotation.y < -90.0f)
			{
				InCarRotation.y += MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y >= -90.0f) InCarRotation.y = -90.0f;
			}
			if (InCarRotation.y > -90.0f)
			{
				InCarRotation.y -= MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y <= -90.0f) InCarRotation.y = -90.0f;
			}

			if (InCarRotation.x > 0.0f)
			{
				InCarRotation.x -= MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x <= 0.0f) InCarRotation.x = 0.0f;
			}
			if (InCarRotation.x < 0.0f)
			{
				InCarRotation.x += MyMatrices::GetResValue(180.0f);
				if (InCarRotation.x >= 0.0f) InCarRotation.x = 0.0f;
			}

			RotationProgress = 0.5f;
		}
		else
		{
			if (InCarRotation.y < 0.0f)
			{
				InCarRotation.y += MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y >= 0.0f)
				{
					InCarRotation.y = 0.0f;
					RotationProgress = 0.0f;
				}
			}

			if (InCarRotation.y > 0.0f)
			{
				InCarRotation.y -= MyMatrices::GetResValue(720.0f);
				if (InCarRotation.y <= 0.0f)
				{
					InCarRotation.y = 0.0f;
					RotationProgress = 0.0f;
				}
			}
		}
	}
	else
	{
		RotationProgress = 0.0f;

		if (!pad->m_bDisablePlayerControls && player->m_nAction != 49 && player->m_nAction != 56)
		{
			InCarRotation.x += CPad::NewMouseControllerState.Y * 0.1f * (-1 * GUI::Settings.InvertMouseVert) * GUI::Settings.MouseSens;
			InCarRotation.x += pad->LookAroundUpDown() * 0.025f * (-1 * GUI::Settings.InvertGamepadVert) * GUI::Settings.GamepadSens;
			InCarRotation.y += CPad::NewMouseControllerState.X *-0.1f * GUI::Settings.MouseSens;
			InCarRotation.y += pad->LookAroundLeftRight() * -0.025f * GUI::Settings.GamepadSens;
		}

		if (InCarRotation.x > 70.0f) InCarRotation.x = 70.0f;
		if (InCarRotation.x < -70.0f) InCarRotation.x = -70.0f;

		if (player->m_nAction != 54)
		{
			if (InCarRotation.y > 180.0f) InCarRotation.y = 180.0f;
			if (InCarRotation.y < -180.0f) InCarRotation.y = -180.0f;
		}
		else
		{
			if (InCarRotation.y > 90.0f) InCarRotation.y = 90.0f;
			if (InCarRotation.y < -90.0f) InCarRotation.y = -90.0f;
		}

		static int LastTime = 0;
		static bool IsBackRotate = false;

		if (!PlayerVehicle->m_fTotSpeed || (abs(InCarRotation.x) <= 30.0f && abs(InCarRotation.y) <= 50.0f
			&& !IsBackRotate) || CPad::NewMouseControllerState.Y || CPad::NewMouseControllerState.X)
			LastTime = CTimer::m_snTimeInMilliseconds;

		int TimePasses = CTimer::m_snTimeInMilliseconds - LastTime;

		if (TimePasses >= 2000)
		{
			IsBackRotate = true;
			if (InCarRotation.y < 0.0f)
			{
				InCarRotation.y += MyMatrices::GetResValue(150.0f);
				if (InCarRotation.y >= 0.0f) InCarRotation.y = 0.0f;
			}

			if (InCarRotation.y > 0.0f)
			{
				InCarRotation.y -= MyMatrices::GetResValue(150.0f);
				if (InCarRotation.y <= 0.0f) InCarRotation.y = 0.0f;
			}

			if (InCarRotation.x < 0.0f)
			{
				InCarRotation.x += MyMatrices::GetResValue(150.0f);
				if (InCarRotation.x >= 0.0f) InCarRotation.x = 0.0f;
			}

			if (InCarRotation.x > 0.0f)
			{
				InCarRotation.x -= MyMatrices::GetResValue(150.0f);
				if (InCarRotation.x <= 0.0f) InCarRotation.x = 0.0f;
			}

			if (InCarRotation.x == 0.0f && InCarRotation.y == 0.0f)
				IsBackRotate = false;
		}
	}

	GUI::SetInVehicleOffset(&((RwFrame*)CBodySystem::GetBone(9)->m_pIFrame)->modelling.pos);

	if (InCarRotation.y > 90.0f || InCarRotation.y < -90.0f)
	{
		if (InCarRotation.y < 0.0f)
		{
			float InCarBonesSlerp = (-InCarRotation.y - 90.0f) / 90.0f;
			CBodySystem::SlerpBones((RtQuat*)CBodySystem::InCarRotationR, InCarBonesSlerp);
			RwV3d newPos, outPos; auto RootBone = CBodySystem::GetBone(0);
			newPos.x = 0.30f; newPos.y = 0.0f; newPos.z = 0.0f;
			MyMatrices::VectorInterpolate(&((RwFrame*)RootBone->m_pIFrame)->modelling.pos, &newPos, &outPos, InCarBonesSlerp);
			((RwFrame*)RootBone->m_pIFrame)->modelling.pos = outPos;
		}
		else
		{
			float InCarBonesSlerp = (InCarRotation.y - 90.0f) / 90.0f;
			CBodySystem::SlerpBones((RtQuat*)CBodySystem::InCarRotationL, InCarBonesSlerp);
			RwV3d newPos, outPos; auto RootBone = CBodySystem::GetBone(0);
			newPos.x = -0.7f; newPos.y = 0.0f; newPos.z = 0.113f;
			MyMatrices::VectorInterpolate(&((RwFrame*)RootBone->m_pIFrame)->modelling.pos, &newPos, &outPos, InCarBonesSlerp);
			((RwFrame*)RootBone->m_pIFrame)->modelling.pos = outPos;
		}
	}
	

	CMatrix headMatrix;
	MyMatrices::SetRotate(headMatrix, InCarRotation.x, 0.0f, InCarRotation.y);
	headMatrix = player->m_pVehicle->m_matrix * headMatrix;
	CBodySystem::SetHeadMatrix(headMatrix);
}

void CFirstPersonHandler::ProcessAnimation()
{
	static RtQuat quats[3]; static float slerp = 1.0f;
	if (PrevProcessMode == MODE_ON_FOOT)
	{
		CBodySystem::SetTorsoMatrix(CameraMatrix); slerp = 1.0f;

		MyMatrices::MatrixToQuat(&quats[0], &((RwFrame*)CBodySystem::GetBone(1)->m_pIFrame)->modelling);
		MyMatrices::MatrixToQuat(&quats[1], &((RwFrame*)CBodySystem::GetBone(2)->m_pIFrame)->modelling);
		MyMatrices::MatrixToQuat(&quats[2], &((RwFrame*)CBodySystem::GetBone(9)->m_pIFrame)->modelling);
	}
	else if (PrevProcessMode != MODE_ANIM) slerp = 0.0f;

	if (slerp > 0.0f)
	{
		CBodySystem::InterpolateBone(1, &quats[0], slerp);
		CBodySystem::InterpolateBone(2, &quats[1], slerp);
		CBodySystem::InterpolateBone(9, &quats[2], slerp);

		slerp -= MyMatrices::GetResValue(3.0f);
	}
}

void CFirstPersonHandler::ProcessZoom()
{
	auto player = FindPlayerPed();
	auto pad = CPad::GetPad(0);

	float ResValue = MyMatrices::GetResValue(200.0f);
	
	if (pad->GetTarget() && ProcessMode == MODE_ON_FOOT)
	{
		if (player->m_bWeaponSlot == WEAPONTYPE_COLT45 || player->m_bWeaponSlot == WEAPONTYPE_UZI
			|| player->m_bWeaponSlot == WEAPONTYPE_SHOTGUN)
		{ 
			if (CurrentZoom > GUI::Settings.LowZoom)
			{
				CurrentZoom -= ResValue;
				if (CurrentZoom < GUI::Settings.LowZoom) CurrentZoom = GUI::Settings.LowZoom;
			}
			else if (CurrentZoom < GUI::Settings.LowZoom)
			{
				CurrentZoom += ResValue;
				if (CurrentZoom > GUI::Settings.LowZoom) CurrentZoom = GUI::Settings.LowZoom;
			}
			return;
		}
		else if (player->m_bWeaponSlot == WEAPONTYPE_AK47 || player->m_bWeaponSlot == WEAPONTYPE_M16)
		{ 
			if (CurrentZoom > GUI::Settings.HighZoom)
			{
				CurrentZoom -= ResValue;
				if (CurrentZoom < GUI::Settings.HighZoom) CurrentZoom = GUI::Settings.HighZoom;
			}
			else if (CurrentZoom < GUI::Settings.HighZoom)
			{
				CurrentZoom += ResValue;
				if (CurrentZoom > GUI::Settings.HighZoom) CurrentZoom = GUI::Settings.HighZoom;
			}
			return;
		}
	}

	if (CurrentZoom < GUI::Settings.FOV)
	{
		CurrentZoom += ResValue;
		if (CurrentZoom > GUI::Settings.FOV) CurrentZoom = GUI::Settings.FOV;
	}
	if (CurrentZoom > GUI::Settings.FOV)
	{
		CurrentZoom -= ResValue;
		if (CurrentZoom < GUI::Settings.FOV) CurrentZoom = GUI::Settings.FOV;
	}
}
