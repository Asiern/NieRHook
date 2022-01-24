#include "NierHook.hpp"
#include <TlHelp32.h>
#include <Windows.h>
#include <iostream>

// Search for window named "NieR:Automata" returns: process ID
DWORD NieRHook::_getProcessID(void)
{
    // Search game window
    HWND hwnd = FindWindowA(NULL, "NieR:Automata");
    if (hwnd == NULL)
        return 0;

    DWORD pID;                                                    // Process ID
    GetWindowThreadProcessId(hwnd, &pID);                         // Get Process ID
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID); // Open process
    if (pHandle == INVALID_HANDLE_VALUE)
        return 0;

    return pID;
}
// Find modules in NieR:Automata process returns: memory address of module
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
    CloseHandle(hSnap); // Close handle to prevent memory leaks
    return modBaseAddr;
}

// Fill memory with custom values
void NieRHook::_patch(BYTE* destination, BYTE* src, unsigned int size)
{
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
    DWORD oldprotection;
    VirtualProtectEx(pHandle, destination, size, PAGE_EXECUTE_READWRITE, &oldprotection);
    WriteProcessMemory(pHandle, destination, src, size, nullptr);
    VirtualProtectEx(pHandle, destination, size, oldprotection, &oldprotection);
    CloseHandle(pHandle);
}

void NieRHook::hookStatus(void)
{
    if (this->_pID != this->_getProcessID())
    {
        this->stop();
    }
}

int NieRHook::getVersion()
{
    return this->version;
}

// Update Player Attributes
void NieRHook::update(void)
{
    this->_entityAddress = this->readMemory<uintptr_t>(this->_baseAddress + this->_offsets.entity);
    this->Health = readMemory<int>(this->_entityAddress + this->_offsets.health);
    this->MaxHealth = readMemory<int>(this->_entityAddress + this->_offsets.maxHealth);
    this->Xpos = readMemory<float>(this->_entityAddress + this->_offsets.x);
    this->Ypos = readMemory<float>(this->_entityAddress + this->_offsets.y);
    this->Zpos = readMemory<float>(this->_entityAddress + this->_offsets.z);
    this->Level = readMemory<int>(this->_entityAddress + this->_offsets.level);
    this->EXP = readMemory<int>(this->_baseAddress + this->_offsets.exp);
    this->Funds = readMemory<int>(this->_baseAddress + this->_offsets.funds);
}

float NieRHook::getXPosition(void)
{
    return this->Xpos;
}

float NieRHook::getYPosition(void)
{
    return this->Ypos;
}

float NieRHook::getZPosition(void)
{
    return this->Zpos;
}

int NieRHook::getFunds(void)
{
    return this->Funds;
}

int NieRHook::getEXP(void)
{
    return this->EXP;
}

void NieRHook::setX(float X)
{
    NoCLip(true); // Enable noclip to teleport
    this->writeMemory(this->_entityAddress + this->_offsets.x, X);
    Sleep(500);
    NoCLip(false); // Disable noclip
}

void NieRHook::setY(float Y)
{
    NoCLip(true); // Enable noclip to teleport
    this->writeMemory(this->_entityAddress + this->_offsets.y, Y);
    Sleep(500);
    NoCLip(false); // Disable noclip
}

void NieRHook::setZ(float Z)
{
    NoCLip(true); // Enable noclip to teleport
    this->writeMemory(this->_entityAddress + this->_offsets.z, Z);
    Sleep(500);
    NoCLip(false); // Disable noclip
}

void NieRHook::setPosition(float X, float Y, float Z)
{
    NoCLip(true); // Enable noclip to teleport
    // Write values at offsets 0x50, 0x54, 0x58 with X,Y,Z
    this->writeMemory(this->_entityAddress + this->_offsets.x, X);
    this->writeMemory(this->_entityAddress + this->_offsets.y, Y);
    this->writeMemory(this->_entityAddress + this->_offsets.z, Z);
    Sleep(500);
    NoCLip(false); // Disable noclip
}

