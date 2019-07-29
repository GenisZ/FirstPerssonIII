/*
    Plugin-SDK (Grand Theft Auto 3) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "CCamera.h"

CCamera &TheCamera = *reinterpret_cast<CCamera *>(GLOBAL_ADDRESS_BY_VERSION(0x6FACF8, 0x6FACF8, 0));//*(CCamera*)0x6FACF8;
