#ifndef _OFFSETS_H
#define _OFFSETS_H

#ifdef USING_WINSOCK2
#include <WinSock2.h>
#else
#include <Windows.h>
#endif;

typedef struct
{
    int size;
    uintptr_t offset;
    BYTE* enabled;
    BYTE* disabled;
} cheat;

typedef struct
{
    uintptr_t loaded;
    uintptr_t slot1;
    uintptr_t slot2;
    uintptr_t slot3;
    int nameSize;
} saves;

typedef struct
{
    uintptr_t soundFX;
    uintptr_t type;
    uintptr_t invisibility;
    uintptr_t distanceType;
    uintptr_t visualFX;
} dash;

struct offsets
{
    // Game
    uintptr_t version;
    uintptr_t GameSpeed;
    saves savefiles;

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
    dash DashOptions;
    cheat SetA2DashTypeSFX;
    cheat Set2BDashTypeSFX;
    cheat SetA2DashType;
    cheat Set2BDashType;
    cheat SetA2DashInvisibility;
    cheat Set2BDashInvisibility;
    cheat SetA2DashDistance;
    cheat Set2BDashDistance;
    cheat SetA2DashVisualFX;
    cheat Set2BDashVisualFX;
    cheat InfiniteBuffDuration;
};

#endif;