#pragma once
#include "CoreMinimal.h"

// BASE MOVES
UENUM(BlueprintType)
enum class EMoveXState : uint8 {
	ES_Idle UMETA(DisplayName = "Idle"),
	ES_MoveForward UMETA(DisplayName = "MoveForward"),
	ES_MoveBack UMETA(DisplayName = "MoveBack")
};

UENUM(BlueprintType)
enum class EMoveYState : uint8 {
	ES_Idle UMETA(DisplayName = "Idle"),
	ES_MoveLeft UMETA(DisplayName = "MoveLeft"),
	ES_MoveRight UMETA(DisplayName = "MoveRight")
};

// ADDITIONAL MOVES
UENUM(BlueprintType)
enum class EMoveAddState : uint8 {
	ES_None UMETA(DisplayName = "None"),
	ES_JumpStart UMETA(DisplayName = "JumpStart"),
	ES_JumpFalling UMETA(DisplayName = "JumpFalling"),
	ES_JumpEnd UMETA(DisplayName = "JumpEnd"),
	ES_Crouch UMETA(DisplayName = "Crouch"),
	ES_ClimbStart UMETA(DisplayName = "ClimbStart"),
	ES_Climbing UMETA(DisplayName = "Climbing"),
	ES_ClimbEnd UMETA(DisplayName = "ClimbEnd")
};

// ACTIONS
UENUM(BlueprintType)
enum class EActionState : uint8 {
	ES_None UMETA(DisplayName = "None"),
	ES_Fire UMETA(DisplayName = "Fire"),
	ES_Ulta UMETA(DisplayName = "Ulta"),
	ES_Grenade UMETA(DisplayName = "Grenade"),
	ES_Bomb UMETA(DisplayName = "Bomb"),
	ES_Reload UMETA(DisplayName = "Reload"),
	ES_DoAction UMETA(DisplayName = "DoAction"),
	ES_DomeShield UMETA(DisplayName = "DomeShield"),
	ES_ArmShockWave UMETA(DisplayName = "ArmShockWave"),
	ES_UltraVision UMETA(DisplayName = "UltraVision")
};

// FIRE MODE
UENUM(BlueprintType)
enum class EFireModeState : uint8 {
	ES_Single UMETA(DisplayName = "Single"),
	ES_Triple UMETA(DisplayName = "Triple"),
	ES_Semi UMETA(DisplayName = "Semi"),
	ES_Full UMETA(DisplayName = "Full")
};

// WEAPON TYPE
UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	ES_One UMETA(DisplayName = "One"),
	ES_Two UMETA(DisplayName = "Two"),
	ES_Three UMETA(DisplayName = "Three"),
	ES_Four UMETA(DisplayName = "Four"),
	ES_Five UMETA(DisplayName = "Five")
};

// AMMO TYPE
UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	ES_AmmoPistol UMETA(DisplayName = "AmmoPistol"),
	ES_AmmoSMG11 UMETA(DisplayName = "AmmoSMG11"),
	ES_AmmoAR4 UMETA(DisplayName = "AmmoAR4"),
	ES_AmmoSniper UMETA(DisplayName = "AmmoSniper"),
	ES_AmmoMinigun UMETA(DisplayName = "AmmoMinigun"),
	ES_AmmoGrenade UMETA(DisplayName = "AmmoGrenade"),
	ES_AmmoBomb UMETA(DisplayName = "AmmoBomb"),
	ES_AmmoHealth UMETA(DisplayName = "AmmoHealth"),
	ES_AmmoLaserPistol UMETA(DisplayName = "AmmoLaserPistol"),
	ES_AmmoAR4Scope UMETA(DisplayName = "AmmoAR4Scope"),
	ES_AmmoAR4Sight UMETA(DisplayName = "AmmoAR4Sight"),
	ES_AmmoAR4Suppressor UMETA(DisplayName = "AmmoAR4Suppressor"),
	ES_AmmoCardCanion UMETA(DisplayName = "AmmoCardCanion"),
	ES_AmmoCardScifi UMETA(DisplayName = "AmmoCardScifi"),
	ES_AmmoCardSnow UMETA(DisplayName = "AmmoCardSnow")
};

// EXPLOSION TYPE
UENUM(BlueprintType)
enum class EExplosionType : uint8 {
	ES_ExplosiveSimple UMETA(DisplayName = "ExplosiveSimple"),
	ES_ExplosiveArmed UMETA(DisplayName = "ExplosiveArmed")
};

// ENEMY COMBAT TYPE
UENUM(BlueprintType)
enum class EEnemyCombatType : uint8 {
	ES_Defender UMETA(DisplayName = "Defender"),
	ES_Combat UMETA(DisplayName = "Combat"),
	ES_SWAT UMETA(DisplayName = "SWAT"),
	ES_Beast UMETA(DisplayName = "Beast")
};

