#pragma once // Include this only once on every compilation

#define WIN32_LEAN_AND_MEAN // Exclude not needed content of Win32API

#include <Windows.h>
#include <TlHelp32.h>

class MemoryReader 
{
	protected:
	HANDLE hProcess;
	DWORD dwPID;

	public:
	MemoryReader(); // Constructor
	~MemoryReader(); // De-Constructor

	template <class vData> // To give our function the datatype with Object.Function<DataType>(Parameters);
	vData ReadMemory(DWORD Address)
	{
		vData vReturn;
		ReadProcessMemory(hProcess, (LPVOID)Address, &vReturn, sizeof(vData), NULL);
		return vReturn;
	}

	template <class vData>
	void WriteMemory(DWORD Address, vData Value)
	{
		WriteProcessMemory(hProcess, (LPVOID)Address, &Value, sizeof(vData), NULL);
	}

	virtual void AttachProcess(char* Name);
	virtual DWORD GetModuleBaseAddress(LPSTR Module);
};