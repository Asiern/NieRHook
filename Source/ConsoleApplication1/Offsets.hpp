#include <Windows.h>
#pragma once

typedef struct
{
    int size;
    uintptr_t offset;
    BYTE* enabled;
    BYTE* disabled;
} cheat;

struct offsets
{
    // Game
    uintptr_t version;
    uintptr_t GameSpeed;

    // Player
    uintptr_t entity;
    uintptr_t health;
    uintptr_t maxHealth;
    uintptr_t x;
    uintptr_t y;
    uintptr_t z;
    uintptr_t level;
    uintptr_t exp;
    uintptr_t funds;

    // Items
    uintptr_t items_first;
    uintptr_t items_last;

    // Weapons
    uintptr_t weapons_first;
    uintptr_t weapons_last;

    // Misc
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

    // Cheats
    cheat NoClipX;
    cheat NoClipY;
    cheat InfiniteDoubleJump;
    cheat NoCooldown;
    cheat InfiniteAirDash;
    cheat WeaponMaterials;
    cheat PodMaterials;
    cheat FreeCamera;
    cheat InfiniteItemUsage;
};