void NieRHook::setHealth(int health)
{
    this->writeMemory(this->_entityAddress + this->_offsets.health, health);
}

void NieRHook::setFunds(int funds)
{
    writeMemory(this->_baseAddress + this->_offsets.funds, funds);
}

void NieRHook::setGameSpeed(float speed)
{
    // Write memory at offset 0x160E6D8 to set game speed (default 1.0 float)
    this->writeMemory(this->_baseAddress + this->_offsets.GameSpeed, speed);
}

void NieRHook::NoCooldown(bool enabled)
{
    if (enabled)
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoCooldown.offset), this->_offsets.NoCooldown.enabled,
               this->_offsets.NoCooldown.size);
    else
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoCooldown.offset), this->_offsets.NoCooldown.disabled,
               this->_offsets.NoCooldown.size);
}

void NieRHook::InfiniteAirDash(bool enabled)
{
    if (enabled)
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteAirDash.offset),
               this->_offsets.InfiniteAirDash.enabled, this->_offsets.InfiniteAirDash.size);
    else
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteAirDash.offset),
               this->_offsets.InfiniteAirDash.disabled, this->_offsets.InfiniteAirDash.size);
}

void NieRHook::IgnoreUpgradeMaterials(bool enabled)
{
    if (enabled)
    { // Enable Ignore Upgrade Materials
        // Set memory at offset 0x5EE5CE = 90 90 90 to disable Weapon
        _patch((BYTE*)(this->_baseAddress + this->_offsets.WeaponMaterials.offset),
               this->_offsets.WeaponMaterials.enabled, this->_offsets.WeaponMaterials.size);
        // Set memory at offset 0x5EE7F0 = 90 90 90 to disable Pod
        _patch((BYTE*)(this->_baseAddress + this->_offsets.PodMaterials.offset), this->_offsets.PodMaterials.enabled,
               this->_offsets.PodMaterials.size);
    }
    else
    { // Disable Ignore Upgrade Materials
        // Set memory at offset 0x5EE5CE = previous values
        _patch((BYTE*)(this->_baseAddress + this->_offsets.WeaponMaterials.offset),
               this->_offsets.WeaponMaterials.disabled, this->_offsets.WeaponMaterials.size);
        // Set memory at offset 0x5EE7F0 = previous values
        _patch((BYTE*)(this->_baseAddress + this->_offsets.PodMaterials.offset), this->_offsets.PodMaterials.disabled,
               this->_offsets.PodMaterials.size);
    }
}

void NieRHook::NoCLip(bool enabled)
{
    if (enabled)
    {
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoClipX.offset), this->_offsets.NoClipX.enabled,
               this->_offsets.NoClipX.size);
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoClipY.offset), this->_offsets.NoClipY.enabled,
               this->_offsets.NoClipY.size);
    }
    else
    {
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoClipX.offset), this->_offsets.NoClipX.disabled,
               this->_offsets.NoClipX.size);
        _patch((BYTE*)(this->_baseAddress + this->_offsets.NoClipY.offset), this->_offsets.NoClipY.disabled,
               this->_offsets.NoClipY.size);
    }
}

void NieRHook::InfiniteDoubleJump(bool enabled)
{
    if (enabled)
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteDoubleJump.offset),
               this->_offsets.InfiniteDoubleJump.enabled, this->_offsets.InfiniteDoubleJump.size);
    else
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteDoubleJump.offset),
               this->_offsets.InfiniteDoubleJump.disabled, this->_offsets.InfiniteDoubleJump.size);
}

// TODO FreeCamera implementation
void NieRHook::FreeCamera(bool enabled)
{
    // int value;
    // if (enabled)
    //{
    //	//Enable freecam
    //	value = 0x80000000;
    //	writeMemory(0x141415B90, value);
    //}
    // else
    //{
    //	//Disable freecam
    //	value = 0;
    //	writeMemory(0x141415B90, value);
    //}
}

