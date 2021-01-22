#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h>
#include <vector>
#include "NierHook.hpp"
#include "Player.hpp"
#include <thread>

int main()
{
	NieRHook hook;
	while (true) {
		hook.start();
	}
	return (0);
}