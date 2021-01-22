
#include "Player.hpp"
class NieRHook;
class NieRHook {
private:
	//Attributes
	DWORD _pID;
	uintptr_t _baseAddress;
	uintptr_t _entityAddress;
	bool _hooked;
	Player player;

	//Methods
	void _hook(void);
	void _unHook(void);
	DWORD _getProcessID(void);
	uintptr_t _getModuleBaseAddress(DWORD procId, const wchar_t* modName);
	void _fetchData(void);
	void _hookStatus(void);

public:
	NieRHook();
	void start(void);
	void stop(void);
	uintptr_t getBaseAddress(void);
	DWORD getProcessID(void);
	Player* getPlayer();

};