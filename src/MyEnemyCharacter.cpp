#include "MyEnemyCharacter.h"
#include "MyEStates.h"
#include "MyGrenadeBomb.h"
#include "GameFramework/CharacterMovementComponent.h"
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
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

AMyEnemyCharacter::AMyEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	GetCapsuleComponent()->SetCapsuleHalfHeight(100.f);
	
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);

	widgetHealth = CreateDefaultSubobject<UWidgetComponent>(TEXT("healthWidget"));
	widgetHealth->SetupAttachment(GetRootComponent());
	widgetHealth->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	widgetHealth->SetWidgetSpace(EWidgetSpace::Screen);
	widgetHealth->SetDrawSize(FVector2D(300.f, 100.f));

	detectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("detectSphere"));
	detectSphere->SetupAttachment(GetRootComponent());
	detectSphere->SetSphereRadius(700.f);
	detectSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	detectSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	detectSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	deadHeroZone = CreateDefaultSubobject<USphereComponent>(TEXT("deadHeroZone"));
	deadHeroZone->SetupAttachment(GetRootComponent());
	deadHeroZone->SetSphereRadius(700.f);
	deadHeroZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	deadHeroZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	deadHeroZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	enemyState = EEnemyState::ES_Idle;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	fireDistTrace = 50'000.f;

	distAttack = 700;
	distChasing = 1200;
	distPatrol = 2000;

	// Class To Spawn Grenades
	GrenadeToSpawn = AMyGrenadeBomb::StaticClass();
}

void AMyEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString tablePath{ TEXT("/Script/Engine.DataTable'/Game/MyBPs/MyEnemy/EnemyDT.EnemyDT'") };
	UDataTable* table = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *tablePath));
	if (table)
	{
		FEnemyDataTable* ammoTableRow = nullptr;
		FName rowName;
		switch (enemyCharacterType) {
			case EEnemyCharacterType::ES_BanditCanyon1 : rowName = "drongoCanyon1"; break;
			case EEnemyCharacterType::ES_BanditCanyon2 : rowName = "drongoCanyon2"; break;
			case EEnemyCharacterType::ES_BanditCave : rowName = "drongoCave"; break;
			case EEnemyCharacterType::ES_WraithCave : rowName = "wraithCave"; break;
			case EEnemyCharacterType::ES_WraithScifi : rowName = "wraithScifi"; break;
			case EEnemyCharacterType::ES_WraithSWAT : rowName = "wraithSWAT"; break;
			case EEnemyCharacterType::ES_FemaleScifi : rowName = "belicaScifi"; break;
			case EEnemyCharacterType::ES_FemaleSWAT : rowName = "belicaSWAT"; break;
			case EEnemyCharacterType::ES_CaveBeast : rowName = "grux"; break;
			case EEnemyCharacterType::ES_HellBeast: rowName = "boss"; break;
		}
		ammoTableRow = table->FindRow<FEnemyDataTable>(FName(rowName), TEXT(""));
		if (ammoTableRow)
		{
			// Set From Table 
			GetMesh()->SetSkeletalMesh(ammoTableRow->skeletalMesh);
			//widgetHealth->SetWidget(ammoTableRow->healthWidget);

			enemyCharacterType = ammoTableRow->enemyCharacterType;
			enemyCombatType = ammoTableRow->enemyCombatType;
			enemyName = ammoTableRow->enemyName;
			currentHealth = ammoTableRow->currentHealth;
			weaponType = ammoTableRow->weaponType;

			damageByGunArmLeg = ammoTableRow->damageByGunArmLeg;
			damageByGunCore = ammoTableRow->damageByGunCore;
			damageByGunHead = ammoTableRow->damageByGunHead;
			damageByGrenadeBomb = ammoTableRow->damageByGrenadeBomb;
			damageByExplosive = ammoTableRow->damageByExplosive;
			damageByArmShockwave = ammoTableRow->damageByArmShockwave;
			damageByHeavyMass = ammoTableRow->damageByHeavyMass;
			damageHero = ammoTableRow->damageHero;

			fireMontage = ammoTableRow->fireMontage;
			fireAltMontage = ammoTableRow->fireAltMontage;
			grenadeMontage = ammoTableRow->grenadeMontage;
			damageMontage = ammoTableRow->damageMontage;
			deathMontage = ammoTableRow->deathMontage;
			saluteMontage = ammoTableRow->saluteMontage;

			timeToSelfDestroy = ammoTableRow->timeToSelfDestroy;

			socketWeaponFire = ammoTableRow->socketWeaponFire;
			socketWeaponFireAlt = ammoTableRow->socketWeaponFireAlt;
			traceParticles = ammoTableRow->traceParticles;
			muzzleParticles = ammoTableRow->muzzleParticles;
			muzzleAltParticles = ammoTableRow->muzzleAltParticles;
			bloodParticles = ammoTableRow->bloodParticles;
		}
	}
}

