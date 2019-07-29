#include "MyMatrices.h"
#include "structs/CCamera.h"
#include "CGeneral.h"
#include "CTimer.h"

int MyMatrices::DeltaTick;

void MyMatrices::SetCameraMatrix(const CMatrix& CamMatrix)
{
	TheCamera.parent.m_matrix = CamMatrix;

	TheCamera.parent.m_matrix.right.x *= -1.0f;
	TheCamera.parent.m_matrix.right.y *= -1.0f;
	TheCamera.parent.m_matrix.right.z *= -1.0f;

	TheCamera.m_asCams[0].Up = CamMatrix.at;
	TheCamera.m_asCams[0].Front = CamMatrix.up;
	TheCamera.m_asCams[0].Source = CamMatrix.pos;

}

void MyMatrices::SetRotate(CMatrix& matrix, float RotX, float RotY, float RotZ)
{
	matrix.SetOrientation(RotX * 0.0174532925f, RotY * 0.0174532925f, RotZ * 0.0174532925f);
}

void MyMatrices::TimerUpdate()
{
	static int LastTick = CTimer::m_snTimeInMilliseconds;
	DeltaTick = CTimer::m_snTimeInMilliseconds - LastTick;
	LastTick = CTimer::m_snTimeInMilliseconds;
}

float MyMatrices::GetResValue(float value)
{
	return value * (float)DeltaTick / 1000.0f;
}

float MyMatrices::AbsDegrees(float degrees)
{
	while (degrees > 360.0f)
	{
		degrees -= 360.0;
	}

	while (degrees < 0.0)
	{
		degrees += 360.0;
	}
	return degrees;
}

float MyMatrices::ATan2(float x, float y)
{
	return CGeneral::GetATanOfXY(x, y) * 57.295776f - 90.0f;
}

void MyMatrices::GetOffsetFromMatrix(CMatrix * m, CVector * vOffset, CVector * vOut)
{
	vOut->x = m->pos.x + m->right.x * vOffset->x + m->up.x * vOffset->y + m->at.x * vOffset->z;
	vOut->y = m->pos.y + m->right.y * vOffset->x + m->up.y * vOffset->y + m->at.y * vOffset->z;
	vOut->z = m->pos.z + m->right.z * vOffset->x + m->up.z * vOffset->y + m->at.z * vOffset->z;
}

RtQuat * MyMatrices::QuatInterpolate(RtQuat * out, RtQuat * in, RtQuat * quat, float slerp)
{
	float a1 = 1.0f - slerp;
	float a2 = in->imag.x * quat->imag.x + in->imag.z * quat->imag.z + in->imag.y * quat->imag.y + in->real * quat->real;

	float a3, a6;

	if (a2 < 0.0f) a3 = -1.0f;
	else a3 = 1.0f;

	float a4 = a3 * a2;
	float a5 = 1.0 - a4;

	if (a5 < 0.00000011920929) a6 = slerp;
	else
	{
		float a7 = sqrt(1.0 - /*a5 * a5*/ a4 * a4);
		float a8 = atan2(a7, a4);
		float a9 = 1.0 / a7;
		a1 = sin(a8 * a1) * a9;
		a6 = sin(a8 * slerp) * a9;
	}

	float a10 = a6 * a3;

	out->imag.x = a1 * in->imag.x + a10 * quat->imag.x;
	out->imag.y = a10 * quat->imag.y + a1 * in->imag.y;
	out->imag.z = a10 * quat->imag.z + a1 * in->imag.z;
	out->real = a10 * quat->real + a1 * in->real;

	return out;
}

