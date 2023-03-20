/**
 * @file NierHook.cpp
 * @author Asiern (asiern.dev@gmail.com)
 * @brief NieR:Automata Memory Hook
 * @version 0.1
 * @date 2023-03-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "NierHook.hpp"

/**
 * @brief Search for window named "NieR:Automata"
 *
 * @return DWORD process ID
 */
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

/**
 * @brief Find modules in NieR:Automata process
 *
 * @param procId process id
 * @param modName name of module
 * @return uintptr_t memory address of module
 */
uintptr_t NieRHook::_getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32FirstW(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap); // Close handle to prevent memory leaks
    return modBaseAddr;
}

/**
 * @brief Fill memory with custom values
 *
 * @param destination destination address
 * @param src source address
 * @param size size
 */
void NieRHook::_patch(BYTE* destination, BYTE* src, unsigned int size)
{
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
    DWORD oldprotection;
    VirtualProtectEx(pHandle, destination, size, PAGE_EXECUTE_READWRITE, &oldprotection);
    WriteProcessMemory(pHandle, destination, src, size, nullptr);
    VirtualProtectEx(pHandle, destination, size, oldprotection, &oldprotection);
    CloseHandle(pHandle);
}

/**
 * @brief Private function to enable a cheat. This replaces the default byte array in memory.
 *
 * @param c
 */
void NieRHook::_enableCheat(cheat* c)
{
    _patch((BYTE*)this->_baseAddress + c->offset, c->enabled, c->size);
}

/**
 * @brief Private function to diable a cheat. This restores the default byte array in memory.
 *
 * @param c
 */
void NieRHook::_disableCheat(cheat* c)
{
    _patch((BYTE*)this->_baseAddress + c->offset, c->disabled, c->size);
}

/**
 * @brief Checkis if NieR:Automata process is still running. If process is not running stops hook.
 *
 */
void NieRHook::hookStatus(void)
{
    if (this->_pID != this->_getProcessID())
    {
        this->stop();
    }
}

/**
 * @brief Get game version
 *
 * @return int game version
 */
int NieRHook::getVersion()
{
    return this->version;
}

/**
 * @brief Check if savefile is loaded
 *
 * @return true
 * @return false
 */
bool NieRHook::isSavefileLoaded(void)
{
    char* loaded = (char*)readMemoryString(this->_baseAddress + this->_offsets.savefiles.loaded,
                                           this->_offsets.savefiles.nameSize);
    bool result = strcmp(loaded, "") != 0;
    free(loaded);
    return result;
}

/**
 * @brief Updates hook attributes
 *
 */
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

/**
 * @brief Get value for X position
 *
 * @return float
 */
float NieRHook::getXPosition(void)
{
    return this->Xpos;
}

/**
 * @brief Get value for Y position
 *
 * @return float
 */
float NieRHook::getYPosition(void)
{
    return this->Ypos;
}

/**
 * @brief Get value for Z position
 *
 * @return float
 */
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
        _enableCheat(&this->_offsets.NoCooldown);
    else
        _disableCheat(&this->_offsets.NoCooldown);
}

/**
 * @brief Writes to memory a byte array to change op codes and enable infinite air dash cheat.
 *
 * @param enabled boolean indicating desirred status. True for enabled and false for disabled.
 */
void NieRHook::InfiniteAirDash(bool enabled)
{
    if (enabled)
        _enableCheat(&this->_offsets.InfiniteAirDash);
    else
        _disableCheat(&this->_offsets.InfiniteAirDash);
}

void NieRHook::IgnoreUpgradeMaterials(bool enabled)
{
    if (enabled)
    { // Enable Ignore Upgrade Materials
        _enableCheat(&this->_offsets.PodMaterials);
        _enableCheat(&this->_offsets.WeaponMaterials);
    }
    else
    { // Disable Ignore Upgrade Materials
        _disableCheat(&this->_offsets.PodMaterials);
        _disableCheat(&this->_offsets.WeaponMaterials);
    }
}

void NieRHook::NoCLip(bool enabled)
{
    switch (this->version)
    {
    case VER_1_0_1:
        if (enabled)
        {
            _enableCheat(&this->_offsets.NoClipX);
            _enableCheat(&this->_offsets.NoClipY);
        }
        else
        {
            _disableCheat(&this->_offsets.NoClipX);
            _disableCheat(&this->_offsets.NoClipY);
        }
        break;
    case VER_1_0_2:
        if (enabled)
        {
            _enableCheat(&this->_offsets.NoClipX);
            // this->noClipWorkerHandle = CreateThread(NULL, 0, NieRHook::_noClipWorker(), NULL, 0, NULL);
            // std::cout << this->noClipWorkerHandle << std::endl;
        }
        else
        {
            _disableCheat(&this->_offsets.NoClipX);
            // CloseHandle(this->noClipWorkerHandle);
        }
        break;
    }
}