void NieRHook::InfiniteItemUsage(bool enabled)
{
    if (enabled)
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteItemUsage.offset),
               this->_offsets.InfiniteItemUsage.enabled, this->_offsets.InfiniteItemUsage.size);
    else
        _patch((BYTE*)(this->_baseAddress + this->_offsets.InfiniteItemUsage.offset),
               this->_offsets.InfiniteItemUsage.disabled, this->_offsets.InfiniteItemUsage.size);
}

/*
    Add item to inventory
    If Item is not in the inventory, Creates a new Item on memory
    returns: true if successful and false if not
*/
bool NieRHook::addItem(int ID, int number)
{
    uintptr_t Address = this->_baseAddress + this->_offsets.items_first;
    uintptr_t emptySlot = 0;
    unsigned int currentID;
    if (!this->_hooked)
    {
        return false;
    }
    while (Address <= this->_baseAddress + this->_offsets.items_last)
    {
        currentID = readMemory<unsigned int>(Address);
        if (ID == currentID)
        { // Item found on memory
            writeMemory(Address + 0x8, number);
            return true;
        }
        else if (emptySlot == 0 && currentID == 0xffffffff)
        { // Get empty slot address
            emptySlot = Address;
        }
        Address += 0xC; // Go to the next slot
    }
    if (emptySlot == 0)
    { // empty slot == 0 => no free slots on memory
        return false;
    }
    // Item not found on memory, create on empty slot
    writeMemory(emptySlot, ID);         // Set ID
    writeMemory(Address + 0x8, number); // Set level
}

bool NieRHook::removeItem(int ID)
{
    uintptr_t Address = this->_baseAddress + this->_offsets.items_first;
    unsigned int currentID;
    if (!this->_hooked)
    {
        // Not hooked return
        return false;
    }
    while (Address <= this->_baseAddress + this->_offsets.items_last)
    {
        currentID = readMemory<unsigned int>(Address);
        if (ID == currentID) // Item found
        {
            // Remove item from memory
            writeMemory(Address, 0xFFFFFFFF);
            return true;
        }
        Address += 0xC; // Go to the next slot
    }
    return false;
}

/*
    Add weapon to memory by ID
    returns: true if successful and false if not
*/
bool NieRHook::addWeapon(int ID, int level)
{
    uintptr_t Address = this->_baseAddress + this->_offsets.weapons_first;
    uintptr_t emptySlot = 0;
    unsigned int currentID;
    if (!this->_hooked)
    {
        return false; // Return if not hooked
    }
    while (Address <= this->_baseAddress + this->_offsets.weapons_last)
    {
        currentID = readMemory<unsigned int>(Address);
        if (ID == currentID)
        {                                      // Weapon found on memory
            writeMemory(Address + 0x4, level); // Set level
            return true;
        }
        if (emptySlot == 0 && currentID == 0xffffffff)
        { // Get empty slot address
            emptySlot = Address;
        }
        Address += 0x14; // Go to the next slot
    }
    if (emptySlot == 0)
    { // empty slot == 0 => no free slots on memory
        return false;
    }
    // Weapon not found on memory
    writeMemory(emptySlot, ID);          // Set Weapon ID
    writeMemory(emptySlot + 0x4, level); // Set level, level at offset 0x4 from ID
    return true;
}

bool NieRHook::removeWeapon(int ID)
{
    uintptr_t Address = this->_baseAddress + this->_offsets.weapons_first;
    unsigned int currentID;
    if (!this->_hooked)
    {
        return false; // Return if not hooked
    }
    while (Address <= this->_baseAddress + this->_offsets.weapons_last)
    {
        currentID = readMemory<unsigned int>(Address);
        if (ID == currentID)
        { // Weapon found on memory
            writeMemory(Address, 0xffffffff);
            return true;
        }
        Address += 0x14; // Go to the next slot
    }
    return false;
}

