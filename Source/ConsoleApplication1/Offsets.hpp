#include <Windows.h>
#pragma once

struct offsets {

	//Player
	uintptr_t entity;
	uintptr_t health;
	uintptr_t maxHealth;
	uintptr_t x;
	uintptr_t y;
	uintptr_t z;
	uintptr_t level;
	uintptr_t exp;
	uintptr_t funds;

	//Items
	uintptr_t items_first;
	uintptr_t items_last;

	//Weapons
	uintptr_t weapons_first;
	uintptr_t weapons_last;

	//Misc
	uintptr_t MusicVolume;
	uintptr_t SoundEffectVolume;
	uintptr_t VoiceVolume;
	uintptr_t AudioOutput;
	uintptr_t VoiceChanger;
	uintptr_t ScreenBrightness;
	uintptr_t Distance;
	uintptr_t CombatDistance;
	uintptr_t VerticalRotationSpeed;
	uintptr_t HorizontalRotationSpeed;
	uintptr_t HorizontalAutoAdjust;
	uintptr_t VerticalAutoAdjust;
	uintptr_t FreeEnemyTracking;
	uintptr_t ZoomSpeed;
	uintptr_t PursuitSpeed;
	uintptr_t LockedEnemyTracking;
	uintptr_t HUDOpacity;
	uintptr_t R;
	uintptr_t G;
	uintptr_t B;

	//Cheats
	uintptr_t NoClipX;
	uintptr_t NoClipY;
	uintptr_t InfiniteDoubleJump;
	uintptr_t GameSpeed;
	uintptr_t NoCooldown;
	uintptr_t InfiniteAirDash;
	uintptr_t WeaponMaterials;
	uintptr_t PodMaterials;
	uintptr_t FreeCamera;

};