void NieRHook::InfiniteDoubleJump(bool enabled)
{
    if (enabled)
        _enableCheat(&this->_offsets.InfiniteDoubleJump);
    else
        _disableCheat(&this->_offsets.InfiniteDoubleJump);
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
        _enableCheat(&this->_offsets.InfiniteItemUsage);
    else
        _disableCheat(&this->_offsets.InfiniteItemUsage);
}

/**
 * @brief Set A2 type dash
 *
 * @param enabled
 */
void NieRHook::SetA2Dash(bool enabled)
{
    if (enabled)
    {
        _enableCheat(&this->_offsets.SetA2DashTypeSFX);
        _enableCheat(&this->_offsets.SetA2DashType);
        _enableCheat(&this->_offsets.SetA2DashInvisibility);
        _enableCheat(&this->_offsets.SetA2DashDistance);
        _enableCheat(&this->_offsets.SetA2DashVisualFX);
    }
    else
    {
        _disableCheat(&this->_offsets.SetA2DashTypeSFX);
        _disableCheat(&this->_offsets.SetA2DashType);
        _disableCheat(&this->_offsets.SetA2DashInvisibility);
        _disableCheat(&this->_offsets.SetA2DashDistance);
        _disableCheat(&this->_offsets.SetA2DashVisualFX);
    }
}

/**
 * @brief Set 2B type dash
 *
 * @param enabled
 */
void NieRHook::Set2BDash(bool enabled)
{
    if (enabled)
    {
        _enableCheat(&this->_offsets.Set2BDashTypeSFX);
        _enableCheat(&this->_offsets.Set2BDashType);
        _enableCheat(&this->_offsets.Set2BDashInvisibility);
        _enableCheat(&this->_offsets.Set2BDashDistance);
        _enableCheat(&this->_offsets.Set2BDashVisualFX);
    }
    else
    {
        _disableCheat(&this->_offsets.Set2BDashTypeSFX);
        _disableCheat(&this->_offsets.Set2BDashType);
        _disableCheat(&this->_offsets.Set2BDashInvisibility);
        _disableCheat(&this->_offsets.Set2BDashDistance);
        _disableCheat(&this->_offsets.Set2BDashVisualFX);
    }
}

/**
 * @brief Infinite Buff duration cheat.
 *
 * @param enabled
 */
void NieRHook::InfiniteBuffDuration(bool enabled)
{
    if (enabled)
        _enableCheat(&this->_offsets.InfiniteBuffDuration);
    else
        _disableCheat(&this->_offsets.InfiniteBuffDuration);
}

/**
 * @brief Save Anywhere cheat. Makes user able to save game far from savepoints.
 *
 * @param enabled
 */
void NieRHook::SaveAnywhere(bool enabled)
{
    if (enabled)
        _enableCheat(&this->_offsets.SaveAnywhere);
    else
        _disableCheat(&this->_offsets.SaveAnywhere);
}

/**
 * @brief Get infinite uses of consumables
 *
 * @param enabled
 */
void NieRHook::InfiniteConsumableItems(bool enabled)
{
    if (enabled)
    {
        _enableCheat(&this->_offsets.InfiniteConsumableItems);
        _enableCheat(&this->_offsets.InfiniteConsumableSupportItems);
    }
    else
    {
        _disableCheat(&this->_offsets.InfiniteConsumableItems);
        _disableCheat(&this->_offsets.InfiniteConsumableSupportItems);
    }
}

/**
 * @brief  Add item to inventory.
 * If Item is not in the inventory, Creates a new Item on memory
 *
 * @param ID item id
 * @param number items quantity
 * @return true if successful
 * @return false if not success
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
    return true;
}

/**
 * @brief Remove item from inventory
 *
 * @param ID item ID
 * @return true if removed
 * @return false if not removed. This could be cause by an error or the item might not be in the inventory.
 */
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

/**
 * @brief Removes weapon from inventory
 *
 * @param ID weapon ID
 * @return true if removed
 * @return false if not removed.m This could be cause by an error or the weapon is not in the inventory.
 */
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

/**
 * @brief Read memory and return a list of the inventory
 *
 * @return std::vector<inventoryItem>
 */
