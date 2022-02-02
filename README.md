# NieR:Automata Hook

A C++ library to attach to the NieR:Automata process and read/write memory.

[NieRHook Demo](#nierhook-demo) <br>
[Using NieRHook](#using-nierhook) <br>
[Build NieRHook](#build-nierhook) <br>
[Memory Reference](#memory-reference)<br>
[NieRHook Reference](#nierhook-reference)

## NieRHook Demo

```
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include "NierHook.hpp"
using namespace std;

//Function used to exit the program
void ENDPressed(NieRHook* hook) {
	while (true) {
		if (GetKeyState(VK_END) & 0x8000) //END button pressed
		{
			//Disable cheats before exiting
			hook->InfiniteDoubleJump(false);
			hook->IgnoreUpgradeMaterials(false);
			//Stop hook
			hook->stop();
			return;//exit function
		}
	}
}

/*This is a showcase program of the hook
* As NieR:Automata is a x64 program, you must compile this solution in x64.
*/
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//Look for memory leaks

	NieRHook hook;
	cout << "Hooking..." << endl;
	//Hook to process
	while (!hook.isHooked()) {
		hook.start();
	}
	cout << "Hooked" << endl;

	//Enable some cheats
	hook.InfiniteDoubleJump(true);
	hook.IgnoreUpgradeMaterials(true);

	//Add some items
	//For ID reference please visit github.com/Asiern/NieRHook Readme
	hook.addItem(510, 80); //Beast Hide ID => 510
	hook.addItem(0, 10); //Small Recovery => ID 0
	hook.addItem(721, 1); //Data chip B => ID 721
	hook.addItem(400, 99); //E-Drug => ID 400

	//Add some weapons
	hook.addWeapon(0x4D8, 1); //Type-40 Blade => ID 0x4D8
	hook.addWeapon(0x41A, 1); //Type-40 Sword => ID 0x41A

	//Create a thread to exit when the 'END' button is pressed
	std::thread exitThread(ENDPressed, &hook);

	//Print some values
	while (hook.isHooked()) {
		hook.update();//update hook internal values (Position, Health, Level...)
		std::cout << "X: " << hook.getXPosition() << "  Y: " << hook.getYPosition() << "  Z: " << hook.getYPosition() << std::endl;
		std::cout << "Health: " << hook.getHealth() << std::endl;
		std::cout << "Press END to exit..." << std::endl;
		Sleep(500);
		system("cls");
	}

	//Join thread and exit
	exitThread.join();
	return (0);
}
```

---

## Using NieRHook

### CMake Projects

### Other projects

---

## Build NieRHook

---

## Memory Reference

You can find all the used IDs and offsets [here](https://docs.google.com/spreadsheets/d/1zowU8VOamVJcsLZni7T-5OaiT8iZAEM3YYEdtVP1F8k/edit?usp=sharing)

---

## NieRHook Reference

#### Methods

- `Start` - attach the hook to `NieRAutomata.exe` process
- `Stop` - detach the hook from process
- `isHooked` - return true if hooked
- `isSaveLoaded` - returns true if a savefile is loaded, otherwise false
- `hookStatus` - ensures the status of the hook, if processID changes stops the hook
- `getProcessID` - returns process ID
- `update` - refresh hook & attributes
- `getLevel` - return player's level
- `getHealth` - returns player's health
- `getMaxHealth` - returns player's max health
- `getFunds` - returns player's Funds(G)
- `getEXP` - returns player's EXP
- `getXPosition` - returns player's X position
- `getYPosition` - returns player's Y position
- `getZPosition` - returns player's Z position
- `getLoadedSaveName` - returns the name of the loaded savefile
- `setPosition` - sets the position of the player
- `setHealth` - sets player health
- `NoClip` - enables or disables No Clip
- `NoCooldown` - enables or disables No Cooldown
- `InfiniteDoubleJump` - enables or disables Infinite Double Jump
- `InfiniteAirDash` - enables or disables Infinite Air Dash
- `IgnoreUpgradeMaterials` - enables or disables Ignore Upgrade Materials
- `FreeCamera` - enables or disables free camera movement
- `setGameSpeed` - sets game speed
- `addItem` - Adds a specific quantity of items to the inventory (See item IDs at [Memory Reference](#memory-reference))
- `removeItem` -Removes item from memory (See item IDs at [Memory Reference](#memory-reference))
- `addWeapon` - Adds a weapon to the inventory (See weapon IDs at [Memory Reference](#memory-reference))
- `removeWeapon` - removes a weapon from the inventory (See weapon IDs at [Memory Reference](#memory-reference))
- `setHUDOpacity` - sets the opacity of the HUD
- `setColor` - sets RGBColor combination
