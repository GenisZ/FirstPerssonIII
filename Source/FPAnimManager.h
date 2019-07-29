#pragma once

#include "plugin.h"
#include "CAnimBlendAssocGroup.h"

class FPAnimManager {
public:
	static void LoadMyIFP();
	static void RemoveFPAnims();
	static void CreateMyAssociations(CAnimBlendAssocGroup* AssocGroup, char* blockName,
		RpClump* clump, char** AnimNames, int AnimNums);
	static CAnimBlendAssociation* GetMainAssotiation(RpClump* clump);
	static CAnimBlendAssociation* GetPartialAssotiation(RpClump* clump, bool fpanim);

	static char* aNames[];
};