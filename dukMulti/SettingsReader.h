#pragma once
#include <Windows.h>


#define WIN32_LEAN_AND_MEAN

class SettingsReader
{
public:
	//=================OFFSETS=====================//
	DWORD fOnGround = 0x1;

	DWORD m_dwClientState = 0x00610344;

	DWORD m_LocalPlayer = 0x00A804CC;
	DWORD m_iCrossHairID = 0x0000A954;

	DWORD m_dwEntityList = 0x04A9B464;
	DWORD m_GlowObjectManager = 0x4FB0B9C;

	DWORD m_vecOrigin = 0x134;
	DWORD m_viewAngles = 0x00004D0C;
	DWORD m_boneMatrix = 0x00002698;
	DWORD m_vecViewOffset = 0x00000104;
	DWORD m_aimPunchAngle = 0x3018;

	DWORD m_EntLoopDist = 0x10;
	DWORD m_iHealth = 0xFC;
	DWORD m_iTeamNum = 0xF0;
	DWORD m_fFlags = 0x100;
	DWORD m_bDormant = 0x000000E9;
	DWORD m_iGlowIndex = 0xA310;
	DWORD m_flFlashMaxAlpha = 0x0000A2F8;
	DWORD m_bSpotted = 0x00000939;

	//=============USER SETTINGS=================//
	bool noFlashEnabled = false;
	bool bhopEnabled = false;
	bool triggerEnabled = false;

	bool glowEnabled = false;
	bool radarHackEnabled = false;

	
	int recoilCompensationAmount = 2;

	int firstShotDelay = 0;
	int followingShotDelay = 0;
};