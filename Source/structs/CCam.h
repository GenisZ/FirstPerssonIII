/*
    Plugin-SDK (Grand Theft Auto 3) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"
#include "CVector.h"
#include "CEntity.h"

class PLUGIN_API CCam {
public:
    unsigned char f0[3];
    unsigned char byte3;
    unsigned char byte4;
    unsigned char f5[1];
    unsigned char byte6;
    unsigned char byte7;
    unsigned char byte8;
    unsigned char byte9;
    unsigned char byteA;
    unsigned char byteB;
    unsigned short m_wCamMode;
    unsigned char fE[6];
    unsigned int dword14;
    unsigned int dword18;
    unsigned int dword1C;
    unsigned char f20[8];
    unsigned int dword28;
    float dword2C;
    unsigned int dword30;
    unsigned int dword34;
    float dword38;
    unsigned int dword3C;
    unsigned char f40[12];
    unsigned int dword4C;
    unsigned int dword50;
    unsigned int dword54;
    unsigned char f58[8];
    unsigned int dword60;
    unsigned int dword64;
    unsigned int dword68;
    unsigned int dword6C;
    unsigned int dword70;
    unsigned char f74[12];
    unsigned int dword80;
    float dword84;
    float dword88;
    unsigned int dword8C;
    unsigned int dword90;
    unsigned char f94[24];
    unsigned int dwordAC;
    unsigned char fB0[8];
    unsigned int dwordB8;
    unsigned int dwordBC;
    float dwordC0;
    unsigned int dwordC4;
    unsigned int dwordC8;
    unsigned int dwordCC;
    unsigned char fD0[112];
    CVector Front; //140
    CVector Source;
    CVector SourceBeforeLookBehind;
    CVector Up; //164
    //unsigned int dword168;
    //float dword16C;
    char f170[24];
    CEntity *field_188;
    char gap_18C[12];
    unsigned int dword198;
    unsigned int dword19C;
    int field_1A0;
};

VALIDATE_SIZE(CCam, 0x1A4);
