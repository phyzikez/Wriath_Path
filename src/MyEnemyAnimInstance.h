#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyEnemyAnimInstance.generated.h"

class AMyEnemyCharacter;
class UCharacterMovementComponent;

UCLASS()
class WRAITH_PATH_API UMyEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) void UpdateAnimParams(float DeltaTime);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Movement", meta = (AllowPrivateAccess = "true"))
	float enemySpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Movement", meta = (AllowPrivateAccess = "true"))
	float enemySpeedZ;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AMyEnemyCharacter* enemy;
};
