#include "MyTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundCue.h"
#include "MyCharacter.h"

AMyTrigger::AMyTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	isTriggerActive = true;

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("triggerBox"));
	boxCollision->SetupAttachment(RootComponent);
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxCollision->SetGenerateOverlapEvents(true);
	boxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

void AMyTrigger::BeginPlay()
{
	Super::BeginPlay();	
}

void AMyTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}