#pragma once
#include "CoreMinimal.h"
#include <array>
#include "MyWeapon.h"
#include "MyAmmo.h"
#include "MyGrenadeBomb.h"
#include "MyExplosiveActor.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "MyCharacter.generated.h"

class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UAnimMontage;
class UParticleSystem;
class USoundCue;
class UWidgetComponent;
class AMyWeaponBox;
class UGameplayStatics;
class UCurveVector;
class UCurveFloat;

enum class EMoveXState : uint8;
enum class EMoveYState : uint8;
enum class EMoveAddState : uint8;
enum class EActionState : uint8;
enum class EFireModeState : uint8;
enum class EWeaponState : uint8; 
enum class EHUDAimState : uint8; 
enum class EDamage : uint8;
enum class EMovieActionState : uint8;

// HUD Info Data Table
USTRUCT(BlueprintType)
struct FHUDInfoDataTable : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName textInfo;
};

UCLASS()
class WRAITH_PATH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Base
	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION() void OnGroundBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnWeaponboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnWeaponboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// States
	FORCEINLINE EMoveXState GetMoveXState() const { return baseMoveXState; }
	FORCEINLINE EMoveYState GetMoveYState() const { return baseMoveYState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EMoveAddState GetMoveAddState() const { return addMoveState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EWeaponState GetWeaponState() const { return weaponState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EActionState GetActionState() const { return actionState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetNoneAddMoveState() { addMoveState = EMoveAddState::ES_None; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetNoneActionState() { actionState = EActionState::ES_None; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EHUDAimState GetCrosshairState() const { return crosshairState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EMovieActionState GetMovieActionState() { return movieState; }
	
	// Targeting
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetRocketLaunchTargeting(bool isLaunchTargeting)  { isRocketLaunchTargeting = isLaunchTargeting; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsRocketLaunchTargeting()  { return isRocketLaunchTargeting; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsWeaponTargeting() const { return isWeaponTargeting; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetMinigunRollerDelay() const { return minigunRollerDelay; }
	
	// Weapon Config Opts
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsPistolLaserEnable() { return isPistolLaserEnable; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4ScopeLaserEnable() { return isAR4ScopeLaserEnable; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4ScopeOpticalEnable() { return isAR4ScopeOpticalEnable; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4SuppressorEnable() { return isAR4SuppressorEnable; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsPistolLaserActive() { return isPistolLaserActive; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4ScopeLaserActive() { return isAR4ScopeLaserActive; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4ScopeOpticalActive() { return isAR4ScopeOpticalActive; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAR4SuppressorActive() { return isAR4SuppressorActive; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetPistolLaserActive(bool isActive) { isPistolLaserActive = isActive; accessoireNum |= PISTOL_LASER_ACTIVE_MAKE; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetAR4ScopeLaserActive(bool isActive) { isAR4ScopeLaserActive = isActive; isAR4ScopeOpticalActive = !isActive; accessoireNum |= AR4_LASER_ACTIVE_MAKE; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetAR4ScopeOpticalActive(bool isActive) { isAR4ScopeOpticalActive = isActive; isAR4ScopeLaserActive = !isActive; accessoireNum |= AR4_OPTICAL_ACTIVE_MAKE; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetAR4SuppressorActive(bool isActive) { isAR4SuppressorActive = isActive; accessoireNum |= AR4_SUPPR_ACTIVE_MAKE; }
	
	// Climbing
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsReadyToClimb() const { return isReadyToClimbLadder; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsReadyToClimb(bool isReady) { isReadyToClimbLadder = isReady; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsReadyToLeaveLadder() const { return isReadyToLeaveLadder; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsReadyToLeaveLadder(bool isReady) { isReadyToLeaveLadder = isReady; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetClimbVelZ() { return climbVelZ; }

	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsThrowEnable() const { return isThrowGrenadeEnable; }
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetHealth() const { return health; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsRidingSnowboard() const { return isRidingSnowboard; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsRidingSnowboard(bool isRiding) { isRidingSnowboard = isRiding; }

	// Camera Interpolating
	UFUNCTION(BlueprintCallable) void SetCameraInterpStartRocket();
	void SetCameraInterpBackRocket();
	void SetCameraInterpEndRocket();
	void CameraInterpRocket(float DeltaTime);

	// Interact With Pickup-Items
	FORCEINLINE void SetOverlappingWeapon(AMyWeapon* _pickedWeapon) { pickedWeapon = _pickedWeapon; }
	FORCEINLINE void SetOverlappingAmmo(AMyAmmo* _pickedAmmo) { pickedAmmo = _pickedAmmo; }
	FORCEINLINE void EquipPistol(AMyWeapon* pistolWeapon) { weapons[0] = currentWeapon = pistolWeapon; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetCanPickAmmo(FString ammoName);
	UFUNCTION(BlueprintCallable) FORCEINLINE uint8 GetMagicNum() { return accessoireNum; } // :)
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsNeedToInformAboutAccessoire() { return isNeedToInformAboutAccessoire; }

	// For HUD
	UFUNCTION() void UpdateCrosshairState();
	UFUNCTION(BlueprintCallable) FORCEINLINE AMyWeapon* GetCurrentWeapon() const { return currentWeapon; }
	UFUNCTION(BlueprintCallable) FORCEINLINE AMyWeapon* GetWeaponByIndex(int32 idx) const { return weapons[idx]; }
	UFUNCTION(BlueprintCallable) FORCEINLINE TArray<FString> GetTextFoundNewItemForHUD() const { return textFoundForHUD; }
	UFUNCTION(BlueprintCallable) FORCEINLINE TArray<FString> GetTextPickAmmoForHUD() const {  return textPickForHUD; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FString GetTextHeadshotForHUD() const { return textHeadshotForHUD; }
	FORCEINLINE void SetTextDiscoveredNewLevelForHUD(FString levelEvent, FString levelName) { 
		if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::FromHex("#00FF00FF"), FString("Set te to HUD"), false); }
		textFoundForHUD[0] = levelEvent; textFoundForHUD[1] = levelName; 
	}
	UFUNCTION(BlueprintCallable) FORCEINLINE FString GetTextTaskForHUD() const { return textTaskForHUD; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FString GetTextActionForHUD() const { return textActionForHUD; }
	FORCEINLINE void SetTextTaskForHUD(FString taskStr) { textTaskForHUD = taskStr; }
	FORCEINLINE void SetTextActionForHUD(FString actionStr) { textActionForHUD = actionStr; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsAutosaveAnimating() const { return isAutosaveAnimating; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsAutosaveAnimating(bool isAnim) { isAutosaveAnimating = isAnim; }

	// XP
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetCurrentXP() const { return XPCurrent; }
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetCurrentXPLevel() const { return XPLevel; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void IncreaseCurrentXP(float incrValue) { XPCurrent += incrValue; }
	FORCEINLINE int32 GetHealthMax() const { return healthMax; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsDead() const { return isDead; }
	UFUNCTION(BlueprintCallable) void SetDamage(int32 deltaHealth, bool isDamageByEnemy);
	void OnEndDamage();
	FORCEINLINE int32 GetDamageByGrenadeBomb() const { return damageByGrenadeBomb; }
	FORCEINLINE int32 GetDamageByExplosive() const { return damageByExplosive; }
	FORCEINLINE int32 GetDamageByEnemy() const { return damageByEnemy; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsPlayerDamaged(bool isDamaged) { isDamage = isDamaged; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsPlayerDamaged() { return isDamage; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsPlayerValnurable(bool isNewVulnarable) { isVulnarable = isNewVulnarable; }
	UFUNCTION(BlueprintCallable) void OnReloadEnd();
	UFUNCTION(BlueprintCallable) void OnEndOpenBox();

	// Skeletal Component Moving
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsMagazineMoves() { return isMagazineMoves; }
	UFUNCTION(BlueprintCallable) FORCEINLINE USceneComponent* GetMagazineComponent() { return magazineComponent; }
	// Autosave
	UFUNCTION(BlueprintCallable) FORCEINLINE FVector GetLastAutosaveLocation() { return lastAutosaveLocation; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetLastAutosaveLocation(FVector newSavedloc) { lastAutosaveLocation = newSavedloc; }	
	// Arm Power
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsArmPowerTimeDilating() { return isArmPowerTimeDilating; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsArmPowerTimeDilating(bool isDilate) { isArmPowerTimeDilating = isDilate; }

	// Access To Open Doors
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetHasCardCanion() { return hasCardCanion; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetHasCardScifi() { return hasCardScifi; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetHasCardSnow() { return hasCardSnow; }

	// Call Elevator
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetCanCallElevator() { return canCallElevator; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetCanCallElevator(bool canCall) { canCallElevator = canCall; }

	// Widget Menu
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsPersWidgetShown() { return isPersWidgetShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsPersWidgetShown(bool isShown) { isPersWidgetShown = isShown; }
	UFUNCTION(BlueprintCallable) void WeaponConfigure();
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsWeaponConfigure() { return isWeaponConfig; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsWeaponConfigure(bool isConfig) { isWeaponConfig = isConfig; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsGameMenuShown() { return isGameMenuShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsGameMenuShown(bool isShown) { isGameMenuShown = isShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsMainMenuShown() { return isMainMenuShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsMainMenuShown(bool isShown) { isMainMenuShown = isShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsSettingsMenuShown() { return isSettingsMenuShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsSettingsMenuShown(bool isShown) { isSettingsMenuShown = isShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsRespawnMenuShown() { return isRespawnMenuShown; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsRespawnMenuShown(bool isShown) { isRespawnMenuShown = isShown; }

	// Movie Action
	UFUNCTION(BlueprintCallable) void OnMovieActionBegin();
	UFUNCTION(BlueprintCallable) void OnMovieActionPeak();
	UFUNCTION(BlueprintCallable) void OnMovieActionEnd();
	void UpdateMovieActionFOV(float DeltaTime);
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsSpaceShipLanded() { return isSpaceshipLanded; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsSpaceShipLanded(bool isLanded) { isSpaceshipLanded = isLanded; }

	float runTime;
protected:
	// Base Params
	int32 health;
	int32 healthMax;
	float XPCurrent;
	int32 XPLevel;
	bool isFirstEquip;
	bool isVulnarable;
	TMap<FString, bool> pickedItems; // Pickups Collection
	AMyWeaponBox* currentWeaponbox; // Weapon Box Interact
	std::array<AMyWeapon*, 5> weapons;	// Weapon Arsenal
	
	// HUD
	TArray<FString> textFoundForHUD;
	TArray<FString> textPickForHUD;
	FString textHeadshotForHUD;
	FString textTaskForHUD;
	FString textActionForHUD;
	bool isAutosaveAnimating;

	// Current Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	AMyWeapon* currentWeapon;

	// Active Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Widget", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* currentWidget;

	// Add Action Params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	float armForceRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	float armImpulseStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	float armForceStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Climb", meta = (AllowPrivateAccess = "true"))
	float MaxStepHeightClimbing;

	// Access Cards To Open Doors
	bool hasCardCanion;
	bool hasCardScifi;
	bool hasCardSnow;

	// Interacts
	bool canCallElevator;

	// Camera Interp (Rocket)
	float oldCameraFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | Rocket", meta = (AllowPrivateAccess = "true"))
	float speedCameraFOVInterpRocket = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | Rocket", meta = (AllowPrivateAccess = "true"))
	float deltaCameraFOVInterpRocket = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | Rocket", meta = (AllowPrivateAccess = "true"))
	float deltaTimeStartCameraFOVInterpRocket = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | Rocket", meta = (AllowPrivateAccess = "true"))
	float deltaTimeEndCameraFOVInterpRocket = 0.5;
	bool isCameraFOVInterpRocket;
	bool isCameraFOVInterpRocketBack;
	FTimerHandle timerCameraFOVRocket;

	// Weapon Config
	bool isWeaponConfig;

	// Time Dilation (ArmPower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | ArmPower", meta = (AllowPrivateAccess = "true"))
	float timeDilation = 0.2f;

	// Camera Interp (ArmPower)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | ArmPower", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* curveCameraFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | ArmPower", meta = (AllowPrivateAccess = "true"))
	UCurveVector* curveCameraHandleMov;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Camera | ArmPower", meta = (AllowPrivateAccess = "true"))
	UCurveVector* curveCameraHandleRot;
	bool isArmPowerTimeDilating;
	FTimerHandle timerCameraArmPower;

	// Components
	UCharacterMovementComponent* movementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* myCameraRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* myCamera;
	UPROPERTY(VisibleInstanceOnly)
	AMyWeapon* pickedWeapon;
	UPROPERTY(VisibleInstanceOnly)
	AMyAmmo* pickedAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* p_DomeShield;
	UPROPERTY(EditAnyWhere, Category = "Spawning Grenade")
	TSubclassOf<class AMyGrenadeBomb> GrenadeToSpawn;

	// Enum States
	EMoveXState baseMoveXState;
	EMoveYState baseMoveYState;
	EMoveAddState addMoveState;
	EActionState actionState;
	EWeaponState weaponState;

	// Collision With Ground
	UPROPERTY(EditAnywhere)
	float collisionGroundBoxHeight = 200;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* collisionGroundBox;

	// Arm Power Radius
	UPROPERTY(EditAnywhere)
	float radiusArmShockWave = 300.f;

	// Menu
	bool isGameMenuShown;
	bool isMainMenuShown;
	bool isPersWidgetShown;
	bool isSettingsMenuShown;
	bool isRespawnMenuShown;

	// MovieAction
	FTimerHandle timerMovieAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Movie", meta = (AllowPrivateAccess = "true"))
	float cameraMovieZoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Movie", meta = (AllowPrivateAccess = "true"))
	float targetingMovieSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Movie", meta = (AllowPrivateAccess = "true"))
	float targetingMovieBeginDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Movie", meta = (AllowPrivateAccess = "true"))
	float targetingMoviePeakDelay;
	EMovieActionState movieState;
	float camRootXDefault;
	float camRootXCur;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Movie", meta = (AllowPrivateAccess = "true"))
	float camRootXDelta;
	bool isSpaceshipLanded;

	// Montages
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageClimb;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageCrouch;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageJump;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageAction;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageFire;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageGrenade;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageExtra;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageChangeWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageReloadWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageHideWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageDeath;
	UPROPERTY(EditDefaultsOnly, Category = "Params | Montages")
	UAnimMontage* montageRespawn;

	// Damage
	int32 damageByGrenadeBomb;
	int32 damageByExplosive;
	int32 damageByEnemy;

	// RMB Aim
	bool isWeaponTargeting;
	float cameraFOVDefault;
	float cameraFOVCurrent;
	float cameraFOVZoom;
	
	// Is Firing
	bool isFiring;
	int32 countMaxBulletsToSend;
	bool isRocketLaunchTargeting;

	// is Climbing
	bool isReadyToClimbLadder;
	bool isReadyToLeaveLadder; 
	float climbVelZ;

	// Is Grenade Throw Enable
	bool isThrowGrenadeEnable;
	bool isThrowBombEnable;
	
	// Is Riding The Snowboard
	bool isRidingSnowboard;
	
	// Damage
	bool isDamage;
	bool isDead;

	// Minigun Roller Rotation
	bool isMinigunRollerRotates = false;
	float minigunRollerSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Minigun", meta = (AllowPrivateAccess = "true"))
	float minigunRollerDelta = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Minigun", meta = (AllowPrivateAccess = "true"))
	float minigunRollerMax = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Minigun", meta = (AllowPrivateAccess = "true"))
	float minigunRollerDelay = 0.8f;

	// Weapon Accessoires
	bool isPistolLaserEnable;
	bool isAR4ScopeLaserEnable;
	bool isAR4ScopeOpticalEnable;
	bool isAR4SuppressorEnable;
	bool isPistolLaserActive;
	bool isAR4ScopeLaserActive;
	bool isAR4ScopeOpticalActive;
	bool isAR4SuppressorActive;

	// Weapon Base Opts
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* magazineComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	FTransform transformMagazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	bool isMagazineMoves;

	// Fire Tracing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* muzzleParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* traceParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* targetParticles;
	// Fire Ulta Tracing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* muzzleUltaParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* traceUltaParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* targetUltaParticles;
	// Fire Helper Vars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Debug", meta = (AllowPrivateAccess = "true"))
	float fireDistTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Debug", meta = (AllowPrivateAccess = "true"))
	bool isDebugFireTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Weapon", meta = (AllowPrivateAccess = "true"))
	float targetingSpeed;

	// Grenades & Bombs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Grenade & Bombs", meta = (AllowPrivateAccess = "true"))
	int32 countGrenades;
	int32 countGrenadesMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Grenade & Bombs", meta = (AllowPrivateAccess = "true"))
	int32 countBombs;
	int32 countBombsMax;

	// Current Ammo Available To Reload
	int32 deltaAmmoEnable;

	// Timers
	FTimerHandle timerEmoCaboom;     
	FTimerHandle timerFireRifle;
	FTimerHandle timerFireStartMinigun;
	FTimerHandle timerDomeShield;		
	FTimerHandle timerUltraVision;			
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle timerArmShockWave;
	FTimerHandle timerPickWeapon;
	FTimerHandle timerPickAmmo;
	FTimerHandle timerGrenadeAmin;
	FTimerHandle timerBombAnim;
	FTimerHandle timerReload;
	FTimerHandle timerClimb;
	FTimerHandle timerResetTextFound;
	FTimerHandle timerResetTextPick;
	FTimerHandle timerResetTextHeadshot;
	FTimerHandle timerDamageBegin;
	FTimerHandle timerDeathBegin;

	// Time To Reactivate Actions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltaReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltaDeactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeArmShockWaveReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeArmShockWaveDeactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeDomeShieldReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeDomeShieldDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltraVisionReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltraVisionDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeArmShockWaveElapsed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltaElapsed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeDomeShieldElapsed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float timeUltraVisionElapsed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Grenade & Bombs", meta = (AllowPrivateAccess = "true"))
	float timeGrenadeAnimReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Grenade & Bombs", meta = (AllowPrivateAccess = "true"))
	float timeBombAnimReactivation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	float timeEquip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	float timeReloadDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Climb", meta = (AllowPrivateAccess = "true"))
	float timeClimbBegin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Climb", meta = (AllowPrivateAccess = "true"))
	float timeClimbEnd;
	
	// Extra Skills
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isUltaEnable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isArmShockEnable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isUltraVisionEnable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isUltraVisionDeactivated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isDomeShieldEnable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Action", meta = (AllowPrivateAccess = "true"))
	bool isDomeShieldDeactivated;

	// Dome Shield Params
	float deltaMovement;
	UPROPERTY(EditAnywhere, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float shieldMovAmpl;
	UPROPERTY(EditAnywhere, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float shieldMovPhase;
	UPROPERTY(EditAnywhere, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"));
	float shieldRotAmpl;
	UPROPERTY(EditAnywhere, Category = "Params | Extra Skills", meta = (AllowPrivateAccess = "true"))
	float shieldRotPhase;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* fireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* fireUltaSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* headshotSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* killSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* effortSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* ultaSound;

	// HUD
	EHUDAimState crosshairState;

	// Last AutoSave Location
	FVector lastAutosaveLocation;

	// Check New Accessoires
	bool isNeedToInformAboutAccessoire;
	uint8 accessoireNum;

	virtual void BeginPlay() override;

	// Movement
	virtual void Jump() override;
	virtual void NotifyJumpApex() override;
	void JumpLanding();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	void CrouchBegin();
	void CrouchEnd();
	UFUNCTION(BlueprintCallable) void ClimbBegin();
	UFUNCTION(BlueprintCallable) void ClimbEnd();
	void OnClimbBegin();
	void OnClimbEnd();

	// Camera Actions
	UFUNCTION(BlueprintCallable) void ArmPowerCameraChangeStart();
	void ArmPowerCameraChanging();
	void ArmPowerCameraChangeEnd();

	// Action
	void DoAction();
	void Grenade();
	void Bomb();
	void ReloadBegin();
	void Ulta();
	void ArmShockWave();
	UFUNCTION(BlueprintCallable) void OnArmShockWaveStart();
	UFUNCTION(BlueprintCallable) void ApplyShockWaveStart();
	UFUNCTION(BlueprintCallable) void LaunchRocket();
	void UltraVision();
	void DomeShield();
	void OnUltraVisionDeactivated();
	void OnDomeShieldDeactivated();
	void OnArmShockWaveDeactivated();
	void OnUltaReactivated();
	void OnUltaDeactivated();
	void OnArmShockWaveReactivated();
	void OnUltraVisionReactivated();
	void OnDomeShieldReactivated();
	void OnGrenadeAnimReactivated();
	void OnBombAnimReactivated();
	void OnPickedAmmoBegin();
	void OnPickedAmmoEnd();
	UFUNCTION(BlueprintCallable) void Respawn();
	void OnDeathBegin();

	// HUD
	void Menu();
	void OnTextFoundResetEnd();
	void OnTextPickResetEnd();
	void OnTextHeadshotResetEnd();
	void OnTextActionResetEnd();

	// Weapon Manips
	void Fire();
	void CheckFiring();
	void StartFire();
	void StartFireMinigun();
	void FirePressed();
	void FireReleased();
	void Targeting();
	void TargetingStop();
	void FireMode();
	UFUNCTION(BlueprintCallable) void OnOpenBox();
	UFUNCTION(BlueprintCallable) void ChangeWeapon(FString weaponName, bool isAfterAction);
	void ChooseWeaponOne();
	void ChooseWeaponTwo();
	void ChooseWeaponThree();
	void ChooseWeaponFour();
	void ChooseWeaponFive();
	void HideWeapon();
	UFUNCTION(BlueprintCallable) void OnHideWeapon();
	UFUNCTION(BlueprintCallable) void OnUnHideWeapon();
	void OnPickedWeaponBegin();
	void OnPickedWeaponEnd();
	UFUNCTION(BlueprintCallable) void OnHandleMagazineBegin();
	UFUNCTION(BlueprintCallable) void OnHandleMagazineEnd();
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetMinigunRollerSpeed() const { return minigunRollerSpeed; }
};