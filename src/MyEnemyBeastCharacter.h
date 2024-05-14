#pragma once
#include "CoreMinimal.h"
#include "MyEnemyCharacter.h"
#include "Engine/DataTable.h"
#include "MyWeapon.h"
#include "MyGrenadeBomb.h"
#include "MyCharacter.h"
#include "MyEStates.h"
#include "MyEnemyBeastCharacter.generated.h"

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
enum class EEnemyState : uint8;

UCLASS()
class WRAITH_PATH_API AMyEnemyBeastCharacter : public AMyEnemyCharacter
{
	GENERATED_BODY()
public:
	AMyEnemyBeastCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void AttackMain();
	virtual void AttackAlt();
protected:
	virtual void BeginPlay() override;
	UFUNCTION() void OnCollisionBoxLeftBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																				    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnCollisionBoxRightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnCollisionBoxLeftEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																				  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION() void OnCollisionBoxRightEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																				    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* leftWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params | Base", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* rightWeapon;
};
