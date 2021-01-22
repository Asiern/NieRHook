#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <iostream>
#include "NierHook.hpp"
#include <thread>

//Hook to NieR:Automata process
void NieRHook::_hook(void)
{
	DWORD ID = this->_pID;
	uintptr_t entityAddress = 0;
	std::cout << "Hooking..." << std::endl;
	while (ID <= 0) {
		ID = this->_getProcessID();
	}
	std::cout << "Hooked." << std::endl;
	this->_pID = ID;
	this->_baseAddress = this->_getModuleBaseAddress(ID, L"NieRAutomata.exe");
	this->_hooked = true;
}
//unHook NieR:Automata
void NieRHook::_unHook(void)
{
	this->_hooked = false;
	this->_pID = 0;
	this->_baseAddress = 0;
	this->_entityAddress = 0;
}

//Search for window named "NieR:Automata" returns: process ID
DWORD NieRHook::_getProcessID(void)
{
	//Search game window
	HWND hwnd = FindWindowA(NULL, "NieR:Automata");
	if (hwnd == NULL)
	{
		//return if game window not found
		return 0;
	}
	DWORD pID; //Process ID	
	GetWindowThreadProcessId(hwnd, &pID); //Get Process ID
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); //Open process
	if (pHandle == INVALID_HANDLE_VALUE)
	{
		//return if couldn't open the process
		return 0;
	}
	return pID;
}
//Find modules in NieR:Automata process returns: memory address of module
uintptr_t NieRHook::_getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap); //Close handle to prevent memory leaks
	return modBaseAddr;
}

void NieRHook::_fetchData(void)
{
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
	float Xpos;
	float Ypos;
	float Zpos;
	int health;
	int maxHealth;
	uintptr_t XAddress = static_cast<uintptr_t>((DWORD)this->_entityAddress) + 0x50;
	uintptr_t YAddress = static_cast<uintptr_t>((DWORD)this->_entityAddress) + 0x54;
	uintptr_t ZAddress = static_cast<uintptr_t>((DWORD)this->_entityAddress) + 0x58;
	uintptr_t HealthAddress = static_cast<uintptr_t>((DWORD)this->_entityAddress) + 0x858;
	uintptr_t MaxHealthAddress = static_cast<uintptr_t>((DWORD)this->_entityAddress) + 0x858;
	std::cout << "Fetching" << std::endl;
	while (this->_hooked)
	{
		ReadProcessMemory(pHandle, (LPCVOID)XAddress, &Xpos, sizeof(Xpos), NULL);
		ReadProcessMemory(pHandle, (LPCVOID)YAddress, &Ypos, sizeof(Ypos), NULL);
		ReadProcessMemory(pHandle, (LPCVOID)ZAddress, &Zpos, sizeof(Zpos), NULL);
		ReadProcessMemory(pHandle, (LPCVOID)HealthAddress, &health, sizeof(health), NULL);
		ReadProcessMemory(pHandle, (LPCVOID)MaxHealthAddress, &maxHealth, sizeof(maxHealth), NULL);
		this->player.setPosition(Xpos, Ypos, Zpos);
		Sleep(1000);
	}
	std::cout << "END Fetching" << std::endl;
}

void NieRHook::_hookStatus(void)
{
	DWORD pID = this->_pID;
	while (this->_hooked) {
		if (pID != this->_getProcessID()) {
			this->_unHook();
		}
	}
}

NieRHook::NieRHook()
{
	this->_hooked = false;
	this->_baseAddress = 0;
	this->_entityAddress = 0;
	this->_pID = 0;
	Player player;
	this->player = player;
}

void NieRHook::start(void)
{
	this->_hook();//Hook
	//Get entity address from pointer at offset 0x16053B8
	uintptr_t entityAdd = 0;
	uintptr_t entityAddPointer = this->_baseAddress + 0x16053B8;
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
	std::cout << "Searchinf entity" << std::endl;
	while (entityAdd == 0) {
		ReadProcessMemory(pHandle, (LPCVOID)entityAddPointer, &entityAdd, sizeof(entityAdd), NULL);
	}
	CloseHandle(pHandle); //Close handle to prevent memory leaks
	std::cout << "Entity found" << std::endl;
	this->_entityAddress = entityAdd;
	std::thread th2(&NieRHook::_fetchData, this); //Create thread to fetch data from memory
	std::thread hookStatusThread(&NieRHook::_hookStatus, this); //Create thread to watch for hook status
	th2.join();
	hookStatusThread.join();
}

void NieRHook::stop(void)
{
	this->_unHook();
}

uintptr_t NieRHook::getBaseAddress(void)
{
	return this->_baseAddress;
}

DWORD NieRHook::getProcessID(void)
{
	return this->_pID;
}

Player* NieRHook::getPlayer()
{
	return &this->player;
}