void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	detectSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemyCharacter::OnCollisionSphereDetectBeginOverlap);
	detectSphere->OnComponentEndOverlap.AddDynamic(this, &AMyEnemyCharacter::OnCollisionSphereDetectEndOverlap);
	deadHeroZone->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemyCharacter::OnCollisionDeadZoneBeginOverlap);
	deadHeroZone->OnComponentEndOverlap.AddDynamic(this, &AMyEnemyCharacter::OnCollisionDeadZoneEndOverlap);

	enemyAIController = Cast<AMyEnemyAIController>(this->GetController());
	if (enemyAIController) {
		for (int i = 0, N = pointsPatrol.Num(); i < N; ++i) {
			FVector pointWorld = UKismetMathLibrary::TransformLocation(this->GetActorTransform(), pointsPatrol[i]);
			enemyAIController->GetBlackboardComponent()->SetValueAsVector(FName(FString::Printf(TEXT("pointPatrol%d"), i+1)), pointWorld);
			//DrawDebugSphere(GetWorld(), pointWorld, 15.f, 8, FColor::Red, true);
		}
		for (int i = 0, N = pointsHide.Num(); i < N; ++i) {
			FVector pointWorld = UKismetMathLibrary::TransformLocation(this->GetActorTransform(), pointsHide[i]);
			enemyAIController->GetBlackboardComponent()->SetValueAsVector(FName(FString::Printf(TEXT("pointHide%d"), i+1)), pointWorld);
			//DrawDebugSphere(GetWorld(), pointWorld, 15.f, 8, FColor::Red, true);
		}
		enemyAIController->RunBehaviorTree(behaviorTree);
		enemyAIController->GetBlackboardComponent()->SetValueAsEnum(TEXT("enemyType"), uint8(enemyCombatType));
		enemyAIController->GetBlackboardComponent()->SetValueAsEnum(TEXT("enemyCharacter"), uint8(enemyCharacterType));
	}
}

void AMyEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set Widget Visibility According To Distance Between Enemy And Pers
	float dist = UKismetMathLibrary::Vector_Distance(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
	widgetHealth->SetVisibility((dist < 2200.f ? true : false), true);

	// Update enemy State According To Distance Between Him And Player
	if (enemyState != EEnemyState::ES_Salute && enemyState != EEnemyState::ES_Dead && isStunning == false) {
		if (dist < distAttack) { SetEnemyState(EEnemyState::ES_Attack); }
		else if (dist >= distAttack && dist < distChasing) { SetEnemyState(EEnemyState::ES_Chasing); }
		else if (dist >= distChasing && dist < distPatrol) { SetEnemyState(EEnemyState::ES_Patrol); }
		else if (dist >= distPatrol) { SetEnemyState(EEnemyState::ES_Idle); }
	}
}

void AMyEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Collision With Pers And Other Actors..
void AMyEnemyCharacter::OnCollisionSphereDetectBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
																					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		//SetEnemyState(EEnemyState::ES_Attack);
		enemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("pers"), p_myCharacter);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Red, FString("ENEMY DETECT YOU"), false); }
	}
}

void AMyEnemyCharacter::OnCollisionSphereDetectEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		//SetEnemyState(EEnemyState::ES_Patrol);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Green, FString("ENEMY NO DETECT YOU"), false); }
	}
}

void AMyEnemyCharacter::OnCollisionDeadZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
	if (myCharacter) {
		isPlayerInDeadZone = true;
		enemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("isPlayerInDeadZone"), isPlayerInDeadZone);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Red, FString("Player Entered Dead Zone"), false); }
	}
}

void AMyEnemyCharacter::OnCollisionDeadZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
	if (myCharacter) {
		isPlayerInDeadZone = false;
		enemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("isPlayerInDeadZone"), isPlayerInDeadZone);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Green, FString("Player Left Dead Zone"), false); }
	}
}

void AMyEnemyCharacter::OnHit(int32 damageValue, EDamage damageBodyPart, FName damagedBoneName, FVector hitLoc)
{
	if (enemyState != EEnemyState::ES_Dead) {
		SetIsStunning(true);
		enemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("isStunning"), isStunning);
		Damage(damageBodyPart, 0.0, hitLoc, damageValue, damagedBoneName);
	}
}