std::vector<inventoryItem> NieRHook::readInventory(void)
{
    std::vector<inventoryItem> items = std::vector<inventoryItem>();
    uintptr_t addr = this->_baseAddress + this->_offsets.items_first;
    int currentID;
    inventoryItem currentItem;
    if (!_hooked)
        return items;

    while (addr < this->_baseAddress + this->_offsets.items_last)
    {
        currentID = readMemory<int>(addr);
        if (currentID != 0xffffffff)
        {
            currentItem.id = currentID;
            currentItem.quantity = readMemory<int>(addr + 0x8);
            items.push_back(currentItem);
        }
        addr += 0xC;
    }
    return items;
}

/**
 * @brief Get weapon list from memory
 *
 * @return std::vector<inventoryWeapon>
 */
std::vector<inventoryWeapon> NieRHook::readWeapons(void)
{
    std::vector<inventoryWeapon> weapons = std::vector<inventoryWeapon>();
    uintptr_t addr = this->_baseAddress + this->_offsets.weapons_first;
    int currentID;
    inventoryWeapon currentItem;
    if (!_hooked)
        return weapons;

    while (addr < this->_baseAddress + this->_offsets.weapons_last)
    {
        currentID = readMemory<int>(addr);
        if (currentID != 0xffffffff)
        {
            currentItem.id = currentID;
            currentItem.level = readMemory<int>(addr + 0x4);
            weapons.push_back(currentItem);
        }
        addr += 0x14;
    }
    return weapons;
}

/**
 * @brief Read string from memory region
 *
 * @param address address to read
 * @param size number of bytes to read
 * @return char* read string
 */
char* NieRHook::readMemoryString(uintptr_t address, int size)
{
    char* val = (char*)calloc(sizeof(char) * size, size);
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->_pID);
    ReadProcessMemory(pHandle, (LPCVOID)address, val, size, NULL);
    CloseHandle(pHandle); // Close handle to prevent memory leaks
    return val;
}

/**
 * @brief Get game version
 *
 */
