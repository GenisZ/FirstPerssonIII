#include "CBodySystem.h"
#include "MyMatrices.h"
#include "CModelInfo.h"
#include "eBodyParts.h"

#define M_PI 3.14159265f

unsigned char CBodySystem::InCarRotationL[]
{
	0xa2, 0x6e, 0xaa, 0x3e, 0xd2, 0x67, 0x8c, 0x3d, 0x48, 0x6c, 0x86, 0x3d, 0xb5, 0x2c, 0x70, 0x3f,
	0x28, 0x3c, 0x85, 0x3d, 0xa1, 0x11, 0xc6, 0x3b, 0xca, 0x3a, 0x80, 0x3d, 0x1c, 0xf3, 0x7e, 0x3f,
	0x7a, 0x7c, 0x24, 0xbe, 0xe9, 0xc5, 0xd4, 0xbe, 0x5f, 0x17, 0x17, 0xbf, 0x4, 0x55, 0x2c, 0x3f,
	0x31, 0x1e, 0x3b, 0xbd, 0xd9, 0xc, 0x2f, 0xbd, 0x5e, 0x17, 0x1c, 0xbf, 0x52, 0x46, 0x4a, 0x3f,
	0x3d, 0x33, 0x54, 0xbf, 0xc2, 0xa0, 0x83, 0x3c, 0x87, 0xa2, 0xd8, 0xbd, 0x2d, 0x8f, 0xc, 0x3f,
	0xbb, 0x35, 0x42, 0xbe, 0x7d, 0x38, 0x53, 0x3b, 0xe2, 0x5c, 0x71, 0x3f, 0x3d, 0x50, 0x8c, 0x3e,
	0x1, 0xa8, 0xdc, 0x39, 0x2, 0xa2, 0xed, 0xba, 0x1, 0x3a, 0xdb, 0xbe, 0x6d, 0x58, 0x67, 0x3f,
	0x6b, 0x84, 0x4c, 0x3f, 0x5, 0x72, 0xef, 0x3d, 0x28, 0x62, 0x1b, 0xbd, 0xc, 0xba, 0x16, 0x3f,
};

unsigned char CBodySystem::InCarRotationR[]
{
	0x18, 0x61, 0xa2, 0xbe, 0x74, 0x10, 0x7a, 0xbd, 0xd5, 0xa4, 0x81, 0x3d, 0xfb, 0xbc, 0x71, 0x3f,
	0xe8, 0xe0, 0xa9, 0xbd, 0xab, 0xef, 0x3f, 0xbc, 0x50, 0x22, 0x7c, 0x3d, 0xeb, 0x9c, 0x7e, 0x3f,
	0x58, 0x65, 0x2f, 0x3e, 0x82, 0x4f, 0xe2, 0x3c, 0x40, 0x5c, 0x71, 0x3f, 0xce, 0xb7, 0x91, 0x3e,
	0x0, 0xb6, 0xd, 0x3a, 0x0, 0x24, 0xae, 0x39, 0x86, 0x22, 0xe4, 0xbe, 0xb5, 0x2e, 0x65, 0x3f,
	0xf5, 0x21, 0x4d, 0xbf, 0x0, 0x1e, 0x39, 0xbe, 0x4a, 0xe3, 0x3f, 0x3d, 0xa7, 0x80, 0x11, 0x3f,
	0xea, 0xc, 0x61, 0x3e, 0x51, 0xd9, 0xef, 0x3e, 0xa3, 0x92, 0x28, 0xbf, 0x48, 0xe6, 0xb, 0x3f,
	0x5c, 0x89, 0x12, 0x3d, 0xfa, 0x3d, 0x69, 0x3d, 0x35, 0xef, 0x17, 0xbf, 0x74, 0x51, 0x4d, 0x3f,
	0x6a, 0x9a, 0x3a, 0x3f, 0x9b, 0xf0, 0x5e, 0x3e, 0x49, 0x30, 0xf4, 0x3d, 0xde, 0x54, 0x23, 0x3f,
};

//funcs
RwFrame* CBodySystem::GetBoneFrame(unsigned char Bone)
{
	return (RwFrame*)*(int*)((int)FindPlayerPed()->m_apFrames[Bone] + 0x10);
}

RwMatrix CBodySystem::GetBoneMatrix(unsigned char Bone, bool WorldMatrix)
{
	auto BoneFrame = GetBoneFrame(Bone);
	RwMatrix BodyMatrix;

	if (WorldMatrix)
		return *CPedIK::GetWorldMatrix(BoneFrame, &BodyMatrix);
	else
		return BoneFrame->modelling;
}

CMatrix CBodySystem::GetHeadMatrix()
{
	CMatrix tempMatrix = CMatrix(&GetBoneMatrix(HEAD, true), false);
	CMatrix tempMatrix2;

	tempMatrix2.SetRotate(0.0f * M_PI, 0.5f * M_PI, 0.0f * M_PI);
	tempMatrix2.pos.x = 0.1f;
	tempMatrix = tempMatrix * tempMatrix2;

	return tempMatrix;
}