void AMyEnemyCharacter::Damage(EDamage damageType, float damageRadius, FVector hitLoc, int32 damageBodypart, FName damagedBoneName)
{
	FName montageSectionName = "";
	float damageValue = 0.0f;
	switch (damageType) {
	case EDamage::ES_ArmLeg:
		damageValue = damageBodypart + damageByGunArmLeg;
		if (damagedBoneName.ToString().Contains("_l")) montageSectionName = "left";
		else if (damagedBoneName.ToString().Contains("_r")) montageSectionName = "right";
		else montageSectionName = "front";
		break;
	case EDamage::ES_Core:
		damageValue = damageBodypart + damageByGunCore;
		montageSectionName = "front";
		break;
	case EDamage::ES_Head:
		damageValue = damageBodypart + damageByGunHead;
		montageSectionName = "stun";
		break;
	case EDamage::ES_GrenadeBomb:
		damageValue = damageRadius * damageByGrenadeBomb;
		if (damageValue > 10.f) montageSectionName = "fall";
		else montageSectionName = "stun";
		break;
	case EDamage::ES_Explosive:
		damageValue = damageRadius * damageByExplosive;
		if (damageValue > 10.f) montageSectionName = "fall";
		else montageSectionName = "stun";
		break;
	case EDamage::ES_Rocket:
		damageValue = 100;
		montageSectionName = "fall";
		break;
	case EDamage::ES_ArmShockwave:
		damageValue = damageRadius * damageByArmShockwave;
		if (damageValue > 20.f) montageSectionName = "fall";
		else montageSectionName = "stun";
		break;
	case EDamage::ES_HeavyMass:
		damageValue = damageByHeavyMass;
		montageSectionName = "fall";
		break;
	}

	// Hit Widget
	ShowDamageValue(damageValue, hitLoc);
	
	// Uodate Health
	currentHealth -= damageValue; 

	if (currentHealth > 0) {
		if (isFall == false) {
			if (montageSectionName == "fall") {
				isFall = true;
			}
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && damageMontage) {
				animInst->Montage_Play(damageMontage);
				animInst->Montage_JumpToSection(montageSectionName, damageMontage);
			}
		}
	}
	else {
		currentHealth = 0;
		if (enemyState != EEnemyState::ES_Dead) {
			SetEnemyState(EEnemyState::ES_Dead);
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Enemy Death Montage"), false); }
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && deathMontage) { animInst->Montage_Play(deathMontage); }
			GetWorldTimerManager().SetTimer(timerDeath, this, &AMyEnemyCharacter::OnSelfDestroy, timeToSelfDestroy);
		}
	}
}

void AMyEnemyCharacter::OnSelfDestroy()
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Enemy Has Gone To Heaven"), false); }
	this->Destroy();
}

void AMyEnemyCharacter::SetEnemyState(EEnemyState newState)
{
	enemyState = newState;
	enemyAIController->GetBlackboardComponent()->SetValueAsEnum(TEXT("enemyState"), uint8(enemyState));
}

void AMyEnemyCharacter::AttackRnd()
{
	int32 rnd = FMath::RandRange(1, 5);
	if (enemyState == EEnemyState::ES_Attack) { (rnd < 4) ? AttackMain() : AttackAlt(); }
	else if (enemyState == EEnemyState::ES_Chasing) { (rnd < 4) ? AttackMain() : Grenade(); }
}

void AMyEnemyCharacter::AttackMain()
{
	UAnimMontage* resMontage;
	// If Beast --> Random Base Or Alt Attack 
	resMontage = (enemyCombatType == EEnemyCombatType::ES_Beast) ? ((FMath::RandRange(0, 1) ? fireMontage : fireAltMontage)) : fireMontage;
	PlayAttackMontage(resMontage);
}

void AMyEnemyCharacter::AttackAlt()
{
	UAnimMontage* resMontage = fireAltMontage;
	PlayAttackMontage(resMontage, "ALT ATTACK");
}

void AMyEnemyCharacter::PlayAttackMontage(UAnimMontage* resMontage, FString debugText) {
	//// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, debugText, false); }
	if (enemyState != EEnemyState::ES_Dead && isStunning == false) {
		SetEnemyState(EEnemyState::ES_Attack);
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && resMontage) {
			int32 rndSection = FMath::RandRange(1, resMontage->GetNumSections());
			animInst->Montage_Play(resMontage);
			animInst->Montage_JumpToSection(FName(FString::Printf(TEXT("%d"), rndSection)), resMontage);
			GetWorldTimerManager().SetTimer(timerAttack, this, &AMyEnemyCharacter::OnEndAttack, resMontage->GetPlayLength());
		}
	}
}
 
