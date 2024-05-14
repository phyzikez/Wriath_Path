#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "MyEnemyAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;

UCLASS()
class WRAITH_PATH_API AMyEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AMyEnemyAIController();
	virtual void OnPossess(APawn* pawn) override;
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return blackboardComponent; }
protected:
	UPROPERTY(BlueprintReadWrite, Category = "AI Params", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* blackboardComponent;
	UPROPERTY(BlueprintReadWrite, Category = "AI Params", meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* behaviorTreeComponent;
};
