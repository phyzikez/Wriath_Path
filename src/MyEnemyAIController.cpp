#include "MyEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MyEnemyCharacter.h"

AMyEnemyAIController::AMyEnemyAIController()
{
	blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("blackboard"));
	check(blackboardComponent);
	behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("behaviorTree"));
	check(behaviorTreeComponent);
}

void AMyEnemyAIController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	
	if (pawn == nullptr) return;
	
	AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(pawn);
	if (enemy) {
		if (enemy->GetBehaviorTree()) {
			blackboardComponent->InitializeBlackboard(*(enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}
}