// ENEMY CHARACTER TYPE
UENUM(BlueprintType)
enum class EEnemyCharacterType : uint8 {
	ES_BanditCanyon1 UMETA(DisplayName = "BanditCanyon1"),
	ES_BanditCanyon2 UMETA(DisplayName = "BanditCanyon2"),
	ES_BanditCave UMETA(DisplayName = "BanditCave"),
	ES_WraithCave UMETA(DisplayName = "WraithCave"),
	ES_WraithScifi UMETA(DisplayName = "WraithScifi"),
	ES_WraithSWAT UMETA(DisplayName = "WraithSWAT"),
	ES_FemaleScifi UMETA(DisplayName = "FemaleScifi"),
	ES_FemaleSWAT UMETA(DisplayName = "FemaleSWAT"),
	ES_CaveBeast UMETA(DisplayName = "CaveBeast"),
	ES_HellBeast UMETA(DisplayName = "HellBeast")
};

// ENEMY STATE
UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	ES_Idle UMETA(DisplayName = "Idle"),
	ES_Patrol UMETA(DisplayName = "Patrol"),
	ES_Chasing UMETA(DisplayName = "Chasing"),
	ES_Attack UMETA(DisplayName = "Attack"),
	ES_Dead UMETA(DisplayName = "Dead"),
	ES_Salute UMETA(DisplayName = "Salute")
};

// DAMAGE TYPE
UENUM(BlueprintType)
enum class EDamage : uint8 {
	ES_ArmLeg UMETA(DisplayName = "ArmLeg"),
	ES_Core UMETA(DisplayName = "Core"),
	ES_Head UMETA(DisplayName = "Head"),
	ES_GrenadeBomb UMETA(DisplayName = "GrenadeBomb"),
	ES_Rocket UMETA(DisplayName = "Rocket"),
	ES_ArmShockwave UMETA(DisplayName = "ArmShockwave"),
	ES_Explosive UMETA(DisplayName = "Explosive"),
	ES_HeavyMass UMETA(DisplayName = "HeavyMass")
};

// HUD AIM STATE
UENUM(BlueprintType)
enum class EHUDAimState : uint8 {
	ES_Regular UMETA(DisplayName = "Regular"),
	ES_Target UMETA(DisplayName = "Target"),
	ES_Sniper UMETA(DisplayName = "Sniper")
};

// PICKED ITEM TYPES
UENUM(BlueprintType)
enum class EPickedType : uint8 {
	ES_Ammo UMETA(DisplayName = "Ammo"),
	ES_Medicine UMETA(DisplayName = "Medicine"),
	ES_Explosive UMETA(DisplayName = "Explosive"),
	ES_Weapon UMETA(DisplayName = "Weapon"),
	ES_Accessoire UMETA(DisplayName = "Accessoire"),
	ES_Card UMETA(DisplayName = "Card")
};

// MOVIE CAMERA ACTION STATES
UENUM(BlueprintType)
enum class EMovieActionState : uint8 {
	ES_Start UMETA(DisplayName = "Start"),
	ES_Peak UMETA(DisplayName = "Peak"),
	ES_End UMETA(DisplayName = "End")
};

// -------------- HELPERS -------------
void PrintMyVector(FVector v, FString msg, float time = 2.f, FColor color = FColor::Red);
void PrintMyVector(FVector v1, FVector v2, FString txt1, FString txt2, float time = 2.f, FColor color = FColor::Red);
void PrintMyVector(FVector v1, FVector v2, FVector v3, FString txt1, FString txt2, FString txt3, float time = 2.f, FColor color = FColor::Red);

// -------------- BITS FOR WEAPON ACCESSOIRE HUD TEXT CHECK --------------
const uint8 PISTOL_LASER_ACTIVE_MAKE = 128;
const uint8 AR4_LASER_ACTIVE_MAKE = 31;
const uint8 AR4_OPTICAL_ACTIVE_MAKE = 8;
const uint8 AR4_SUPPR_ACTIVE_MAKE = 2;

const uint8 PISTOL_LASER_ENABLE_MAKE = 64;
const uint8 AR4_LASER_ENABLE_MAKE = 16;
const uint8 AR4_OPTICAL_ENABLE_MAKE = 4;
const uint8 AR4_SUPPR_ENABLE_MAKE = 1;

const uint8 PISTOL_LASER_MASK = 63;
const uint8 AR4_LASER_MASK = 207;
const uint8 AR4_OPTICAL_MASK = 243;
const uint8 AR4_SUPPR_MASK = 252;

const uint8 PISTOL_LASER_RES = 127;
const uint8 AR4_LASER_RES = 223;
const uint8 AR4_OPTICAL_RES = 247;
const uint8 AR4_SUPPR_RES = 253;