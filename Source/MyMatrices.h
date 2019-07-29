#pragma once

#include "plugin.h"
#include "CQuaternion.h"

class MyMatrices{
public:
	static int DeltaTick;

	static void SetCameraMatrix(const CMatrix& CamMatrix);
	static void SetRotate(CMatrix& matrix, float RotX, float RotY, float RotZ);
	static void TimerUpdate();
	static float GetResValue(float value);
	static float AbsDegrees(float degrees);
	static float ATan2(float x, float y);
	static void GetOffsetFromMatrix(CMatrix *m, CVector *vOffset, CVector* vOut);
	static RtQuat * QuatInterpolate(RtQuat *out, RtQuat *in, RtQuat *quat, float slerp);
	static RwMatrix* GameQuatToMatrix(RwMatrix* a1, RtQuat* a2);
	static RwMatrix *QuatToMatrix(RtQuat *q, RwMatrix *m);
	static RtQuat* MatrixToQuat(RtQuat* Quat, RwMatrix* Mat);
	static CVector GetMatrixRotation(CMatrix& mat);
	static void VectorInterpolate(RwV3d *v1, RwV3d *v2, RwV3d *vOut, float t);
};