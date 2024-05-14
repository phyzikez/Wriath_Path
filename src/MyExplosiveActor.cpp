#include "MyExplosiveActor.h"
#include "MyEStates.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimMontage.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "PhysicsEngine/RadialForceActor.h"
#include "MyCharacter.h"
#include "MyEnemyCharacter.h"

AMyExplosiveActor::AMyExplosiveActor()
{
 	PrimaryActorTick.bCanEverTick = true;

	rootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshRoot"));
	RootComponent = rootMesh;
	collisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("sphereInteract"));
	collisionSphere->SetupAttachment(GetRootComponent());

	barrelName = "";
	currentDamage = 0;
	maxDamage = 100;

	enemyActorClass = AMyEnemyCharacter::StaticClass();
	heroActorClass = AMyCharacter::StaticClass();
}

void AMyExplosiveActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMyExplosiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyExplosiveActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	rootMesh->SetStaticMesh(barrelMesh);
}

void AMyExplosiveActor::DamageBarrel(int32 damageValue)
{
	currentDamage += damageValue;
	if (currentDamage >= 100) {
		currentDamage = 100;
		Explode();
	}
}

void AMyExplosiveActor::Explode()
{
	float radiusDamage = collisionSphere->GetScaledSphereRadius();
	FVector burrelLocation = GetActorLocation();

	if (explosionSound) { UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionSound, burrelLocation); }

	// Spawn Explosion Wave
	ARadialForceActor* force = GetWorld()->SpawnActor<ARadialForceActor>();
	if (force) {
		force->GetRootComponent()->SetWorldLocation(burrelLocation);
		URadialForceComponent* forceComp = force->GetForceComponent();
		if (forceComp) {
			forceComp->Radius = collisionSphere->GetScaledSphereRadius();
			forceComp->Falloff = RIF_Linear;
			forceComp->ImpulseStrength = impulseStrength;
			forceComp->ForceStrength = forceStrength;
			forceComp->bImpulseVelChange = true;
			forceComp->bIgnoreOwningActor = true;
			forceComp->SetActive(true);
			forceComp->FireImpulse();
		}
	}

	// Damage Enemies
	TArray<AActor*> allActors;
	GetOverlappingActors(allActors, enemyActorClass);
	for (AActor* actor : allActors) {
		AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(actor);
		if (enemy) {
			float distToEpicenter = GetDistanceTo(enemy);
			if (distToEpicenter < radiusDamage) {
				float damage = 1.0f - distToEpicenter / radiusDamage;
				enemy->Damage(EDamage::ES_Explosive, damage);
			}
		}
	}

	// Damage Hero
	AMyCharacter* player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player) {
		float distToEpicenter = this->GetDistanceTo(player);
		if ((distToEpicenter < radiusDamage) && (player->GetActionState() != EActionState::ES_DomeShield)) {
			int32 damage = player->GetDamageByExplosive() * (1.f - distToEpicenter / radiusDamage);
			player->SetDamage(damage, false);
		}
	}

	// Particles
	if (explosionParticles) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticles, GetTransform()); }
	if (smokeParticles) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), smokeParticles, GetTransform()); }

	// Self Destroy
	if (force) force->Destroy();
	this->Destroy();
}