AnimBlendFrameData * CBodySystem::GetBone(unsigned int bone)
{
	auto player = FindPlayerPed();

	if (!player->m_pRwClump) return nullptr;

	CAnimBlendClumpData* clumpData = *(CAnimBlendClumpData**)((unsigned int)player->m_pRwClump + ClumpOffset);
	return &clumpData->m_pBones[bone];
}

void CBodySystem::InterpolateBone(unsigned int bone, RtQuat * quat, float slerp)
{
	AnimBlendFrameData* boneH; RtQuat boneQuat;
	boneH = GetBone(bone); MyMatrices::MatrixToQuat(&boneQuat, &((RwFrame*)boneH->m_pIFrame)->modelling);
	MyMatrices::QuatInterpolate(&boneQuat, &boneQuat, quat, slerp);
	MyMatrices::QuatToMatrix(&boneQuat, &((RwFrame*)boneH->m_pIFrame)->modelling);
}

void CBodySystem::SlerpBones(RtQuat * rotations, float slerp)
{
	if (slerp > 1.0f) slerp = 1.0f;
	if (slerp <= 0.0f) return;

	if (!FindPlayerPed()->m_pRwClump) return;

	InterpolateBone(1, &rotations[0], slerp); InterpolateBone(5, &rotations[4], slerp);
	InterpolateBone(2, &rotations[1], slerp); InterpolateBone(6, &rotations[5], slerp);
	InterpolateBone(3, &rotations[2], slerp); InterpolateBone(7, &rotations[6], slerp);
	InterpolateBone(4, &rotations[3], slerp); InterpolateBone(8, &rotations[7], slerp);
}

void CBodySystem::RemoveHead(bool Remove)
{
	static char state;
	auto& headflags = ((RwObject*)((int)GetBoneFrame(HEAD)->objectList.link.next - 0x8))->flags;

	if (Remove)
	{ 
		state = headflags;
		headflags = 0x0;
	}
	else
		headflags = state;
}

float CBodySystem::GetCollSphereRadius()
{
	return CModelInfo::GetModelInfo(FindPlayerPed()->m_nModelIndex)->m_pColModel->m_pSpheres[2].m_fRadius;
}

void CBodySystem::ExtendCol(float Radius)
{
	CModelInfo::GetModelInfo(FindPlayerPed()->m_nModelIndex)->m_pColModel->m_pSpheres[2].m_fRadius = Radius;
}

void CBodySystem::SetAngleZ(float Angle)
{
	CPed& Player = *FindPlayerPed();
	MyMatrices::SetRotate(Player.m_matrix, 0.0f, 0.0f, Angle);
	Player.m_fRotationCur = Player.m_fRotationDest = Angle * 0.0174532925f;

	CMatrix* BaseMatrix = (CMatrix*)&(((RwFrame*)((RwFrame*)GetBone(0)->m_pIFrame)->object.parent)->modelling);
	MyMatrices::SetRotate(*BaseMatrix, 0.0f, 0.0f, Angle);
}

void CBodySystem::SetHeadMatrix(CMatrix & HeadMatrix)
{
	if (!FindPlayerPed()->m_pRwClump) return;

	CMatrix HeadMat(&GetBoneFrame(HEAD)->modelling, false); CMatrix ParentMatrix; 
	ParentMatrix = *(CMatrix*)CPedIK::GetWorldMatrix((RwFrame*)GetBone(2)->m_pIFrame, (RwMatrix*)&ParentMatrix);
	CVector pos = HeadMat.pos;

	HeadMat = Invert(ParentMatrix) * HeadMatrix;

	//fix
	CVector right;
	right = HeadMat.right;
	right.x *= -1.0;
	right.y *= -1.0;
	right.z *= -1.0;
	HeadMat.right = HeadMat.at;
	HeadMat.at = right;

	HeadMat.pos = pos;
	HeadMat.UpdateRW();
}

void CBodySystem::SetTorsoMatrix(CMatrix& TorsoMatrix)
{
	if (!FindPlayerPed()->m_pRwClump) return;

	CMatrix TorsoMat(&GetBoneFrame(TORSO)->modelling, false);
	TorsoMat.ResetOrientation();
	TorsoMat.UpdateRW();

	CMatrix ParentMatrix = *(CMatrix*)&GetBoneMatrix(TORSO, true);
	CMatrix BodyMatrix(&((RwFrame*)GetBone(2)->m_pIFrame)->modelling, false);
	CVector pos = BodyMatrix.pos;

	BodyMatrix = Invert(ParentMatrix) * TorsoMatrix;

	//fix
	CVector right;
	right = BodyMatrix.right;
	right.x *= -1.0;
	right.y *= -1.0;
	right.z *= -1.0;
	BodyMatrix.right = BodyMatrix.at;
	BodyMatrix.at = right;

	BodyMatrix.pos = pos;
	BodyMatrix.UpdateRW();

	CMatrix HeadMat(&GetBoneFrame(HEAD)->modelling, false);
	HeadMat.ResetOrientation();
	HeadMat.UpdateRW();
}