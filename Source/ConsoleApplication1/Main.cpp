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