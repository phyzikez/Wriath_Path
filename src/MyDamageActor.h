#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <set>
#include "MyDamageActor.generated.h"

class UBoxComponent;
class USoundCue;
class UWidgetComponent;
class AMyCharacter;
class AMyEnemyCharacter;
class AMyTrigger;

UCLASS()
class WRAITH_PATH_API AMyDamageActor : public AActor
{
	GENERATED_BODY()
public:
	AMyDamageActor();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* boxCollision;
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
	std::set<AActor*> damagedActors;
	bool isDamageActivated;

	virtual void BeginPlay() override;
	UFUNCTION() virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnDestroyActor();
private:
	UStaticMeshComponent* rootMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Params", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* rootMesh;
};