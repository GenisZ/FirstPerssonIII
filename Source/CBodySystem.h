#pragma once

#include "plugin.h"

class CBodySystem{
public:
	static unsigned char InCarRotationL[];
	static unsigned char InCarRotationR[];

	static RwFrame* GetBoneFrame(unsigned char Bone);
	static RwMatrix GetBoneMatrix(unsigned char Bone, bool WorldMatrix);
	static CMatrix GetHeadMatrix();
	static AnimBlendFrameData* GetBone(unsigned int bone);
	static void InterpolateBone(unsigned int bone, RtQuat* quat, float slerp);
	static void SlerpBones(RtQuat* rotations, float slerp);
	static void RemoveHead(bool Remove);
	static float GetCollSphereRadius();
	static void ExtendCol(float Radius);
	static void SetAngleZ(float Angle);
	static void SetHeadMatrix(CMatrix& HeadMatrix);
	static void SetTorsoMatrix(CMatrix& TorsoMatrix);
};