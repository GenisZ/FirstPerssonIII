#pragma once

class FOVhook {
	static unsigned int AdressFOV;

	static void SetFOV(float FOV);
public:
	static void InstallHook();
};