char* NieRHook::readMemoryString(uintptr_t address, int size)
{
    char* val = (char*)calloc(sizeof(char) * size, size);
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
    ReadProcessMemory(pHandle, (LPCVOID)address, val, size, NULL);
    CloseHandle(pHandle); // Close handle to prevent memory leaks
    return val;
}

void NieRHook::getGameVersion()
{
    // Check for v0.0.2
    char* version = readMemoryString(this->_baseAddress + 0x1422130, 14);
    if (strcmp(version, "version v0.0.2") == 0)
    {
        this->version = VER_0_0_2;
        free(version);
        return;
    }

    // Check for v0.0.1
    char version2[8];
    char* res;
    for (int i = 0; i < 7; i++)
    {
        res = readMemoryString(0x146557790 + (i * 2), 1);
        version[i] = *res;
        free(res);
    }
    version2[7] = '\0';
    if (strcmp(version, "1.0.0.0") == 0)
    {
        this->version = VER_0_0_1;
        free(version);
        return;
    }

    // No verion found
    this->version = 0;
    free(version);
}

NieRHook::NieRHook()
{
    this->_hooked = false;
    this->_baseAddress = 0;
    this->_entityAddress = 0;
    this->_pID = 0;
    this->Health = 0;
    this->MaxHealth = 0;
    this->Xpos = 0.000000;
    this->Ypos = 0.000000;
    this->Zpos = 0.000000;
    this->EXP = 0;
    this->Funds = 0;
    this->Level = 0;
    this->_offsets = {};
    this->version = 0;
}

NieRHook::~NieRHook()
{
}

