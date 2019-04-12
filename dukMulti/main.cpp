#include "MemoryReader.h"
#include "SettingsReader.h"
#include <iostream>
#include <cmath>
#include <thread> 
#include "Vector3.h"

//add more symbols if you want to be precise af
//P.S. You probably will not, my hack is to bad to be precise ;C
#define PI 3.14159265

DWORD fOnGround = 0x1;

DWORD m_dwClientState = 0x58BCFC;

DWORD m_LocalPlayer = 0xCD2764;
DWORD m_iCrossHairID = 0xB394;

DWORD m_dwEntityList = 0x4CE34DC;
DWORD m_GlowObjectManager = 0x5223730;

DWORD m_vecOrigin = 0x138;
DWORD m_viewAngles = 0x31D8;
DWORD m_boneMatrix = 0x26A8;
DWORD m_vecViewOffset = 0x108;
DWORD m_aimPunchAngle = 0x302C;

DWORD m_EntLoopDist = 0x10;
DWORD m_iHealth = 0x100;
DWORD m_iTeamNum = 0xF4;
DWORD m_fFlags = 0x104;
DWORD m_bDormant = 0xED;
DWORD m_iGlowIndex = 0xA3F8;
DWORD m_flFlashMaxAlpha = 0xA3DC;
DWORD m_bSpotted = 0x93D;
int flag = 0x104;


DWORD dwClientDLLBaseAddress;
DWORD dwEngineDllBaseAddress;
DWORD dwEngineState;
DWORD dwLocalPlayer;
DWORD dwGlowObjectManager;
int iLocalPlayerTeam;

bool noFlashEnabled = false;
bool bhopEnabled = false;
bool triggerEnabled = false;
bool glowEnabled = false;
bool radarHackEnabled = false;

//keys for activating some hacks
int bhopKey = 32;
int triggerKey = 86;

//how much to compensate for recoil ( 2 = ideal )
float recoilCompensationAmount = 2;

//delay before trigger bot to shoot for the first time
int firstShotDelay = 0;
//delay before trigger bot to shoot all the next bullets
int followingShotDelay = 0;



//our cs go window
HWND gameWindow;

//our memory reader
MemoryReader Memory;

//Threads
void NoFlashLoop();
void BHOPLoop();
void TriggerLoop();
void RadarHackLoop();
void GlowLoop();
void AddressUpdateLoop();

void SetGlow(int playerGlowIndex, float r, float g, float b, float a);

//updates console text's
void DrawConsole();






int main()
{
	gameWindow = FindWindow(NULL, "Counter-Strike: Global Offensive");

	//finding cs go windows
	while (!gameWindow)
	{
		std::cout << ". ";
		gameWindow = FindWindow(NULL, "Counter-Strike: Global Offensive");
		Sleep(300);
	}

	Memory.AttachProcess("csgo.exe");
	//getting base module addresses
	dwClientDLLBaseAddress = Memory.GetModuleBaseAddress("client_panorama.dll");
	dwEngineDllBaseAddress = Memory.GetModuleBaseAddress("engine.dll");

	//set things that change only on server change or player select other team
	dwEngineState = Memory.ReadMemory<DWORD>(dwEngineDllBaseAddress + m_dwClientState);
	dwLocalPlayer = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_LocalPlayer);
	iLocalPlayerTeam = Memory.ReadMemory<int>(dwLocalPlayer + m_iTeamNum);
	dwGlowObjectManager = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_GlowObjectManager);
	//and create a thread to update them once in a while
	std::thread AddressUpdateThread(AddressUpdateLoop);

	//threads for each hack
	std::thread NoFlashThread(NoFlashLoop);
	std::thread BHOPThread(BHOPLoop);
	std::thread TriggerThread(TriggerLoop);

	std::thread RadarHackThread(RadarHackLoop);
	std::thread GlowThread(GlowLoop);

	//draws the hack console for the first time
	DrawConsole();
	Sleep(9999);
	//I just added this in case anyone desides to use this in mm and cant remove one line of code
	//also if you are using this in mm go use a better one this is dog shit and I did this in like 10 minutes to see if I could
	exit(1);

	//infinite input loop
	while (true)
	{
		//get input number
		int input;
		std::cin >> input;

		//toggle hack at number 'input'
		switch (input)
		{
		case 0:
			noFlashEnabled = !noFlashEnabled;
			break;
		case 1:
			bhopEnabled = !bhopEnabled;
			break;
		case 2:
			triggerEnabled = !triggerEnabled;
			break;

		case 5:
			glowEnabled = !glowEnabled;
			break;
		case 6:
			radarHackEnabled = !radarHackEnabled;
			break;
		default:
			break;
		}

		//we changed hack state, redraw console

		DrawConsole();



	

	}

	return 0;
}

