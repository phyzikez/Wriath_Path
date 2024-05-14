#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDamageBreakableActor.generated.h"

class UBoxComponent;
class USoundCue;
class UWidgetComponent;
class AMyCharacter;
class AMyEnemyCharacter;
class UGeometryCollectionComponent;
class AMyTrigger;

UCLASS()
class WRAITH_PATH_API AMyDamageBreakableActor : public AActor
{
	GENERATED_BODY()
public:
	AMyDamageBreakableActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) FORCEINLINE float GetGeoMass() const { return mass; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	float mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	UUserWidget* widget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	AMyTrigger* triggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	USoundCue* sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	float lifeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* widgetComponent;
	FTimerHandle timerSelfDestoy;
	
	UPROPERTY(EditAnywhere, Category = "Actors To Find")
	TSubclassOf<class AMyEnemyCharacter> enemyActorClass;
	TSet<AActor*> damagedActors;
	bool isDamageActivated;

	virtual void BeginPlay() override;
	UFUNCTION() virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnDestroyActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	UGeometryCollectionComponent* geoCollection;
private:
};