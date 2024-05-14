#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "MyInteractBox.generated.h"

UCLASS()
class WRAITH_PATH_API AMyInteractBox : public AActor
{
	GENERATED_BODY()
public:	
	AMyInteractBox();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
													  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnTextShowEnd();
	void OnAutosaveEnd();

	bool isEnableToUseAgain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* boxInteract;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params", meta = (AllowPrivateAccess = "true"))
	FName keyName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Params", meta = (AllowPrivateAccess = "true"))
	FString valueText;

	FTimerHandle timerReset;

};