void NieRHook::getGameVersion()
{
    // Check for v1.0.2
    char* version = readMemoryString(this->_baseAddress + 0x1422130, 14);
    if (strcmp(version, "version v0.0.2") == 0)
    {
        this->version = VER_1_0_2;
        free(version);
        return;
    }

    // Check for v1.0.1
    char version2[8];
    char* res;
    for (int i = 0; i < 7; i++)
    {
        res = readMemoryString(this->_baseAddress + 0x6557790 + (i * 2), 1);
        version[i] = *res;
        free(res);
    }
    version2[7] = '\0';
    if (strcmp(version, "1.0.0.0") == 0)
    {
        this->version = VER_1_0_1;
        free(version);
        return;
    }

    // No version found
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

void NieRHook::start(int version)
{
    // Obtain process ID
    DWORD ID = this->_pID;
    while (ID <= 0)
    {
        ID = this->_getProcessID();
    }
    this->_pID = ID;

    // Get "NieRAutomata.exe" module base address
    this->_baseAddress = this->_getModuleBaseAddress(ID, L"NieRAutomata.exe");

    // Game version
    if (version == AUTO)
    {
        this->getGameVersion();
        if (this->version == 0)
            return;
    }
    else
        this->version = version;

    switch (this->version)
    {
    case VER_1_0_2:
        this->_offsets = {};

        // Game
        this->_offsets.GameSpeed;
        this->_offsets.version = 0x1422130;

        // SaveFiles
        this->_offsets.savefiles.loaded = 0x13EADB0;
        this->_offsets.savefiles.slot1 = 0x145BAA8;
        this->_offsets.savefiles.slot2 = 0x145BB08;
        this->_offsets.savefiles.slot3 = 0x145BB68;
        this->_offsets.savefiles.nameSize = 32;

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
        this->_offsets.NoClipX.offset = 0x4E1290;
        this->_offsets.NoClipX.disabled = (BYTE*)"\x48\x8B\xC4";
        this->_offsets.NoClipX.enabled = (BYTE*)"\xC3\x90\x90";
        this->_offsets.NoClipX.size = 3;
        this->_offsets.NoClipY.offset = 0x4E1290;
        this->_offsets.NoClipY.disabled = (BYTE*)"\x48\x8B\xC4";
        this->_offsets.NoClipY.enabled = (BYTE*)"\xC3\x90\x90";
        this->_offsets.NoClipY.size = 3;
        this->_offsets.InfiniteDoubleJump.offset = 0x47E257;
        this->_offsets.InfiniteDoubleJump.enabled = (BYTE*)"\xFF\x0F\x8C";
        this->_offsets.InfiniteDoubleJump.disabled = (BYTE*)"\x02\x0f\x8D";
        this->_offsets.InfiniteDoubleJump.size = 3;
        // TODO this->_offsets.NoCooldown;
        this->_offsets.InfiniteAirDash.offset = 0x47E391;
        this->_offsets.InfiniteAirDash.enabled = (BYTE*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        this->_offsets.InfiniteAirDash.disabled = (BYTE*)"\xC7\x83\x98\x0A\x01\x00\x01\x00\x00\x00";
        this->_offsets.InfiniteAirDash.size = 10;
        this->_offsets.WeaponMaterials.offset = 0x976F72;
        this->_offsets.WeaponMaterials.disabled = (BYTE*)"\x85\xC0";
        this->_offsets.WeaponMaterials.enabled = (BYTE*)"\x31\xC8";
        this->_offsets.WeaponMaterials.size = 2;
        // TODO this->_offsets.PodMaterials;
        // TODO this->_offsets.FreeCamera;
        this->_offsets.InfiniteItemUsage.offset = 0x7C9D82;
        this->_offsets.InfiniteItemUsage.enabled = (BYTE*)"\x90\x90\x90";
        this->_offsets.InfiniteItemUsage.disabled = (BYTE*)"\x89\x70\x08";
        this->_offsets.InfiniteItemUsage.size = 3;

        // Dash offsets
        this->_offsets.DashOptions.soundFX = 0x4B1145;
        this->_offsets.DashOptions.type = 0x4B18F2;
        this->_offsets.DashOptions.invisibility = 0x43A7D0;
        this->_offsets.DashOptions.distanceType = 0x4DA57E;
        this->_offsets.DashOptions.visualFX = 0x4B1D5F;

        // Set A2 Dash type
        this->_offsets.SetA2DashTypeSFX.disabled = (BYTE*)"\x8B\x8B\xB8\x05\x00\x00";
        this->_offsets.SetA2DashTypeSFX.enabled = (BYTE*)"\xB9\x00\x01\x01\x00\x90";
        this->_offsets.SetA2DashTypeSFX.offset = this->_offsets.DashOptions.soundFX;
        this->_offsets.SetA2DashTypeSFX.size = 6;

        this->_offsets.SetA2DashType.disabled = (BYTE*)"\x8B\x8B\xB8\x05\x00\x00";
        this->_offsets.SetA2DashType.enabled = (BYTE*)"\xB9\x00\x01\x01\x00\x90";
        this->_offsets.SetA2DashType.offset = this->_offsets.DashOptions.type;
        this->_offsets.SetA2DashType.size = 6;

        this->_offsets.SetA2DashInvisibility.disabled = (BYTE*)"\x8B\x91\xB8\x05\x00\x00";
        this->_offsets.SetA2DashInvisibility.enabled = (BYTE*)"\xBA\x00\x01\x01\x00\x90";
        this->_offsets.SetA2DashInvisibility.offset = this->_offsets.DashOptions.invisibility;
        this->_offsets.SetA2DashInvisibility.size = 6;

        this->_offsets.SetA2DashDistance.disabled = (BYTE*)"\x8B\x83\xB8\x05\x00\x00";
        this->_offsets.SetA2DashDistance.enabled = (BYTE*)"\xB8\x00\x01\x01\x00\x90";
        this->_offsets.SetA2DashDistance.offset = this->_offsets.DashOptions.distanceType;
        this->_offsets.SetA2DashDistance.size = 6;

        this->_offsets.SetA2DashVisualFX.disabled = (BYTE*)"\x8B\x83\xB8\x05\x00\x00";
        this->_offsets.SetA2DashVisualFX.enabled = (BYTE*)"\xB8\x00\x01\x01\x00\x90";
        this->_offsets.SetA2DashVisualFX.offset = this->_offsets.DashOptions.visualFX;
        this->_offsets.SetA2DashVisualFX.size = 6;

        // Set 2B Dash type
        this->_offsets.Set2BDashTypeSFX.disabled = (BYTE*)"\x8B\x8B\xB8\x05\x00\x00";
        this->_offsets.Set2BDashTypeSFX.enabled = (BYTE*)"\xB9\x00\x00\x01\x00\x90";
        this->_offsets.Set2BDashTypeSFX.offset = this->_offsets.DashOptions.soundFX;
        this->_offsets.Set2BDashTypeSFX.size = 6;

        this->_offsets.Set2BDashType.disabled = (BYTE*)"\x8B\x8B\xB8\x05\x00\x00";
        this->_offsets.Set2BDashType.enabled = (BYTE*)"\xB9\x00\x00\x01\x00\x90";
        this->_offsets.Set2BDashType.offset = this->_offsets.DashOptions.type;
        this->_offsets.Set2BDashType.size = 6;

        this->_offsets.Set2BDashInvisibility.disabled = (BYTE*)"\x8B\x91\xB8\x05\x00\x00";
        this->_offsets.Set2BDashInvisibility.enabled = (BYTE*)"\xBA\x00\x00\x01\x00\x90";
        this->_offsets.Set2BDashInvisibility.offset = this->_offsets.DashOptions.invisibility;
        this->_offsets.Set2BDashInvisibility.size = 6;

        this->_offsets.Set2BDashDistance.disabled = (BYTE*)"\x8B\x83\xB8\x05\x00\x00";
        this->_offsets.Set2BDashDistance.enabled = (BYTE*)"\xB8\x00\x00\x01\x00\x90";
        this->_offsets.Set2BDashDistance.offset = this->_offsets.DashOptions.distanceType;
        this->_offsets.Set2BDashDistance.size = 6;

        this->_offsets.Set2BDashVisualFX.disabled = (BYTE*)"\x8B\x83\xB8\x05\x00\x00";
        this->_offsets.Set2BDashVisualFX.enabled = (BYTE*)"\xB8\x00\x00\x01\x00\x90";
        this->_offsets.Set2BDashVisualFX.offset = this->_offsets.DashOptions.visualFX;
        this->_offsets.Set2BDashVisualFX.size = 6;

        // Infinite buff duration
        this->_offsets.InfiniteBuffDuration.offset = 0x519DC0;
        this->_offsets.InfiniteBuffDuration.disabled = (BYTE*)"\x48\x8B\xC4";
        this->_offsets.InfiniteBuffDuration.enabled = (BYTE*)"\xC3\x90\x90";
        this->_offsets.InfiniteBuffDuration.size = 3;

        // Save Anywhere
        this->_offsets.SaveAnywhere.offset = 0x38C7C7;
        this->_offsets.SaveAnywhere.disabled = (BYTE*)"\x0F\x94\xC0";
        this->_offsets.SaveAnywhere.enabled = (BYTE*)"\xB0\x01\x90";
        this->_offsets.SaveAnywhere.size = 3;

        // Infinite consumable items
        this->_offsets.InfiniteConsumableItems.offset = 0x84410C;
        this->_offsets.InfiniteConsumableItems.disabled = (BYTE*)"\x41\xB8\xFF\xFF\xFF\xFF";
        this->_offsets.InfiniteConsumableItems.enabled = (BYTE*)"\x45\x31\xC0\x90\x90\x90";
        this->_offsets.InfiniteConsumableItems.size = 6;

        this->_offsets.InfiniteConsumableSupportItems.offset = 0x7D604D;
        this->_offsets.InfiniteConsumableSupportItems.disabled = (BYTE*)"\x41\xB8\xFF\xFF\xFF\xFF";
        this->_offsets.InfiniteConsumableSupportItems.enabled = (BYTE*)"\x45\x31\xC0\x90\x90\x90";
        this->_offsets.InfiniteConsumableSupportItems.size = 6;

        break;

    case VER_1_0_1:
        this->_offsets = {};

        // Game
        this->_offsets.GameSpeed = 0x160E6D8;
        this->_offsets.version = 0x6557790;

        // SaveFiles
        this->_offsets.savefiles.loaded = 0x18E9E00;
        this->_offsets.savefiles.slot1 = 0x194BAA8;
        this->_offsets.savefiles.slot2 = 0x194BB08;
        this->_offsets.savefiles.slot3 = 0x194BB68;
        this->_offsets.savefiles.nameSize = 16;

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
        // TODO this->_offsets.FreeCamera;
        this->_offsets.InfiniteItemUsage.offset = 0x7C9D82;
        this->_offsets.InfiniteItemUsage.enabled = (BYTE*)"\x90\x90\x90";
        this->_offsets.InfiniteItemUsage.disabled = (BYTE*)"\x89\x70\x08";
        this->_offsets.InfiniteItemUsage.size = 3;
        break;
    }

    this->_hooked = true;
}

void NieRHook::start(void)
{
    start(AUTO);
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
