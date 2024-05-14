#include "MyEnemyAnimInstance.h"
#include "MyEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMyEnemyAnimInstance::UpdateAnimParams(float DeltaTime)
{
	if (enemy == nullptr) { enemy = Cast<AMyEnemyCharacter>(this->TryGetPawnOwner()); }
	if (enemy)
	{
		FVector vel = enemy->GetVelocity();
		vel.Z = 0.f;
		enemySpeed = vel.Size();
		enemySpeedZ = enemy->GetCharacterMovement()->Velocity.Z;
	}
}