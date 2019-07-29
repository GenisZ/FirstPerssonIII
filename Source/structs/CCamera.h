/*
    Plugin-SDK (Grand Theft Auto 3) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"
#include "CPlaceable.h"
#include "CCam.h"
#include "CVector.h"
#include "RenderWare.h"
#include "CEntity.h"
#include "CMatrix.h"

class PLUGIN_API CCamera{
    //PLUGIN_NO_DEFAULT_CONSTRUCTION(CCamera)

public:
	CPlaceable parent;
    unsigned char byte4C;
    unsigned char byte4D;
    unsigned char m_bDirectlyBehindForFollowPed_CamOnString;
    unsigned char m_bDirectlyInFrontForFollowPed_CamOnAString;
    unsigned char f50[2];
    unsigned char byte52;
    unsigned char f53[1];
    unsigned char byte54;
    unsigned char byte55;
    unsigned char byte56;
    unsigned char byte57;
    unsigned char byte58;
    unsigned char byte59;
    unsigned char byte5A;
    unsigned char byte5B;
    unsigned char byte5C;
    unsigned char byte5D;
    bool m_bVectorOnPed;
    unsigned char f5F[1];
    unsigned char byte60;
    unsigned char f61[2];
    unsigned char byte63;
    unsigned char m_bNearClipScript;
    unsigned char f65[1];
    unsigned char byte66;
    unsigned char byte67;
    unsigned char f68[1];
    unsigned char byte69;
    unsigned char byte6A;
    unsigned char m_bZoomValueFollowPedScript;
    unsigned char m_bZoomValueCamStringScript;
    unsigned char byte6D;
    unsigned char f6E[1];
    unsigned char m_bWideScreenOff;
    unsigned char m_bWideScreenOn;
    unsigned char f71[1];
    unsigned char byte72;
    unsigned char byte73;
    unsigned char f74[1];
    unsigned char byte75;
    char m_wCurrentCam;
    char field_77;
    int m_dwShakingTimerStart;
    unsigned char f7C[4];
    unsigned int dword80;
    unsigned char f84[5];
    unsigned char byte89;
    unsigned char f8A[2];
    unsigned int dword8C;
    unsigned int dword90;
    unsigned int dword94;
    int f98;
    int m_dwBlurBlue;
    int m_dwBlurGreen;
    int m_dwBlurRed;
    int m_dwBlurAlpha;
    unsigned int dwordAC;
    unsigned int dwordB0;
    unsigned int dwordB4;
    unsigned char fB8[4];
    unsigned int dwordBC;
    unsigned int m_dwMotionBlurAlpha;
    unsigned int dwordC4;
    unsigned int dwordC8;
    unsigned char fCC[12];
    unsigned short wordD8;
	unsigned short m_wCarMode;
    unsigned char fDC[4];
    unsigned int dwordE0;
    unsigned char fE4[8];
    unsigned int dwordEC;
    unsigned char fF0[44];
    float dword11C;
    unsigned int dword120;
    unsigned int dword124;
    float m_fShakingValue;
    float m_fZoomValueCamStringScript;
    unsigned int dword130;
    unsigned int dword134;
    float m_fNearScreenScript;
    unsigned char f13C[8];
    float m_fZoomValueFollowPedScript;
    unsigned int dword148;
    float f14C;
    unsigned int dword150;
    unsigned int dword154;
    unsigned char f158[8];
    float m_fModeOnPed;
    unsigned int dword164;
    unsigned char f168[44];
    float dword194;
    float dword198;
    float m_fCrosshairX;
    float m_fCrosshairY;
    CCam m_asCams[3];
    char field_690[32];
    unsigned int dword6B0;
    unsigned int dword6B4;
    unsigned int dword6B8;
    char f6BC[12];
    CVector m_vAimingCoors;
    char field_6D4[36];
    CVector m_vCamCutSceneOffset;
    char field_704[108];
    CVector m_vGameCamPosition;
    char field_77C[36];
    RwCamera *m_pRwCamera;
    CEntity *m_pTargetEntity;
    unsigned char f7A8[57672];
    unsigned char byteE8F0;
    unsigned char byteE8F1;
    unsigned char byteE8F2;
    unsigned char m_bFading;
    unsigned char byteE8F4;
    char fE8F5[3];
    CMatrix field_E8F8;
    char field_E940[88];
    float m_fFadingState;
    unsigned int dwordE99C;
    unsigned char fE9A0[4];
    unsigned int dwordE9A4;
    unsigned int dwordE9A8;
    unsigned int dwordE9AC;
    unsigned int dwordE9B0;
    unsigned char fE9B4[16];
    unsigned int dwordE9C4;
    unsigned short wordE9C8;
    unsigned char fE9CA[2];
    unsigned short wordE9CC;
    short field_E9CE;
    int field_E9D0;
    int field_E9D4;

    // virtual function #0 (not found)

};

extern CCamera &TheCamera;

VALIDATE_SIZE(CCamera, 0xE9D8);