void NieRHook::start(void)
{
    DWORD ID = this->_pID;
    while (ID <= 0)
    {
        ID = this->_getProcessID();
    }
    this->_pID = ID;
    this->_baseAddress = this->_getModuleBaseAddress(ID, L"NieRAutomata.exe");

    this->getGameVersion();
    if (this->version == 0)
        return;

    switch (this->version)
    {
    case VER_0_0_2:
        // Define offsets UWP
        this->_offsets = {};

        // Game
        this->_offsets.GameSpeed;
        this->_offsets.version = 0x1422130;

        // Player
        this->_offsets.entity = 0x1020948;
        this->_offsets.health = 0x858;
        this->_offsets.maxHealth = 0x85c;
        this->_offsets.x = 0x50;
        this->_offsets.y = 0x54;
        this->_offsets.z = 0x58;
        this->_offsets.level = 0x14BC;
        this->_offsets.exp = 0xFC6060;
        this->_offsets.funds = 0xFC6064;

        // Items
        this->_offsets.items_first = 0x148C4C4;
        this->_offsets.items_last = 0x148CE18;

        // Weapons
        this->_offsets.weapons_first = 0x148DCC4;
        this->_offsets.weapons_last = 0x148DFBC;

        // Misc
        this->_offsets.MusicVolume = 0x14956C0;
        this->_offsets.SoundEffectVolume = 0x14956C4;
        this->_offsets.VoiceVolume = 0x14956C8;
        this->_offsets.AudioOutput = 0x14956CC;
        this->_offsets.VoiceChanger = 0x14956D0;
        this->_offsets.ScreenBrightness = 0x14956DC;
        this->_offsets.Distance = 0x14956EC;
        this->_offsets.CombatDistance = 0x14956F0;
        this->_offsets.VerticalRotationSpeed = 0x14956F4;
        this->_offsets.HorizontalRotationSpeed = 0x14956F8;
        this->_offsets.HorizontalAutoAdjust = 0x14956FC;
        this->_offsets.VerticalAutoAdjust = 0x1495700;
        this->_offsets.FreeEnemyTracking = 0x1495704;
        this->_offsets.ZoomSpeed = 0x1495708;
        this->_offsets.PursuitSpeed = 0x1495710;
        this->_offsets.LockedEnemyTracking = 0x1495714;

        // Cheats
        // this->_offsets.NoClipX;
        // this->_offsets.NoClipY;
        this->_offsets.InfiniteDoubleJump.offset = 0x47E257;
        this->_offsets.InfiniteDoubleJump.enabled = (BYTE*)"\xFF\x0F\x8C";
        this->_offsets.InfiniteDoubleJump.disabled = (BYTE*)"\x02\x0f\x8D";
        this->_offsets.InfiniteDoubleJump.size = 3;
        // this->_offsets.NoCooldown;
        this->_offsets.InfiniteAirDash.offset = 0x47E391;
        this->_offsets.InfiniteAirDash.enabled = (BYTE*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        this->_offsets.InfiniteAirDash.disabled = (BYTE*)"\xC7\x83\x98\x0A\x01\x00\x01\x00\x00\x00";
        this->_offsets.InfiniteAirDash.size = 10;
        // this->_offsets.WeaponMaterials;
        // this->_offsets.PodMaterials;
        // this->_offsets.FreeCamera;
        this->_offsets.InfiniteItemUsage.offset = 0x7C9D82;
        this->_offsets.InfiniteItemUsage.enabled = (BYTE*)"\x90\x90\x90";
        this->_offsets.InfiniteItemUsage.disabled = (BYTE*)"\x89\x70\x08";
        this->_offsets.InfiniteItemUsage.size = 3;
        break;

    case VER_0_0_1:
        // Define Offsets Steam OLD
        this->_offsets = {};

        // Game
        this->_offsets.GameSpeed = 0x160E6D8;
        this->_offsets.version;

        // Player
        this->_offsets.entity = 0x16053B8;
        this->_offsets.health = 0x858;
        this->_offsets.maxHealth = 0x85c;
        this->_offsets.x = 0x50;
        this->_offsets.y = 0x54;
        this->_offsets.z = 0x58;
        this->_offsets.level = 0x14BC;
        this->_offsets.exp = 0x1984670;
        this->_offsets.funds = 0x1984674;

        // Items
        this->_offsets.items_first = 0x197C4C4;
        this->_offsets.items_last = 0x197CE18;

        // Weapons
        this->_offsets.weapons_first = 0x197DCC4;
        this->_offsets.weapons_last = 0x197DFBC;

        // Misc
        this->_offsets.MusicVolume = 0x19856C0;
        this->_offsets.SoundEffectVolume = 0x19856C4;
        this->_offsets.VoiceVolume = 0x19856C8;
        this->_offsets.AudioOutput = 0x19856CC;
        this->_offsets.VoiceChanger = 0x19856D0;
        this->_offsets.ScreenBrightness = 0x19856DC;
        this->_offsets.Distance = 0x19856EC;
        this->_offsets.CombatDistance = 0x19856F0;
        this->_offsets.VerticalRotationSpeed = 0x19856F4;
        this->_offsets.HorizontalRotationSpeed = 0x19856F8;
        this->_offsets.HorizontalAutoAdjust = 0x19856FC;
        this->_offsets.VerticalAutoAdjust = 0x1985700;
        this->_offsets.FreeEnemyTracking = 0x1985704;
        this->_offsets.ZoomSpeed = 0x1985708;
        this->_offsets.PursuitSpeed = 0x1985710;
        this->_offsets.LockedEnemyTracking = 0x1985714;

        // Cheats
        this->_offsets.NoClipX.offset = 0x1354B1;
        this->_offsets.NoClipX.enabled = (BYTE*)"\x90\x90\x90\x90";
        this->_offsets.NoClipX.disabled = (BYTE*)"\x0F\x29\x42\x50";
        this->_offsets.NoClipX.size = 4;
        this->_offsets.NoClipY.offset = 0x135758;
        this->_offsets.NoClipY.enabled = (BYTE*)"\x90\x90\x90\x90";
        this->_offsets.NoClipY.disabled = (BYTE*)"\x0F\x29\x43\x50";
        this->_offsets.NoClipY.size = 4;
        this->_offsets.InfiniteDoubleJump.offset = 0x1E2D4C;
        this->_offsets.InfiniteDoubleJump.enabled = (BYTE*)"\xFF\x0F\x8C";
        this->_offsets.InfiniteDoubleJump.disabled = (BYTE*)"\x02\x0f\x8D";
        this->_offsets.InfiniteDoubleJump.size = 3;
        this->_offsets.NoCooldown.offset = 0x23821F;
        this->_offsets.NoCooldown.enabled = (BYTE*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        this->_offsets.NoCooldown.disabled = (BYTE*)"\xF3\x0F\x11\x84\xC3\x24\x6A\x01\x00";
        this->_offsets.NoCooldown.size = 9;
        this->_offsets.InfiniteAirDash.offset = 0x47E391;
        this->_offsets.InfiniteAirDash.enabled = (BYTE*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        this->_offsets.InfiniteAirDash.disabled = (BYTE*)"\xC7\x83\x98\x0A\x01\x00\x01\x00\x00\x00";
        this->_offsets.InfiniteAirDash.size = 10;
        this->_offsets.WeaponMaterials.offset = 0x5EE5CE;
        this->_offsets.WeaponMaterials.enabled = (BYTE*)"\x90\x90\x90\x90\x90";
        this->_offsets.WeaponMaterials.disabled = (BYTE*)"\x41\x3B\xC2\x7C\x31";
        this->_offsets.WeaponMaterials.size = 5;
        this->_offsets.PodMaterials.offset = 0x5EE7F0;
        this->_offsets.PodMaterials.enabled = (BYTE*)"\x90\x90\x90";
        this->_offsets.PodMaterials.disabled = (BYTE*)"\x83\xFB\xFF";
        this->_offsets.PodMaterials.size = 3;
        // this->_offsets.FreeCamera;
        this->_offsets.InfiniteItemUsage.offset = 0x7C9D82;
        this->_offsets.InfiniteItemUsage.enabled = (BYTE*)"\x90\x90\x90";
        this->_offsets.InfiniteItemUsage.disabled = (BYTE*)"\x89\x70\x08";
        this->_offsets.InfiniteItemUsage.size = 3;
        break;
    }

    this->_hooked = true;
}

void NieRHook::stop(void)
{
    this->_hooked = false;
    this->version = 0;
    this->_baseAddress = 0;
    this->_entityAddress = 0;
    this->_pID = 0;
    this->Health = 0;
    this->MaxHealth = 0;
    this->Xpos = 0.000000;
    this->Ypos = 0.000000;
    this->Zpos = 0.000000;
    this->EXP = 0;
    this->Funds = 0;
    this->Level = 0;

    // Disable cheats
    this->NoCLip(false);
    this->InfiniteDoubleJump(false);
    this->setGameSpeed(1);
    this->NoCooldown(false);
    this->InfiniteAirDash(false);
    this->IgnoreUpgradeMaterials(false);
    this->FreeCamera(false);
    this->InfiniteItemUsage(false);
}

DWORD NieRHook::getProcessID(void)
{
    return this->_pID;
}

bool NieRHook::isHooked(void)
{
    return this->_hooked;
}

int NieRHook::getLevel(void)
{
    return this->Level;
}

int NieRHook::getHealth(void)
{
    return this->Health;
}

int NieRHook::getMaxHealth(void)
{
    return this->MaxHealth;
}

void NieRHook::setHUDOpacity(float opacity)
{
    writeMemory(this->_baseAddress + _offsets.HUDOpacity, opacity);
}

void NieRHook::setColor(float R, float G, float B)
{
    writeMemory(this->_baseAddress + _offsets.R, R);
    writeMemory(this->_baseAddress + _offsets.G, G);
    writeMemory(this->_baseAddress + _offsets.B, B);
}

void NieRHook::setBrightness(int value)
{
    writeMemory(this->_baseAddress + _offsets.ScreenBrightness, value);
}

void NieRHook::setMusicVolume(int value)
{
    writeMemory(this->_baseAddress + _offsets.MusicVolume, value);
}

void NieRHook::setSoundEffectVolume(int value)
{
    writeMemory(this->_baseAddress + _offsets.SoundEffectVolume, value);
}

void NieRHook::setVoiceVolume(int value)
{
    writeMemory(this->_baseAddress + _offsets.VoiceVolume, value);
}

void NieRHook::setAudioOutput(int value)
{
    writeMemory(this->_baseAddress + _offsets.AudioOutput, value);
}

void NieRHook::setVoiceChanger(int value)
{
    writeMemory(this->_baseAddress + _offsets.VoiceChanger, value);
}

void NieRHook::setHorizontalRotationSpeed(int value)
{
    writeMemory(this->_baseAddress + _offsets.HorizontalRotationSpeed, value);
}

void NieRHook::setVerticalRotationSpeed(int value)
{
    writeMemory(this->_baseAddress + _offsets.VerticalRotationSpeed, value);
}

void NieRHook::setDistance(int value)
{
    writeMemory(this->_baseAddress + _offsets.Distance, value);
}

void NieRHook::setCombatDistance(int value)
{
    writeMemory(this->_baseAddress + _offsets.CombatDistance, value);
}

void NieRHook::setZoomSpeed(int value)
{
    writeMemory(this->_baseAddress + _offsets.ZoomSpeed, value);
}

void NieRHook::setVerticalAutoAdjust(int value)
{
    writeMemory(this->_baseAddress + _offsets.VerticalAutoAdjust, value);
}

void NieRHook::setHorizontalAutoAdjust(int value)
{
    writeMemory(this->_baseAddress + _offsets.HorizontalAutoAdjust, value);
}

void NieRHook::setFreeEnemyTracking(int value)
{
    writeMemory(this->_baseAddress + _offsets.FreeEnemyTracking, value);
}

void NieRHook::setPursuitSpeed(int value)
{
    writeMemory(this->_baseAddress + _offsets.PursuitSpeed, value);
}

void NieRHook::setLockedEnemyTracking(int value)
{
    writeMemory(this->_baseAddress + _offsets.LockedEnemyTracking, value);
}

int NieRHook::getMusicVolume()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.MusicVolume);
}

int NieRHook::getSoundEffectVolume()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.SoundEffectVolume);
}
int NieRHook::getVoiceVolume()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.VoiceVolume);
}

int NieRHook::getAudioOutput()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.AudioOutput);
}

int NieRHook::getVoiceChanger()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.VoiceChanger);
}

int NieRHook::getBrightness()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.ScreenBrightness);
}

int NieRHook::getDistance()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.Distance);
}

int NieRHook::getCombatDistance()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.CombatDistance);
}

int NieRHook::getVerticalRotationSpeed()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.VerticalRotationSpeed);
}

int NieRHook::getHorizontalRotationSpeed()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.HorizontalRotationSpeed);
}

int NieRHook::getHorizontalAutoAdjust()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.HorizontalAutoAdjust);
}

int NieRHook::getVerticalAutoAdjust()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.VerticalAutoAdjust);
}

int NieRHook::getFreeEnemyTracking()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.FreeEnemyTracking);
}

int NieRHook::getZoomSpeed()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.ZoomSpeed);
}

int NieRHook::getPursuitSpeed()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.PursuitSpeed);
}

int NieRHook::getLockedEnemyTracking()
{
    return readMemory<int>(this->_baseAddress + this->_offsets.LockedEnemyTracking);
}

float NieRHook::getHUDOpacity()
{
    return readMemory<float>(this->_baseAddress + this->_offsets.HUDOpacity);
}