void AMyEnemyCharacter::SendBullet(FName socketFireName)
{
	// Particles
	const USkeletalMeshSocket* fireStartSocket = GetMesh()->GetSocketByName(socketFireName);
	if (fireStartSocket) {
		// Muzzle Particles
		const FTransform socketTransform = fireStartSocket->GetSocketTransform(GetMesh());
		if (muzzleParticles && GetWorld()) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleParticles, socketTransform);
		}
		// (Drongo Is Not Included If Bazooka)
		bool drongoBazookaFire = ((enemyCharacterType == EEnemyCharacterType::ES_BanditCanyon1 ||
												  enemyCharacterType == EEnemyCharacterType::ES_BanditCanyon2 ||
												  enemyCharacterType == EEnemyCharacterType::ES_BanditCave) && 
												  (socketFireName == socketWeaponFireAlt));
		// Trace 
		if (!drongoBazookaFire) {
			FHitResult traceHit;
			FVector from{ socketTransform.GetLocation() };
			FVector to;
			AMyCharacter* player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (player) {
				//to = from + this->GetActorTransform().GetUnitAxis(EAxis::X) * fireDistTrace;
				to = from + socketTransform.GetUnitAxis(EAxis::X) * fireDistTrace;
				//to.X += FMath::RandRange(-0.1, 0.1);
				//to.Y += FMath::RandRange(-0.1, 0.1);
				//to.Z += FMath::RandRange(-0.1, 0.1);
				//PrintMyVector(socketTransform.GetUnitAxis(EAxis::X), "AXIS UNIT 2", 7.f);
				FVector traceEndPoint{ to };
				if (GetWorld()) GetWorld()->LineTraceSingleByChannel(traceHit, from, to, ECollisionChannel::ECC_Visibility);
				if (traceHit.bBlockingHit) // was there a trace hit?
				{
					traceEndPoint = traceHit.Location;
					//DrawDebugLine(GetWorld(), from, to, FColor::Red, false, 4.f);
					//DrawDebugPoint(GetWorld(), from, 4.f, FColor::Blue, false, 4.f);
					//DrawDebugPoint(GetWorld(), traceEndPoint, 4.f, FColor::Red, false, 4.f);
					AMyCharacter* damagedHero = Cast<AMyCharacter>(traceHit.HitObjectHandle.GetManagingActor());
					if (damagedHero) {
						damagedHero->SetDamage(damageHero, true);
						if (damagedHero->GetIsDead()) {
							enemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("persIsDead"), true);
						}
					}
				}
				if (traceParticles && GetWorld())
				{
					UParticleSystemComponent* trace = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), traceParticles, socketTransform);
					trace->SetVectorParameter(FName("Target"), traceEndPoint);
				}
			}
		}
	}
}

void AMyEnemyCharacter::OnEndAttack() {
	AMyCharacter* player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player && player->GetIsDead()) {
		Salute();
	}
}

void AMyEnemyCharacter::Salute()
{
	if (enemyState != EEnemyState::ES_Dead && isStunning == false) {
		SetEnemyState(EEnemyState::ES_Salute);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, FString("Salute"), false); }
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && saluteMontage) { animInst->Montage_Play(saluteMontage); }
	}
}

void AMyEnemyCharacter::Grenade()
{
	if (enemyState != EEnemyState::ES_Dead && isStunning == false) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, FString("Enemy Toss Grenade"), false); }
		FTransform newThrowTransform = GetMesh()->GetSocketTransform("grenadeBombSocket");
		AMyGrenadeBomb* newGrenade = GetWorld()->SpawnActor<AMyGrenadeBomb>(GrenadeToSpawn, newThrowTransform);
		if (newGrenade) {
			FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
			if (enemyName.Contains("Female")) {
				newGrenade->SetGrenadeThrowParams(0.5f, 3.5f, GetMesh()->GetSocketTransform("grenadeBombSocket").GetUnitAxis(EAxis::X) * 1200.f); // Bomb
			}
			else {
				newGrenade->SetGrenadeThrowParams(0.5f, 2.5f, GetMesh()->GetSocketTransform("grenadeBombSocket").GetUnitAxis(EAxis::X)  * 1300.f); // Grenade
			}
			newGrenade->GetRootMeshSkeletal()->SetVisibility(false);
			newGrenade->GetRootMeshSkeletal()->AttachToComponent(this->GetMesh(), rulesA, FName("grenadeBombSocket"));
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && grenadeMontage) { animInst->Montage_Play(grenadeMontage); }
			thrownGrenade = newGrenade;
			GetWorldTimerManager().SetTimer(timerAttack, this, &AMyEnemyCharacter::OnEndAttack, 6.f);
		}
	}
}

void AMyEnemyCharacter::GrenadeDisarm()
{
	if (thrownGrenade) {
		thrownGrenade->GetRootMeshSkeletal()->SetVisibility(true);
		thrownGrenade->OnGrenadeDisarm();
	}
}