# NieR:Automata Hook

A C++ library to attach to the NieR:Automata process and read/write memory.

[Using NieRHook](#using-nierhook) <br>
[NieRHook Reference](#nierhook-reference)

## Using NieRHook

```
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include "NierHook.hpp"

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
	NieRHook hook;
	//Hook to process
	while (!hook.isHooked()) {
		hook.start();
	}

	//Enable some cheats
	hook.InfiniteDoubleJump(true);
	hook.IgnoreUpgradeMaterials(true);

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

## NieRHook Reference

#### Methods

- `Start` - attach the hook to `NieRAutomata.exe` process
- `Stop` - detach the hook from process
- `isHooked` - return true if hooked
- `hookStatus` - ensures the status of the hook, if processID changes stops
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
- `setPosition` - sets the position of the player
- `setHealth` - sets player health
- `NoClip` - enables or disables No Clip
- `NoCooldown` - enables or disables No Cooldown
- `InfiniteDoubleJump` - enables or disables Infinite Double Jump
- `InfiniteAirDash` - enables or disables Infinite Air Dash
- `IgnoreUpgradeMaterials` - enables or disables Ignore Upgrade Materials
- `setGameSpeed` - sets game speed
