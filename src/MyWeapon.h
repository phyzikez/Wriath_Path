#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MyEStates.h"
#include "MyActor.h"
#include "Components/BoxComponent.h"
#include "MyWeapon.generated.h"

class AMyCharacter;
class USoundCue;

// Weapon Data Table
USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString weaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* weaponSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* weaponStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType ammoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fireFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speedWalk;

	// Sockets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName socketArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName socketFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName socketHandle;

	// Ammo Count
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ammoCurrentWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ammoCapacityWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ammoCurrentMagazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ammoCapacityMagazine;

	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cameraFOVZoom;

	// Fire Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireModeState fireModeBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireModeState fireModeAlt;

	// Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageValue;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* soundEquip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* soundFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* soundFireAlt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* soundReload;

	// Images
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* weaponIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* weaponCrosshair;

	// Material/Skins
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* materialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 materialIndex;

	// Animations
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;*/

	// Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* effectFireMuzzle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* effectFireTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* effectFireTrace;
};


// Weapon Class
UCLASS()
class WRAITH_PATH_API AMyWeapon : public AMyActor
{
	GENERATED_BODY()
public:	
	AMyWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void Equip(USceneComponent* ParentBind, FString SocketNameBind);
	void Drop(USceneComponent* ParentBind, FString SocketNameBind);
	void SetWeaponVisibility(bool isVisible);
	void SetWeaponPickupPhysics();

