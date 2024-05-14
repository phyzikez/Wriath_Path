#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyTrigger.generated.h"

class UBoxComponent;
class USoundCue;
class UWidgetComponent;
class AMyCharacter;

UCLASS()
class WRAITH_PATH_API AMyTrigger : public AActor
{
	GENERATED_BODY()
public:	
	AMyTrigger();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetCountShotsCurrent() const { return countShotsCurrent; }
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetCountShotsRequired() const { return countShotsRequired; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsDamageOnlyByRocket() const { return isDamageByRocketOnly; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void IncrementCountShotsCurrent() { ++countShotsCurrent; if (countShotsCurrent >= countShotsRequired) isTriggerActive = false; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsTriggerActivated() { return !isTriggerActive; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Params", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* boxCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* widget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	USoundCue* sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	bool isDamageByRocketOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	int32 countShotsRequired;
	bool isTriggerActive;
	int32 countShotsCurrent;
	FTimerHandle timerSelfDestoy;
	virtual void BeginPlay() override;
};