//addresses are updated here 
void AddressUpdateLoop()
{
	while (true)
	{
		dwEngineState = Memory.ReadMemory<DWORD>(dwEngineDllBaseAddress + m_dwClientState);
		//gets local player
		dwLocalPlayer = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_LocalPlayer);
		//update his team
		iLocalPlayerTeam = Memory.ReadMemory<int>(dwLocalPlayer + m_iTeamNum);

		dwGlowObjectManager = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_GlowObjectManager);
		Sleep(50);
	}
}

void RadarHackLoop()
{
	DWORD entity;
	while (true)
	{
		if (radarHackEnabled)
		{
			//loop through all entites
			for (int i = 1; i < 65; i++)
			{
				entity = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_dwEntityList + (i - 1) * m_EntLoopDist);
				//if entity exists
				if (entity)
				{
					//set it's spotted state to true
					Memory.WriteMemory<bool>(entity + m_bSpotted, true);
				}
			}
		}
		Sleep(1);
	}
}

void DrawConsole()
{
	system("cls");
	std::cout << "DUK'S SHITTY MULTI HACK FOR CS:GO" << std::endl;
	std::cout << "0.(!)NO FLASH ENABLED = " << noFlashEnabled << std::endl;
	std::cout << "1.BHOP ENABLED = " << bhopEnabled << std::endl;
	std::cout << "2.TRIGGER BOT ENABLED = " << triggerEnabled << std::endl;
	std::cout << "5.(!)GLOW ENABLED = " << glowEnabled << std::endl;
	std::cout << "6.(!)RADAR HACK ENABLED = " << radarHackEnabled << std::endl;
	std::cout << "write a number of the hack, to toggle it." << std::endl;
	std::cout << "if something broke, just reload the hack" << std::endl;
	std::cout << "updated gubbna in 2019" << std::endl;
}

//I am not sure if this is safe, but this makes players glow
void GlowLoop()
{
	while (true)
	{
		if (glowEnabled)
		{
			//loop through every player 
			for (int i = 1; i < 65; i++)
			{
				DWORD dwEntity = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_dwEntityList + (i - 1) * m_EntLoopDist);
				int iEntityTeam = Memory.ReadMemory<int>(dwEntity + m_iTeamNum);
				bool bEntityDormant = Memory.ReadMemory<bool>(dwEntity + m_bDormant);
				
				//if this is an actual player
				if (!bEntityDormant)
				{
					//get glow index
					int iGlowIndex = Memory.ReadMemory<int>(dwEntity + m_iGlowIndex);
					//if player from our team, paint it this color
					if(iEntityTeam == iLocalPlayerTeam) SetGlow(iGlowIndex, 1, 1, 0, 0.5f);
					//otherwise ( enemy player ), another color
					else SetGlow(iGlowIndex, 0, 1, 0, 0.5f);
				}
			}
		}
		Sleep(1);
	}
}

//sets rgba glow to entity
void SetGlow(int iEntityGlowIndex, float r, float g, float b, float a)
{
	//this are rgba values of glow
	Memory.WriteMemory<float>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x4), r);
	Memory.WriteMemory<float>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x8), g);
	Memory.WriteMemory<float>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0xC), b);
	Memory.WriteMemory<float>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x10), a);

	//this stuff somehow enables glow .-.
	Memory.WriteMemory<bool>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x24), true);
	Memory.WriteMemory<bool>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x25), false);
	Memory.WriteMemory<bool>(dwGlowObjectManager + ((iEntityGlowIndex * 0x38) + 0x26), false);
}



