#include "MyDamageActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MyCharacter.h"
#include "MyEnemyCharacter.h"
#include "MyTrigger.h"

AMyDamageActor::AMyDamageActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshRoot"));
	RootComponent = rootMeshComponent;
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollision"));
	boxCollision->SetupAttachment(RootComponent);
}

void AMyDamageActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	rootMeshComponent->SetStaticMesh(rootMesh);
	if (widget) widgetComponent->SetWidget(widget);
}

void AMyDamageActor::BeginPlay()
{
	Super::BeginPlay();
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyDamageActor::OnBoxBeginOverlap);
}

void AMyDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isDamageActivated && triggerBox) {
		if (triggerBox->IsTriggerActivated()) {
			isDamageActivated = true;
			// Physics Run
			rootMeshComponent->SetSimulatePhysics(true);
			rootMeshComponent->SetEnableGravity(true);
			rootMeshComponent->SetMassOverrideInKg(NAME_None, mass);
			GetWorldTimerManager().SetTimer(timerSelfDestoy, this, &AMyDamageActor::OnDestroyActor, lifeTime);
		}
	}
}

void AMyDamageActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(OtherActor);
	if (enemy) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 2.5f, FColor::FromHex("#FFFF00FF"), FString("DAMAGE ENEMY : ") + OverlappedComponent->GetName(), false); }
		if (!damagedActors.contains(enemy)) {
			enemy->Damage(EDamage::ES_HeavyMass);
			damagedActors.insert(enemy);
		}
	}
}

void AMyDamageActor::OnDestroyActor()
{
	this->Destroy();
}