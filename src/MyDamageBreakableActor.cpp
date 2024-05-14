#include "MyDamageBreakableActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MyCharacter.h"
#include "MyEnemyCharacter.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "MyTrigger.h"

AMyDamageBreakableActor::AMyDamageBreakableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	geoCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("meshRoot"));
	RootComponent = geoCollection;
	//geoCollection->SetGenerateOverlapEvents(true);
	//geoCollection->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	geoCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	geoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	geoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	geoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	geoCollection->SetSimulatePhysics(false);
	geoCollection->SetEnableGravity(false);
	geoCollection->bNotifyCollisions = true;

	enemyActorClass = AMyEnemyCharacter::StaticClass();

	isDamageActivated = false;
}

void AMyDamageBreakableActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (widget) widgetComponent->SetWidget(widget);
	geoCollection->SetMassOverrideInKg(NAME_None, mass);
}

void AMyDamageBreakableActor::BeginPlay()
{
	Super::BeginPlay();
	geoCollection->OnComponentBeginOverlap.AddDynamic(this, &AMyDamageBreakableActor::OnBoxBeginOverlap);
}

void AMyDamageBreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isDamageActivated && triggerBox) {
		if (triggerBox->IsTriggerActivated()) {
			// Physics Run
			geoCollection->SetSimulatePhysics(true);
			geoCollection->SetEnableGravity(true);
			isDamageActivated = true;
			GetWorldTimerManager().SetTimer(timerSelfDestoy, this, &AMyDamageBreakableActor::OnDestroyActor, lifeTime);
		}
	}
}

void AMyDamageBreakableActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
																					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(OtherActor);
	if (enemy) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FFFF00FF"), FString("DAMAGE ENEMY : ") + enemy->GetEnemyName(), false); }
		if (!damagedActors.Contains(enemy)) {
			enemy->Damage(EDamage::ES_HeavyMass);
			damagedActors.Add(enemy);
		}
	}
}

void AMyDamageBreakableActor::OnDestroyActor()
{
	this->Destroy();
}