	// Getters
	FORCEINLINE USkeletalMeshComponent* GetWeaponSkeletalMesh() const { return rootSkeletalMesh; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetWeaponName() const { return weaponName; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAmmoType GetAmmoType() const { return ammoType; }
	FORCEINLINE float GetWeaponFireFrequency() const { return fireFrequency; }
	FORCEINLINE float GetWeaponFOV() const { return cameraFOVZoom; }
	FORCEINLINE float GetSpeedWalk() const { return speedWalk; }
	FORCEINLINE EFireModeState GetFireModeBase() const { return fireModeBase; }
	FORCEINLINE EFireModeState GetFireModeAlt() const { return fireModeAlt; }
	FORCEINLINE EFireModeState GetCurrentFireMode() const { return currentFireMode; }
	FORCEINLINE void ChangeFireMode() { currentFireMode = (currentFireMode == fireModeBase ? fireModeAlt : fireModeBase); }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetFireModeBase() { currentFireMode = fireModeBase; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetFireModeAlt() { currentFireMode = fireModeAlt; }
	FORCEINLINE UTexture2D* GetWeaponCrosshair() const { return weaponCrosshair; }
	FORCEINLINE int32 GetAmmoCurrentMagazine() const { return ammoCurrentMagazine; }
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetAmmoCurrentWeapon() const { return ammoCurrentWeapon; }
	FORCEINLINE void SetAmmoCurrentMagazine(int32 deltaAmmo) { ammoCurrentMagazine += deltaAmmo; }
	FORCEINLINE void SetAmmoCurrentWeapon(int32 deltaAmmo) { ammoCurrentWeapon -= deltaAmmo; }
	FORCEINLINE void ResetAmmoCurrentWeapon(int32 newCountAmmo) { ammoCurrentWeapon = newCountAmmo; }
	FORCEINLINE int32 GetAmmoCapacityMagazine() const { return ammoCapacityMagazine; }
	FORCEINLINE int32 GetAmmoCapacityWeapon() const { return ammoCapacityWeapon; }
	FORCEINLINE void DescreaseAmmoByOne() { --ammoCurrentMagazine; }
	FORCEINLINE int32 GetDamage() const { return damageValue; }

	// Delta Skeletal
	// CLIP (FIRE)
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetDeltaClipPistolX() const { return deltaClipXPislol; }
	FORCEINLINE float GetDeltaClipPistolXMax() const { return deltaClipXPislolMax; }
	FORCEINLINE void SetDeltaClipPistolX(float newX) { deltaClipXPislol = newX; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetDeltaClipSniperX() const { return deltaClipXSniper; }
	FORCEINLINE float GetDeltaClipSniperXMax() const { return deltaClipXSniperMax; }
	FORCEINLINE void SetDeltaClipSniperX(float newX) { deltaClipXSniper = newX; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FTimerHandle& GetTimerClip() { return clipMoveTimer; }
	FORCEINLINE void OnClipEnd() {}
	// SIGHT (RMB) 
	UFUNCTION(BlueprintCallable) FORCEINLINE FVector GetDeltaSightSniper() const { return deltaSightSniper; }
	FORCEINLINE FVector GetDeltaSightSniperDownLoc() const { return deltaSightSniperDownLoc; }
	FORCEINLINE FVector GetDeltaSightSniperUpperLoc() const { return deltaSightSniperUpperLoc; }
	FORCEINLINE void SetDeltaSightSniper(FVector newPos) { deltaSightSniper = newPos; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FVector GetDeltaSightLensSniper() const { return deltaSightLensSniper; }
	FORCEINLINE FVector GetDeltaSightLensSniperDownLoc() const { return deltaSightLensSniperDownLoc; }
	FORCEINLINE FVector GetDeltaSightLensSniperUpperLoc() const { return deltaSightLensSniperUpperLoc; }
	FORCEINLINE void SetDeltaSightLensSniper(FVector newPos) { deltaSightLensSniper = newPos; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FTimerHandle& GetTimerSightMoveUp() { return sightMoveUpTimer; }
	UFUNCTION(BlueprintCallable) FORCEINLINE FTimerHandle& GetTimerSightMoveDown() { return sightMoveDownTimer; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetSightMoveDT() { return sightMoveDT; }
	void OnSightMoveUpEnd();
	void OnSightMoveDownEnd();
	void OnSightLensEnd();
	UFUNCTION(BlueprintCallable) FORCEINLINE FTimerHandle& GetTimerSightLens() { return sightLensMoveTimer; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetSightLensMoveDT() { return sightLensMoveDT; }
	// ROLLER (MINIGUN)
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetDeltaRotXMinigun() const { return deltaRotXRollerMinigun; }
	FORCEINLINE float GetDeltaRotXMinigunMax() const { return deltaRotXRollerMinigunMax; }
	FORCEINLINE void SetDeltaRotXMinigun(float newRot) { deltaRotXRollerMinigun = newRot; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void ActivateWeaponBoxInteract() { boxInteract->SetGenerateOverlapEvents(true); }
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
														   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* rootSkeletalMesh;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* boxInteract;

	// TABLE DATA
	// Base Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	FString weaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* weaponSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* weaponStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	float fireFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	float speedWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Base", meta = (AllowPrivateAccess = "true"))
	float cameraFOVZoom;

	// Sockets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Sockets", meta = (AllowPrivateAccess = "true"))
	FName socketArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Sockets", meta = (AllowPrivateAccess = "true"))
	FName socketFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Sockets", meta = (AllowPrivateAccess = "true"))
	FName socketHandle;

	// TODO : Alternatively take params from Table/Curve
	// Delta Curve Anim Skelet Parts
	// PISTOL
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float deltaClipXPislol; // var
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float deltaClipXPislolMax;
	// SNIPER
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float deltaClipXSniper; // var
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float deltaClipXSniperMax;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightSniper; // var
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightSniperDownLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightSniperUpperLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float sightMoveDT;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightLensSniper; // var
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightLensSniperDownLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	FVector deltaSightLensSniperUpperLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float sightLensMoveDT;
	// MINIGUN
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Delta Skeleton", meta = (AllowPrivateAccess = "true"))
	float deltaRotXRollerMinigun;
	float deltaRotXRollerMinigunMax;

	// Timers
	FTimerHandle clipMoveTimer;
	FTimerHandle sightMoveUpTimer;
	FTimerHandle sightMoveDownTimer;
	FTimerHandle sightLensMoveTimer;
	
	// Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageValue;

	// Fire Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Fire Mode", meta = (AllowPrivateAccess = "true"))
	EFireModeState fireModeBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Fire Mode", meta = (AllowPrivateAccess = "true"))
	EFireModeState fireModeAlt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Fire Mode", meta = (AllowPrivateAccess = "true"))
	EFireModeState currentFireMode;

	// Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType ammoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 ammoCurrentMagazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 ammoCurrentWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 ammoCapacityMagazine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 ammoCapacityWeapon;

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* soundEquip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* soundFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* soundFireAlt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Audio", meta = (AllowPrivateAccess = "true"))
	USoundCue* soundReload;

	// Images
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Images", meta = (AllowPrivateAccess = "true"))
	UTexture2D* weaponIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Images", meta = (AllowPrivateAccess = "true"))
	UTexture2D* weaponCrosshair;

	// Material/Skins
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Materials", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* materialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Materials", meta = (AllowPrivateAccess = "true"))
	int32 materialIndex;

	// Animations
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAnimInstance> AnimBP;*/

	// Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* effectFireMuzzle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* effectFireTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params | Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* effectFireTrace;
};
