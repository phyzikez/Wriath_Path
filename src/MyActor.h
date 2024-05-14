#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class WRAITH_PATH_API AMyActor : public AActor
{
	GENERATED_BODY()
public:
	AMyActor();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintPure) FORCEINLINE float GetAmpl() { return ampl; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetAmpl(float newAmpl) { ampl = newAmpl; }
	UFUNCTION(BlueprintPure) FORCEINLINE float GetPhase() { return phase; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetPhase(float newPhase) { phase = newPhase;  }
	UFUNCTION(BlueprintPure) FORCEINLINE float GetRotRate() { return rotRate; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetRotRate(float newRotRate) { rotRate = newRotRate; }
	UFUNCTION(BlueprintPure) FORCEINLINE bool CanMove() { return canMove; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetCanMove(bool isMoveEnable) { canMove = isMoveEnable; }
protected:
	bool canMove;
	float runTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Params", meta = (AllowPrivateAccess = "true"))
	float ampl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Params", meta = (AllowPrivateAccess = "true"))
	float phase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Params", meta = (AllowPrivateAccess = "true"))
	float rotRate;
private:
};