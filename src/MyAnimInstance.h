#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

class AMyCharacter;
enum class EMoveXState : uint8;
enum class EMoveYState : uint8;
enum class EMoveAddState : uint8;
enum class EActionState : uint8;
enum class EWeaponState : uint8;

UCLASS()
class WRAITH_PATH_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaTime) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	class AMyCharacter* myCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* myCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Move X")
	EMoveXState baseMoveXState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Move Y")
	EMoveYState baseMoveYState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Jump & Crouch")
	EMoveAddState addMoveState;

	UPROPERTY(BlueprintReadOnly, Category = "Action")
	EActionState actionState;

	UPROPERTY(BlueprintReadOnly, Category = "Action | Weapon")
	bool isWeaponTargeting;

	UPROPERTY(BlueprintReadOnly, Category = "Action | Weapon")
	EWeaponState weaponState;

};