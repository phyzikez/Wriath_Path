#include "MyEnemyBeastCharacter.h"
#include "MyEStates.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "MyEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyEnemyBeastCharacter::AMyEnemyBeastCharacter()
{
	leftWeapon = CreateDefaultSubobject<UBoxComponent>(TEXT("leftWeapon"));
	rightWeapon = CreateDefaultSubobject<UBoxComponent>(TEXT("rightWeapon"));



	leftWeapon->SetGenerateOverlapEvents(true);
	leftWeapon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	leftWeapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	leftWeapon->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	rightWeapon->SetGenerateOverlapEvents(true);
	rightWeapon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	rightWeapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	rightWeapon->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AMyEnemyBeastCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
	leftWeapon->AttachToComponent(this->GetMesh(), rulesA, FName("leftWeaponSocket"));
	rightWeapon->AttachToComponent(this->GetMesh(), rulesA, FName("rightWeaponSocket"));
}

void AMyEnemyBeastCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMyEnemyBeastCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMyEnemyBeastCharacter::AttackMain()
{
	Super::AttackMain();
}

void AMyEnemyBeastCharacter::AttackAlt()
{
	Super::AttackAlt();
}

void AMyEnemyBeastCharacter::BeginPlay()
{
	Super::BeginPlay();
	leftWeapon->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemyBeastCharacter::OnCollisionBoxLeftBeginOverlap);
	leftWeapon->OnComponentEndOverlap.AddDynamic(this, &AMyEnemyBeastCharacter::OnCollisionBoxLeftEndOverlap);
	rightWeapon->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemyBeastCharacter::OnCollisionBoxRightBeginOverlap);
	rightWeapon->OnComponentEndOverlap.AddDynamic(this, &AMyEnemyBeastCharacter::OnCollisionBoxRightEndOverlap);
}

void AMyEnemyBeastCharacter::OnCollisionBoxLeftBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		p_myCharacter->SetDamage(damageHero, true);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Red, FString("Damage Left Weapon"), false); }
	}
}

void AMyEnemyBeastCharacter::OnCollisionBoxLeftEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMyEnemyBeastCharacter::OnCollisionBoxRightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		p_myCharacter->SetDamage(damageHero, true);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Red, FString("Damage Right Weapon"), false); }
	}
}

void AMyEnemyBeastCharacter::OnCollisionBoxRightEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}