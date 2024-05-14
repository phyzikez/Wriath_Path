#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "MyWeapon.h"
#include "MyGrenadeBomb.h"
#include "MyCharacter.h"
#include "MyEStates.h"
#include "MyEnemyCharacter.generated.h"

class UBoxComponent;
class USphereComponent;
class UCapsuleComponent;
class UAnimMontage;
class UParticleSystem;
class USoundCue;
class UWidgetComponent;
class AMyCharacter;
class AMyEnemyAIController;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UPawnSensingComponent;

enum class EEnemyState : uint8;

// Enemy Data Table
USTRUCT(BlueprintType)
struct FEnemyDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyCharacterType enemyCharacterType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyCombatType enemyCombatType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* skeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString enemyName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 currentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType weaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* healthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByGunArmLeg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByGunCore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByGunHead;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByGrenadeBomb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByExplosive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByArmShockwave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageByHeavyMass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 damageHero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* fireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* fireAltMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* grenadeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* damageMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* deathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* saluteMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName socketWeaponFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName socketWeaponFireAlt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* muzzleParticles;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* muzzleAltParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* traceParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* bloodParticles;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AMyEnemyAIController* enemyAIController;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UBehaviorTree* behaviorTree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float timeToSelfDestroy;
};

UCLASS()
class WRAITH_PATH_API AMyEnemyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AMyEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Damage(EDamage damageType, float damageRadius = 0.0f, FVector hitLoc = FVector::ZeroVector, int32 damageBodypart = 0, FName damagedBoneName = "");
	UFUNCTION() void OnHit(int32 damageValue, EDamage damageBodyPart, FName damagedBoneName, FVector hitLoc = FVector::ZeroVector);
	UFUNCTION(BlueprintCallable) void OnSelfDestroy();
	UFUNCTION(BlueprintCallable) virtual void AttackRnd();
	UFUNCTION(BlueprintCallable) virtual void AttackMain();
	UFUNCTION(BlueprintCallable) virtual void AttackAlt();
	UFUNCTION(BlueprintCallable) void SendBullet(FName socketFireName);
	UFUNCTION(BlueprintCallable) void OnEndAttack();
	UFUNCTION(BlueprintCallable) void Salute();
private:
	UFUNCTION(BlueprintCallable) void Grenade();
	UFUNCTION(BlueprintCallable) void GrenadeDisarm();
	AMyGrenadeBomb* thrownGrenade;
public:
	FORCEINLINE FString GetEnemyName() const { return enemyName; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return behaviorTree; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetEnemyState(EEnemyState newState);
	UFUNCTION(BlueprintCallable) FORCEINLINE EEnemyState GetEnemyState() const { return enemyState; }
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetEnemyHealth() const { return currentHealth; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsStunning(bool isStun) { isStunning = isStun; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsStunning() const { return isStunning; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsFall(bool bFall) { isFall = bFall; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsFall() const { return isFall; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsPlayerInDeadZone() const { return isPlayerInDeadZone; }
	UFUNCTION(BlueprintCallable) FORCEINLINE UParticleSystem* GetEnemyBloodParticles() { return bloodParticles; }
	
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetYawAI() const { return yawAI; }
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetPitchAI() const { return pitchAI; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetYawAI(float newYaw) { yawAI = newYaw; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetPitchAI(float newPitch) { pitchAI = newPitch; }

	UFUNCTION(BlueprintImplementableEvent) void ShowDamageValue(float damVal, FVector hitLoc);

protected:
	virtual void BeginPlay() override;
	UFUNCTION() void OnCollisionSphereDetectBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
																				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnCollisionSphereDetectEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
																				 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION() void OnCollisionDeadZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnCollisionDeadZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PlayAttackMontage(UAnimMontage* resMontage, FString debugText = "ATTACK");
	//UFUNCTION() void PawnSeen(APawn* Pawn);

	UPROPERTY(EditAnyWhere, Category = "Spawning Grenade")
	TSubclassOf<class AMyGrenadeBomb> GrenadeToSpawn;

	// Enemy Base Params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	EEnemyCharacterType enemyCharacterType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	USphereComponent* detectSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	USphereComponent* deadHeroZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	EEnemyCombatType enemyCombatType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	FString enemyName;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	//UBoxComponent* collisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* skeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	int32 currentHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	EAmmoType weaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* widgetHealth;

	// AI Behaviour Distances
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Distances", meta = (AllowPrivateAccess = "true"))
	float distAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Distances", meta = (AllowPrivateAccess = "true"))
	float distChasing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Distances", meta = (AllowPrivateAccess = "true"))
	float distPatrol;

	// Enemy Inner Damage Koefs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByGunArmLeg;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByGunCore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByGunHead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByGrenadeBomb;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByExplosive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByArmShockwave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Inner Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageByHeavyMass;
	// Enemy Out Damage Koef
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Outer Damage", meta = (AllowPrivateAccess = "true"))
	int32 damageHero;
	
	// Enemy Montages
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* fireMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* fireAltMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* grenadeMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* damageMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* deathMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* saluteMontage;

	// AI
	AMyEnemyAIController* enemyAIController;
	UPROPERTY(EditAnywhere, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* behaviorTree;
	UPROPERTY(EditAnywhere, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FVector> pointsPatrol;
	UPROPERTY(EditAnywhere, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FVector> pointsHide;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	EEnemyState enemyState;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	bool isStunning;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	bool isFall;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	bool isPlayerInDeadZone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	float fireDistTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	float yawAI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | AI", meta = (AllowPrivateAccess = "true"))
	float pitchAI;

	UPROPERTY(VisibleAnywhere) UPawnSensingComponent* pawnSensing; // TODO ???

	// Timers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Timers", meta = (AllowPrivateAccess = "true"))
	FTimerHandle timerAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Timers", meta = (AllowPrivateAccess = "true"))
	FTimerHandle timerDeath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Params | Timers", meta = (AllowPrivateAccess = "true"))
	float timeToSelfDestroy;

	// Particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	FName socketWeaponFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	FName socketWeaponFireAlt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* muzzleParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* muzzleAltParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* traceParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* bloodParticles;
};
