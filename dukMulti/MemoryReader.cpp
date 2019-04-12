#include "MemoryReader.h" // Include our header file

MemoryReader::MemoryReader() // Our Constructor
{

}

MemoryReader::~MemoryReader() // Our De-Constructor
{
	CloseHandle(hProcess); // Close the handle hProcess
}

void MemoryReader::AttachProcess(char* Name) // Get's our handle to the process passed in and gives the handle full rights
{
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(ProcessEntry);

	do
		if (!strcmp(ProcessEntry.szExeFile, Name))
		{
			dwPID = ProcessEntry.th32ProcessID;
			CloseHandle(hPID);

			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
			return;
		}
		else
			Sleep(1);
	while (Process32Next(hPID, &ProcessEntry));
}

DWORD MemoryReader::GetModuleBaseAddress(LPSTR Module) // Get's the base address of the module in our specified process
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	MODULEENTRY32 ModuleEntry;
	ModuleEntry.dwSize = sizeof(ModuleEntry);

	do
		if (!strcmp(ModuleEntry.szModule, Module))
		{
			CloseHandle(hModule);
			return (DWORD)ModuleEntry.modBaseAddr;
		}
	while (Module32Next(hModule, &ModuleEntry));
}