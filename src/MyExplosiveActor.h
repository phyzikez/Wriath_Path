#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyExplosiveActor.generated.h"

class UBoxComponent;
class USphereComponent;
class UStaticMeshComponent;
class USoundCue;
class UAnimMontage;
class UParticleSystem;
class UWidgetComponent;
class AMyCharacter;
class AMyEnemyCharacter;
enum class EAmmoType : uint8;
enum class EExplosionType : uint8;

UCLASS()
class WRAITH_PATH_API AMyExplosiveActor : public AActor
{
	GENERATED_BODY()
public:
	AMyExplosiveActor();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	FORCEINLINE UFUNCTION() int32 GetCurrentDamage() const { return currentDamage; }
	FORCEINLINE UFUNCTION() EExplosionType GetBarrelType() const { return barrelType; }
	virtual void DamageBarrel(int32 damageValue);
	virtual void Explode();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Actors To Find")
	TSubclassOf<class AMyEnemyCharacter> enemyActorClass;
	UPROPERTY(EditAnywhere, Category = "Actors To Find")
	TSubclassOf<class AMyCharacter> heroActorClass;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Params", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* rootMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* barrelMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Params", meta = (AllowPrivateAccess = "true"))
	USphereComponent* collisionSphere;

	// Barrel Base Params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Base", meta = (AllowPrivateAccess = "true"))
	FString barrelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Base", meta = (AllowPrivateAccess = "true"))
	EExplosionType barrelType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Base", meta = (AllowPrivateAccess = "true"))
	int32 currentDamage;
	int32 maxDamage;

	// Barrel Damage Params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Damage", meta = (AllowPrivateAccess = "true"))
	float impulseStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Damage", meta = (AllowPrivateAccess = "true"))
	float forceStrength;

	// Barrel Additional Params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	FTimerHandle timer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* widget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	bool isWidgetActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* explosionParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* smokeParticles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Params | Additional", meta = (AllowPrivateAccess = "true"))
	USoundCue* explosionSound;
};
