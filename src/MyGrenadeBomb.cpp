#include "MyGrenadeBomb.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "PhysicsEngine/RadialForceActor.h"
#include "MyCharacter.h"
#include "MyEnemyCharacter.h"

AMyGrenadeBomb::AMyGrenadeBomb()
{
	PrimaryActorTick.bCanEverTick = true;
	rootMeshSkeletalComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshRootSkeletal"));
	RootComponent = rootMeshSkeletalComponent;
	sphereInteract = CreateDefaultSubobject<USphereComponent>(TEXT("sphereInteract"));
	sphereInteract->SetSphereRadius(200);
	sphereInteract->SetupAttachment(GetRootComponent());
																																	
	auto skeletalMeshAsset = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/MyBPs/MyWeapons/Grenade/GrenadeSk.GrenadeSk'"));
	if (skeletalMeshAsset.Object != nullptr) {
		rootMeshSkeletal = skeletalMeshAsset.Object;
		rootMeshSkeletalComponent->SetSkeletalMesh(rootMeshSkeletal);
		//// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("SkeletMesh CTOR was created"), false); }
	}

	auto animParticlesAsset = ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("/Script/Engine.ParticleSystem'/Game/MyBPs/MyEffects/Grenade_Explotion.Grenade_Explotion'"));
	if (animParticlesAsset.Object != nullptr) {
		explosionParticles = animParticlesAsset.Object;
	}

	timeToDisarm = 0.5f; // experimental from animation
	timeToExplode = 3.0f;
	radiusDamage = 300.f;

	enemyActorClass = AMyEnemyCharacter::StaticClass();
	heroActorClass = AMyCharacter::StaticClass();
}

void AMyGrenadeBomb::BeginPlay()
{
	Super::BeginPlay();
}

void AMyGrenadeBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyGrenadeBomb::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetGrenadeBombThrowPhysics(false, false, ECollisionEnabled::NoCollision, false);
	sphereInteract->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	sphereInteract->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyGrenadeBomb::SetGrenadeBombThrowPhysics(bool isPhysics, bool isGravity, ECollisionEnabled::Type collType, bool isBlock)
{
	rootMeshSkeletalComponent->SetSimulatePhysics(isPhysics);
	rootMeshSkeletalComponent->SetEnableGravity(isGravity);
	rootMeshSkeletalComponent->SetCollisionEnabled(collType);
	rootMeshSkeletalComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	if (isBlock) rootMeshSkeletalComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void AMyGrenadeBomb::SetGrenadeThrowParams(float _timeToDisarm, float _timeToExplode, FVector _throwForce)
{
	timeToDisarm = _timeToDisarm;
	timeToExplode = _timeToExplode;
	throwForce = _throwForce;
}

void AMyGrenadeBomb::SetGrenadeVisible() {
	rootMeshSkeletalComponent->SetVisibility(true);
	GetWorldTimerManager().SetTimer(timerDisarm, this, &AMyGrenadeBomb::OnGrenadeDisarm, timeToDisarm); 
}

void AMyGrenadeBomb::OnGrenadeDisarm()
{
	FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
	FDetachmentTransformRules rulesD(rulesA, true);
	GetRootMeshSkeletal()->DetachFromComponent(rulesD);
	//throwingGrenadeBomb->SetGrenadeBombThrowPhysics(true, true, ECollisionEnabled::QueryAndPhysics, true);
	GetRootMeshSkeletal()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetRootMeshSkeletal()->SetSimulatePhysics(true);
	GetRootMeshSkeletal()->SetEnableGravity(true);
	GetRootMeshSkeletal()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetRootMeshSkeletal()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetRootMeshSkeletal()->AddImpulse(throwForce, NAME_None, true);
	GetWorldTimerManager().SetTimer(timerExplosion, this, &AMyGrenadeBomb::OnGrenadeExploded, timeToExplode);
}

void AMyGrenadeBomb::OnGrenadeExploded() {
	FVector grenadeLocation = this->GetActorLocation();
	// Spawn Explosion Wave
	ARadialForceActor* force = GetWorld()->SpawnActor<ARadialForceActor>();
	if (force) {
		force->GetRootComponent()->SetWorldLocation(grenadeLocation);
		URadialForceComponent* forceComp = force->GetForceComponent();
		if (forceComp) {
			forceComp->Radius = 400.f;
			forceComp->Falloff = RIF_Linear;
			forceComp->ImpulseStrength = 1400.0f;
			forceComp->ForceStrength = 750000.0f;
			forceComp->bImpulseVelChange = true;
			forceComp->bIgnoreOwningActor = true;
			forceComp->SetActive(true);
			forceComp->FireImpulse();
		}
	}

	// Damage Enemies
	TArray<AActor*> allEnemies;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), enemyActorClass, allEnemies);
	for (AActor* actor : allEnemies) {
		AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(actor);
		if (enemy) {
			float distToEpicenter = GetDistanceTo(enemy);
			if (distToEpicenter < radiusDamage) {
				float damage = 1.0f - distToEpicenter / radiusDamage;
				enemy->Damage(EDamage::ES_GrenadeBomb, damage);
			}
		}
	}

	// Damage Hero
	AMyCharacter* player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player) {
		float distToEpicenter = GetDistanceTo(player);
		if (distToEpicenter < radiusDamage) {
			int32 damage = player->GetDamageByGrenadeBomb() * (1.f - distToEpicenter / radiusDamage);
			player->SetDamage(damage, false);
		}
	}

	// Particles
	const FTransform meshTransform = this->GetTransform();
	if (explosionParticles) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticles, meshTransform); }

	// Self Destroy
	if (force) force->Destroy();
	this->Destroy();
}