#include "MyAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	myCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	if (myCharacter) {
		myCharacterMovement = myCharacter->GetCharacterMovement();
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float deltaTime) {
	Super::NativeUpdateAnimation(deltaTime);
	if (myCharacterMovement) {

		// Speed
		//auto velocity = myCharacterMovement->Velocity;
		//speed = UKismetMathLibrary::VSize(velocity);
		//float forwardVal = myCharacter->GetInputAxisValue(FName("MoveForward"));
		//float rightVal = myCharacter->GetInputAxisValue(FName("MoveRight"));
		
		// Base Move State
		baseMoveXState = myCharacter->GetMoveXState();
		baseMoveYState = myCharacter->GetMoveYState();

		// Additional Move State
		addMoveState = myCharacter->GetMoveAddState();

		// Action State
		actionState = myCharacter->GetActionState();
		
		// Is Weapon In Target Mode
		isWeaponTargeting = myCharacter->IsWeaponTargeting();

		// Acttive Weapon
		weaponState = myCharacter->GetWeaponState();
	}
}