RwMatrix * MyMatrices::GameQuatToMatrix(RwMatrix * a1, RtQuat * a2)
{
	const RtQuat *v2; // eax@1
	float v3; // ST0C_4@1
	double v4; // st7@1
	double v5; // st6@1
	double v6; // st5@1
	double v7; // st4@1
	float v8; // ST00_4@1
	float v9; // ST0C_4@1
	float v10; // ST08_4@1
	float v11; // ST04_4@1
	double v12; // st3@1
	RwMatrix *result; // eax@1
	double v14; // st2@1
	float v15; // [sp+18h] [bp+8h]@1
	float v16; // [sp+1Ch] [bp+Ch]@1
	float v17; // [sp+1Ch] [bp+Ch]@1

	v2 = a2;
	v3 = *(float *)a2 * 2.0;
	v16 = *((float *)a2 + 1) * 2.0;
	v4 = 2.0 * *((float *)v2 + 2);
	v5 = v3 * *((float *)v2 + 3);
	v6 = v16 * *((float *)v2 + 3);
	v7 = v4 * *((float *)v2 + 3);
	v8 = v3 * *(float *)v2;
	v9 = v16 * *(float *)v2;
	v10 = v4 * *(float *)v2;
	v11 = v16 * *((float *)v2 + 1);
	v17 = v4 * *((float *)v2 + 1);
	v12 = v4 * *((float *)v2 + 2);
	result = a1;
	*(float *)a1 = 1.0 - v11 - v12;
	*((float *)a1 + 1) = v9 + v7;
	*((float *)a1 + 2) = v10 - v6;
	*((float *)a1 + 3) = 0.0;
	*((float *)a1 + 4) = v9 - v7;
	v14 = 1.0 - v8;
	v15 = v14;
	*((float *)result + 5) = v14 - v12;
	*((float *)result + 6) = v17 + v5;
	*((float *)result + 7) = 0.0;
	*((float *)result + 8) = v10 + v6;
	*((float *)result + 9) = v17 - v5;
	*((float *)result + 10) = v15 - v11;
	*((float *)result + 11) = 0.0;
	*((float *)result + 12) = 0.0;
	*((float *)result + 13) = 0.0;
	*((float *)result + 14) = 0.0;
	*((float *)result + 15) = 1.0;
	return result;
}

RwMatrix * MyMatrices::QuatToMatrix(RtQuat * q, RwMatrix * m)
{
	RwV3d pos;
	pos = m->pos;
	GameQuatToMatrix(m, q);
	m->pos = pos;
	return m;
}

RtQuat * MyMatrices::MatrixToQuat(RtQuat * Quat, RwMatrix * Mat)
{
	float T = Mat->right.x + Mat->up.y + Mat->at.z;

	if (T > 0)
	{
		float S = sqrt(T + 1.0) * 0.5;
		Quat->real = S;
		S = 0.25 / S;
		Quat->imag.x = (Mat->up.z - Mat->at.y) * S;
		Quat->imag.y = (Mat->at.x - Mat->right.z) * S;
		Quat->imag.z = (Mat->right.y - Mat->up.x) * S;
	}
	else
	{
		float cmp0 = Mat->right.x - Mat->up.y - Mat->at.z;
		float cmp1 = Mat->up.y - Mat->right.x - Mat->at.z;
		float cmp2 = Mat->at.z - (Mat->up.y + Mat->right.x);

		if (cmp0 >= cmp1 && cmp0 >= cmp2)
		{
			Quat->imag.x = sqrt(cmp0 + 1.0) * 0.5;
			float S = 0.25 / Quat->imag.x;
			Quat->imag.y = (Mat->up.x + Mat->right.y) * S;
			Quat->imag.z = (Mat->at.x + Mat->right.z) * S;
			Quat->real = (Mat->up.z - Mat->at.x) * S;
		}
		else if (cmp1 >= cmp2)
		{
			Quat->imag.y = sqrt(cmp1 + 1.0) * 0.5;
			float S = 0.25 / Quat->imag.y;
			Quat->imag.z = (Mat->at.y + Mat->up.z) * S;
			Quat->imag.x = (Mat->right.y + Mat->up.x) * S;
			Quat->real = (Mat->at.x - Mat->right.z) * S;
		}
		else
		{
			Quat->imag.z = sqrt(cmp2 + 1.0) * 0.5;
			float S = 0.25 / Quat->imag.z;
			Quat->imag.x = (Mat->right.z + Mat->at.x) * S;
			Quat->imag.y = (Mat->up.z + Mat->at.y) * S;
			Quat->real = (Mat->right.y - Mat->up.x) * S;
		}
	}

	return Quat;
}

CVector MyMatrices::GetMatrixRotation(CMatrix & mat)
{
	CVector rot;
	rot.z = ATan2(mat.up.x, mat.up.y);
	rot.y = ATan2(mat.right.z, sqrt(mat.right.x * mat.right.x + mat.right.y * mat.right.y));
	rot.x = AbsDegrees(360.0f - ATan2(mat.up.z, sqrt(mat.up.x * mat.up.x + mat.up.y * mat.up.y)));
	return rot;
}

void MyMatrices::VectorInterpolate(RwV3d * v1, RwV3d * v2, RwV3d * vOut, float t)
{
	if (t > 1.0) t = 1.0;
	if (t < 0.0) t = 0.0;
	vOut->x = (v2->x - v1->x) * t + v1->x;
	vOut->y = (v2->y - v1->y) * t + v1->y;
	vOut->z = (v2->z - v1->z) * t + v1->z;
}