//I do not think that writing in to m_flFlashMaxAlpha is safe, but this doed exactly it
void NoFlashLoop()
{
	while (true)
	{
		if (noFlashEnabled)
		{
			//get amount of flash effect of player
			float fLocalPlayerFlashMaxAlpha = Memory.ReadMemory<float>(dwLocalPlayer + m_flFlashMaxAlpha);

			//if it is greater than zero 
			if (fLocalPlayerFlashMaxAlpha > 0.f)
			{
				//set it back to zero 
				Memory.WriteMemory<float>(dwLocalPlayer + m_flFlashMaxAlpha, 0.f);
			}
		}
		Sleep(1);
	}
}

void TriggerLoop()
{
	//is the shot first
	bool firstShot = true;
	while (true)
	{
		if (triggerEnabled && GetAsyncKeyState(triggerKey) < 0)
		{
			int iCrosshair = Memory.ReadMemory<int>(dwLocalPlayer + m_iCrossHairID);

			//gets infromation about player in crosshair
			DWORD dwEntity = Memory.ReadMemory<DWORD>(dwClientDLLBaseAddress + m_dwEntityList + (iCrosshair - 1) * m_EntLoopDist);
			int iEntityHealth = Memory.ReadMemory<int>(dwEntity + m_iHealth);
			int iEntityTeam = Memory.ReadMemory<int>(dwEntity + m_iTeamNum);

			//checks if player is not on our team, and is not dead
			if (iLocalPlayerTeam != iEntityTeam && iEntityHealth > 0 && iCrosshair >= 1 && iCrosshair < 65)
			{
				//if this is our first shot, wait more time
				if (firstShot)
				{
					firstShot = false;
					Sleep(firstShotDelay);
				}
				//press left mouse button and release it after a small amount of time
				SendMessage(gameWindow, WM_LBUTTONDOWN, VK_LBUTTON, 0x390000);
				Sleep(1);
				SendMessage(gameWindow, WM_LBUTTONUP, VK_LBUTTON, 0x390000);
				//wait before allowing trigger to run again
				Sleep(followingShotDelay);
			}
			//we did not shot somewhy. Next shot will be first again
			else
			{
				firstShot = true;
			}
		}
		Sleep(1);
	}
}

void BHOPLoop()
{
	while (true)
	{
		if (bhopEnabled && GetAsyncKeyState(bhopKey) < 0)
		{
			//i do not know what is stored here yet again, but this lets us check if player is on ground
			int dwFlags = Memory.ReadMemory<BYTE>(dwLocalPlayer + m_fFlags);

			// if player is on ground, press space
			if (dwFlags == 1)
			{
				SendMessage(gameWindow, WM_KEYDOWN, VK_SPACE, 0x390000);
				Sleep(1);
				SendMessage(gameWindow, WM_KEYUP, VK_SPACE, 0x390000);
			}
		}
		Sleep(1);
	}
}





//gets bone position of an entity			#copypasted code
Vector3 GetBonePosition(DWORD dwEntity, int iTargetBone)
{
	//i do not know shit about this
	DWORD BoneMatrix = Memory.ReadMemory<DWORD>(dwEntity + m_boneMatrix);

	//and this makes a vector with bone position
	Vector3 Bone(Memory.ReadMemory<float>(BoneMatrix + (0x30 * iTargetBone) + 0x0c),		//x coordinate of the bone
				 Memory.ReadMemory<float>(BoneMatrix + (0x30 * iTargetBone) + 0x1c),		//y
				 Memory.ReadMemory<float>(BoneMatrix + (0x30 * iTargetBone) + 0x2c));		//z
	
	return Bone;
}

//Multiplies every parametor of a vector by amount
Vector3 VectorMultiplied(Vector3 vector, float amount)
{
	vector.x *= amount;
	vector.y *= amount;
	vector.z *= amount;
	return vector;
}

//vector2 - vector1 ( what did you expect :D?)
Vector3 VectorDifference(Vector3 vector1, Vector3 vector2)
{
	return Vector3(vector2.x - vector1.x, vector2.y - vector1.y, vector2.z - vector1.z);
}

//Distance between end point of two vectors(for vectors v1 and v2 = VectorMagnitude(VectorDifference(v2, v1))))
float VectorDistance(Vector3 vector1, Vector3 vector2)
{
	return sqrt(pow((vector1.x - vector2.x),2) + pow((vector1.y - vector2.y), 2) + pow((vector1.z - vector2.z), 2));
}

//length of the vector
float VectorMagnitude(Vector3 vector)
{
	return sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

