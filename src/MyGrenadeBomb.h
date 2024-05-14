#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGrenadeBomb.generated.h"

class USphereComponent;
class USoundCue;
class UParticleSystem;
class AMyCharacter;
class AMyEnemyCharacter;

UCLASS()
class WRAITH_PATH_API AMyGrenadeBomb : public AActor
{
	GENERATED_BODY()
public:	
	AMyGrenadeBomb();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void SetGrenadeBombThrowPhysics(bool isPhysics, bool isGravity, ECollisionEnabled::Type collType, bool isBlock);
	FORCEINLINE USkeletalMeshComponent* GetRootMeshSkeletal() const { return rootMeshSkeletalComponent; }
	UFUNCTION() void SetGrenadeThrowParams(float _timeToDisarm, float _timeToExplode, FVector _throwForce);
	void SetGrenadeVisible();
	void OnGrenadeDisarm();
	void OnGrenadeExploded();
	FORCEINLINE FTimerHandle& GetGrenadeTimerVisibility() { return timerVisibility; }
protected:
	UPROPERTY(EditAnywhere, Category = "Actors To Find") TSubclassOf<class AMyEnemyCharacter> enemyActorClass;
	UPROPERTY(EditAnywhere, Category = "Actors To Find") TSubclassOf<class AMyCharacter> heroActorClass;
	virtual void BeginPlay() override;
private:
	USkeletalMeshComponent* rootMeshSkeletalComponent;
	USkeletalMesh* rootMeshSkeletal;
	USphereComponent* sphereInteract;
	FTimerHandle timerVisibility;
	FTimerHandle timerDisarm;
	FTimerHandle timerExplosion;
	float timeToExplode;
	float timeToDisarm;
	float radiusDamage;
	FVector throwForce;
	USoundCue* explosionSound;
	UParticleSystem* explosionParticles;
};