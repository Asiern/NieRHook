#include "NierHook.hpp"
#include <TlHelp32.h>
#include <Windows.h>
#include <iostream>
#include <thread>
using namespace std;

// Function used to exit the program
void ENDPressed(NieRHook* hook)
{
    while (true)
    {
        if (GetKeyState(VK_END) & 0x8000) // END button pressed
        {
            // Disable cheats before exiting
            hook->InfiniteDoubleJump(false);
            hook->IgnoreUpgradeMaterials(false);
            // Stop hook
            hook->stop();
            return; // exit function
        }
    }
}

/*This is a showcase program of the hook
 * As NieR:Automata is a x64 program, you must compile this solution in x64.
 */
int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Look for memory leaks

    NieRHook hook;
    cout << "Hooking..." << endl;
    // Hook to process
    while (!hook.isHooked())
    {
        hook.start();
    }
    cout << "Hooked" << endl;

    // Enable some cheats
    hook.InfiniteDoubleJump(true);
    hook.IgnoreUpgradeMaterials(true);

    // Add some items
    // For ID reference please visit github.com/Asiern/NieRHook Readme
    hook.addItem(510, 80); // Beast Hide ID => 510
    hook.addItem(0, 10);   // Small Recovery => ID 0
    hook.addItem(721, 1);  // Data chip B => ID 721
    hook.addItem(400, 99); // E-Drug => ID 400

    // Add some weapons
    hook.addWeapon(0x4D8, 1); // Type-40 Blade => ID 0x4D8
    hook.addWeapon(0x41A, 1); // Type-40 Sword => ID 0x41A

    // Create a thread to exit when the 'END' button is pressed
    std::thread exitThread(ENDPressed, &hook);

    // Print some values
    while (hook.isHooked())
    {
        Sleep(500);
        system("cls");
        switch (hook.getVersion())
        {
        case VER_0_0_1:
            std::cout << "version 0.0.1" << std::endl;
            break;
        case VER_0_0_2:
            std::cout << "version 0.0.2" << std::endl;
            break;
        default:
            break;
        }
        if (!hook.isSavefileLoaded())
        {
            std::cout << "Loaded Savefile: NONE" << std::endl;
            continue;
        }

        hook.update(); // update hook internal values (Position, Health, Level...)
        std::cout << "X: " << hook.getXPosition() << "  Y: " << hook.getYPosition() << "  Z: " << hook.getYPosition()
                  << std::endl;
        std::cout << "Health: " << hook.getHealth() << std::endl;
        std::cout << "Press END to exit..." << std::endl;
    }

    // Join thread and exit
    exitThread.join();
    return 0;
}