#include "MyCharacter.h"
#include "MyEStates.h"
#include "MyEnemyCharacter.h"
#include "MyEnemyBeastCharacter.h"
#include "MyWeaponBox.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "../../Engine/Classes/Components/BoxComponent.h"
#include "Animation/AnimMontage.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "PhysicsEngine/RadialForceActor.h"
#include "MyTrigger.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"

AMyCharacter::AMyCharacter()
{
	healthMax = 100;
	health = healthMax;

	for (AMyWeapon* w : weapons) w = nullptr;
	currentWeapon = nullptr;
	pickedWeapon = nullptr;

	// Shield Dome Params
	deltaMovement = 0.f;
	shieldMovAmpl = { 5.f };
	shieldMovPhase = { 5.f };
	shieldRotAmpl = { 5.f };
	shieldRotAmpl = { 5.f };

	// Init States
	isWeaponTargeting = false;
	baseMoveXState = EMoveXState::ES_Idle;
	baseMoveYState = EMoveYState::ES_Idle;
	addMoveState = EMoveAddState::ES_None;
	actionState = EActionState::ES_None;
	weaponState = EWeaponState::ES_One;
	crosshairState = EHUDAimState::ES_Regular;

	PrimaryActorTick.bCanEverTick = true;
	// Default Rotation
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	// Orient Rotation
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetMesh()->SetRelativeLocation(FVector(5.f, 0.f, -91.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, -113.f));

	// Collision With Ground
	collisionGroundBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Ground Box"));
	collisionGroundBox->SetupAttachment(GetRootComponent());
	collisionGroundBox->SetBoxExtent(FVector(31.f, 31.f, 100.f));
	collisionGroundBox->SetRelativeLocation(FVector(0.f, 0.f, -180.f));
	collisionGroundBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	collisionGroundBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	collisionGroundBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// Camera Foot
	myCameraRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("theCameraRoot"));
	myCameraRoot->SetupAttachment(GetRootComponent());
	camRootXDefault = -20.f;
	camRootXCur = camRootXDefault;
	myCameraRoot->SetRelativeLocation(FVector(camRootXDefault, 65.f, 68.f));
	myCameraRoot->SetRelativeRotation(FRotator(0.f, -6.f, -15.f));
	myCameraRoot->TargetArmLength = 100.f;
	// Camera
	myCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("theCamera"));
	myCamera->SetupAttachment(myCameraRoot);
	myCamera->SetFieldOfView(55.f);

	// Dome Shield
	p_DomeShield = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("theShieldDome"));
	p_DomeShield->SetupAttachment(GetRootComponent());
	p_DomeShield->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	p_DomeShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	p_DomeShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	currentWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MyWidgetTest"));
	currentWidget->SetupAttachment(GetRootComponent());

	// Movement
	movementComponent = this->GetCharacterMovement();

	// Crouch
	movementComponent->MaxWalkSpeedCrouched = 70.f;

	// Fire
	fireDistTrace = 50'000.f;
	isDebugFireTrace = false;
	isFiring = false;
	targetingSpeed = 20.f;

	// Bomb & Grenades
	countGrenadesMax = 5;
	countBombsMax = 3;
	countGrenades = countGrenadesMax;
	countBombs = countBombsMax;

	// Zoom RMB
	cameraFOVDefault = 55.f;
	cameraFOVZoom = 50.f;

	// Time To Reactivate Actions
	timeUltaDeactivation = 2.f;
	timeArmShockWaveDeactivation = 2.f;
	timeGrenadeAnimReactivation = 1.f;
	timeBombAnimReactivation = 1.f;
	timeDomeShieldReactivation = 8.f;
	timeUltraVisionReactivation = 8.f;
	timeEquip = 0.5f;
	timeReloadDuration = 1.5f;
	timeClimbBegin = 1.5f;
	timeClimbEnd = 1.5f;

	// Extra Skills Init States
	isUltaEnable = true;
	isDomeShieldEnable = true;
	isArmShockEnable = true;
	isUltraVisionEnable = true;
	isDomeShieldDeactivated = false;
	isUltraVisionDeactivated = false;

	// Is Climbing
	isReadyToClimbLadder = false;
	MaxStepHeightClimbing = 0.5f;

	// Class To Spawn Grenades
	GrenadeToSpawn = AMyGrenadeBomb::StaticClass();
	isThrowGrenadeEnable = false;
	isThrowBombEnable = false;

	// Init XP
	XPLevel = 1;

	isVulnarable = true;

	isFirstEquip = true;

	damageByGrenadeBomb = 30;
	damageByExplosive = 40;

	textFoundForHUD.Add("");
	textFoundForHUD.Add("");
	textPickForHUD.Add("");
	textPickForHUD.Add("");
	textHeadshotForHUD = "";
	textTaskForHUD = "";
	textActionForHUD = "";

	pickedItems.Add(TTuple<FString, bool>("pistol", false));
	pickedItems.Add(TTuple<FString, bool>("SMG11", false));
	pickedItems.Add(TTuple<FString, bool>("AR4", false));
	pickedItems.Add(TTuple<FString, bool>("sniper", false));
	pickedItems.Add(TTuple<FString, bool>("minigun", false));
	pickedItems.Add(TTuple<FString, bool>("ammoAR4Scope", false));
	pickedItems.Add(TTuple<FString, bool>("ammoAR4Sight", false));
	pickedItems.Add(TTuple<FString, bool>("ammoAR4Suppressor", false));
	pickedItems.Add(TTuple<FString, bool>("ammoLaserPistol", false));

	//magazineComponent->CreateDefaultSubobject<USceneComponent>(TEXT("magComponent"));

	cameraMovieZoom = 15.f;
	targetingMovieSpeed = 1.f;
	targetingMovieBeginDelay = 18.f;
	targetingMoviePeakDelay = 4.f;
	movieState = EMovieActionState::ES_End;
	camRootXDelta = 40.f;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	collisionGroundBox->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnGroundBoxOverlap);

	if (myCamera) {
		cameraFOVDefault = myCamera->FieldOfView;
		cameraFOVCurrent = cameraFOVDefault;
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnWeaponboxBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnWeaponboxEndOverlap);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Zoom RMB
	if (movieState == EMovieActionState::ES_End) {
		cameraFOVCurrent = FMath::FInterpTo(cameraFOVCurrent, (isWeaponTargeting ? cameraFOVZoom : cameraFOVDefault), DeltaTime, targetingSpeed);
		myCamera->SetFieldOfView(cameraFOVCurrent);
	}

	// Update Extra Skill Time Elapsed From Activation
	timeArmShockWaveElapsed = GetWorldTimerManager().GetTimerElapsed(timerArmShockWave);
	timeUltaElapsed = GetWorldTimerManager().GetTimerElapsed(timerEmoCaboom);
	timeDomeShieldElapsed = GetWorldTimerManager().GetTimerElapsed(timerDomeShield);
	timeUltraVisionElapsed = GetWorldTimerManager().GetTimerElapsed(timerUltraVision);

	// Grenades & Bombs
	isThrowGrenadeEnable = GetWorldTimerManager().GetTimerRemaining(timerGrenadeAmin) < 0.f;
	isThrowBombEnable = GetWorldTimerManager().GetTimerRemaining(timerBombAnim) < 0.f;

	runTime += DeltaTime;

	// Delta Move Skeletal Pistol Clip X
	if (currentWeapon) {
		float fireDT = GetWorldTimerManager().GetTimerElapsed(currentWeapon->GetTimerClip());
		float maxT = currentWeapon->GetWeaponFireFrequency();
		// PISTOL
		if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoPistol) {
			// CLIP
			if (fireDT < 0.01f) currentWeapon->SetDeltaClipPistolX(0);
			else if (fireDT < maxT / 2.0f) currentWeapon->SetDeltaClipPistolX(2 * currentWeapon->GetDeltaClipPistolXMax() / -maxT * fireDT);
			else currentWeapon->SetDeltaClipPistolX(2 * currentWeapon->GetDeltaClipPistolXMax() * (fireDT / maxT - 1));
		}
		// SNIPER
		else if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoSniper) {
			// CLIP
			if (fireDT < 0.001f) currentWeapon->SetDeltaClipSniperX(0);
			else if (fireDT < maxT / 2) currentWeapon->SetDeltaClipSniperX(2 * currentWeapon->GetDeltaClipSniperXMax() / -maxT * fireDT);
			else currentWeapon->SetDeltaClipSniperX(2 * currentWeapon->GetDeltaClipSniperXMax() * (fireDT / maxT - 1));
			// SIGHT
			float sightCurTimeMoveUp = GetWorldTimerManager().GetTimerElapsed(currentWeapon->GetTimerSightMoveUp());
			float sightCurTimeMoveDown = GetWorldTimerManager().GetTimerElapsed(currentWeapon->GetTimerSightMoveDown());
			float sightMaxTime = currentWeapon->GetSightMoveDT();
			FVector sightDownPos = currentWeapon->GetDeltaSightSniperDownLoc();
			FVector sightUpperPos = currentWeapon->GetDeltaSightSniperUpperLoc();
			FVector sightLensDownPos = currentWeapon->GetDeltaSightLensSniperDownLoc();
			FVector sightLensUpperPos = currentWeapon->GetDeltaSightLensSniperUpperLoc();
			float sightLensDiff = sightLensDownPos.X - sightLensUpperPos.X;
			if (isWeaponTargeting) {
				// Fixed Upper Pos
				if (sightCurTimeMoveUp < 0.f) {
					currentWeapon->SetDeltaSightSniper(sightUpperPos);
					currentWeapon->SetDeltaSightLensSniper(sightLensUpperPos);
				}
				// Moving Up
				else {
					currentWeapon->SetDeltaSightSniper(FVector(sightDownPos.X * (1 - sightCurTimeMoveUp), 0.f, sightDownPos.Z * (1 - sightCurTimeMoveUp)));
					currentWeapon->SetDeltaSightLensSniper(FVector(sightLensUpperPos.X + sightLensDiff * (1 - sightCurTimeMoveUp), 0.f, 0.f));
				}
			}
			else {
				// Fixed Down Pos
				if (sightCurTimeMoveDown < 0.f) {
					currentWeapon->SetDeltaSightSniper(sightDownPos);
					currentWeapon->SetDeltaSightLensSniper(sightLensDownPos);
				}
				// Moving Down
				else {
					currentWeapon->SetDeltaSightSniper(FVector(sightDownPos.X * sightCurTimeMoveDown, 0.f, sightDownPos.Z * sightCurTimeMoveDown));
					currentWeapon->SetDeltaSightLensSniper(FVector(sightLensUpperPos.X + sightLensDiff * sightCurTimeMoveUp, 0.f, 0.f));
				}
			}
		}
		else if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoMinigun) {
			if (isMinigunRollerRotates) {
				if (minigunRollerSpeed < minigunRollerMax) minigunRollerSpeed += minigunRollerDelta;
			}
			else {
				if (minigunRollerSpeed > 0.f) minigunRollerSpeed -= minigunRollerDelta;
			}
		} 
	} 

	// HUD Crosshair Check
	UpdateCrosshairState();

	// XP Check
	if (XPCurrent >= 100.f) {
		XPCurrent = 0.f;
		++XPLevel;
	}

	// --------------- Camera Interp ---------------- //
	// 1. Rocket
	if (isCameraFOVInterpRocket) { CameraInterpRocket(DeltaTime); }
	// 2. ArmPower
	if (isArmPowerTimeDilating) { ArmPowerCameraChanging(); }
	// 3. Movie Action
	if (movieState != EMovieActionState::ES_End) { UpdateMovieActionFOV(DeltaTime); }

	// ------------- Accessoire HUD Inform Check ------------- //
	isNeedToInformAboutAccessoire = ( ((accessoireNum | PISTOL_LASER_MASK) == PISTOL_LASER_RES) || 
															((accessoireNum | AR4_LASER_MASK) == AR4_LASER_RES) ||
															((accessoireNum | AR4_OPTICAL_MASK) == AR4_OPTICAL_RES) ||
															((accessoireNum | AR4_SUPPR_MASK) == AR4_SUPPR_RES) );
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyCharacter::MoveForward); 
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAction(FName("Menu"), EInputEvent::IE_Pressed, this, &AMyCharacter::Menu);
	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::CrouchBegin);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Released, this, &AMyCharacter::CrouchEnd);
	PlayerInputComponent->BindAction(FName("DoAction"), EInputEvent::IE_Pressed, this, &AMyCharacter::DoAction);
	PlayerInputComponent->BindAction(FName("Grenade"), EInputEvent::IE_Pressed, this, &AMyCharacter::Grenade);
	PlayerInputComponent->BindAction(FName("Bomb"), EInputEvent::IE_Pressed, this, &AMyCharacter::Bomb);
	PlayerInputComponent->BindAction(FName("Reload"), EInputEvent::IE_Pressed, this, &AMyCharacter::ReloadBegin);
	PlayerInputComponent->BindAction(FName("Ulta"), EInputEvent::IE_Pressed, this, &AMyCharacter::Ulta);
	PlayerInputComponent->BindAction(FName("FireMode"), EInputEvent::IE_Pressed, this, &AMyCharacter::FireMode);
	PlayerInputComponent->BindAction(FName("WeaponConfigure"), EInputEvent::IE_Pressed, this, &AMyCharacter::WeaponConfigure);
	PlayerInputComponent->BindAction(FName("WeaponOne"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChooseWeaponOne);
	PlayerInputComponent->BindAction(FName("WeaponTwo"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChooseWeaponTwo);
	PlayerInputComponent->BindAction(FName("WeaponThree"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChooseWeaponThree);
	PlayerInputComponent->BindAction(FName("WeaponFour"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChooseWeaponFour);
	PlayerInputComponent->BindAction(FName("WeaponFive"), EInputEvent::IE_Pressed, this, &AMyCharacter::ChooseWeaponFive);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &AMyCharacter::FirePressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Released, this, &AMyCharacter::FireReleased);
	PlayerInputComponent->BindAction(FName("Targeting"), EInputEvent::IE_Pressed, this, &AMyCharacter::Targeting);
	PlayerInputComponent->BindAction(FName("Targeting"), EInputEvent::IE_Released, this, &AMyCharacter::TargetingStop);
	PlayerInputComponent->BindAction(FName("ArmShockWave"), EInputEvent::IE_Pressed, this, &AMyCharacter::ArmShockWave);
	PlayerInputComponent->BindAction(FName("UltraVision"), EInputEvent::IE_Pressed, this, &AMyCharacter::UltraVision);
	PlayerInputComponent->BindAction(FName("DomeShield"), EInputEvent::IE_Pressed, this, &AMyCharacter::DomeShield);
}

// Camera Interp Start (Rocket Launch)
void AMyCharacter::SetCameraInterpStartRocket()
{
	oldCameraFOV = cameraFOVCurrent;
	isCameraFOVInterpRocket = true;
	GetWorldTimerManager().SetTimer(timerCameraFOVRocket, this, &AMyCharacter::SetCameraInterpBackRocket, deltaTimeStartCameraFOVInterpRocket);
} 

// Camera Interp Back (Rocket Launch)
void AMyCharacter::SetCameraInterpBackRocket()
{
	isCameraFOVInterpRocketBack = true;
	GetWorldTimerManager().SetTimer(timerCameraFOVRocket, this, &AMyCharacter::SetCameraInterpEndRocket, deltaTimeEndCameraFOVInterpRocket);
}

// Camera Interp End (Rocket Launch)
void AMyCharacter::SetCameraInterpEndRocket()
{
	isCameraFOVInterpRocket = false;
	isCameraFOVInterpRocketBack = false;
}

// Camera Interp(Rocket Launch)
void AMyCharacter::CameraInterpRocket(float DeltaTime)
{
	cameraFOVCurrent = FMath::FInterpTo(cameraFOVCurrent, 
																(isCameraFOVInterpRocketBack ? oldCameraFOV : oldCameraFOV - deltaCameraFOVInterpRocket), 
																DeltaTime, 
																speedCameraFOVInterpRocket);
	myCamera->SetFieldOfView(cameraFOVCurrent);
}

void AMyCharacter::OnGroundBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	if (addMoveState == EMoveAddState::ES_JumpFalling || movementComponent->Velocity.Z != 0) {
		addMoveState = EMoveAddState::ES_JumpEnd;
		JumpLanding();

		const FVector start = collisionGroundBox->GetComponentLocation();

		TArray<AActor*> actorsIgnored;
		actorsIgnored.Add(this);

		FHitResult hitResult;
		UKismetSystemLibrary::BoxTraceSingle(this, start, start, FVector(10.f, 10.f, 10.f),
																    collisionGroundBox->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
																    false, actorsIgnored, EDrawDebugTrace::ForDuration, hitResult, true);
	}
}

void AMyCharacter::OnWeaponboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
																			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyWeaponBox* p_weaponbox = Cast<AMyWeaponBox>(OtherActor);
	if (p_weaponbox && (p_weaponbox->GetIsBoxOpened() == false)) {
		currentWeaponbox = p_weaponbox;
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Enter The Box"), false); }
	}
}

void AMyCharacter::OnWeaponboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyWeaponBox* p_weaponbox = Cast<AMyWeaponBox>(OtherActor);
	if (p_weaponbox && (p_weaponbox->GetIsBoxOpened() == false)) {
		currentWeaponbox = nullptr;
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Leave The Box"), false); }
	}
}

void AMyCharacter::JumpLanding() {
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, FString(" --- Jump End --- "), false); }
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageJump) {
		animInst->Montage_Play(montageJump);
		animInst->Montage_JumpToSection(FName("jump_end"), montageJump);
	}
}

void AMyCharacter::ArmPowerCameraChangeStart()
{
	isArmPowerTimeDilating = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.3f, FColor::Red, FString("D I L A T I O N   S T A R T"), false); }
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), timeDilation);
	GetWorldTimerManager().SetTimer(timerCameraArmPower, this, &AMyCharacter::ArmPowerCameraChangeEnd, 0.75f);
	ArmPowerCameraChanging();
}

void AMyCharacter::ArmPowerCameraChanging()
{
	float elapsed = GetWorldTimerManager().GetTimerElapsed(timerCameraArmPower);
	if (curveCameraHandleMov) {
		FVector curCamVecMov = curveCameraHandleMov->GetVectorValue(elapsed);
		FVector curCamVecRot = curveCameraHandleRot->GetVectorValue(elapsed);
		myCameraRoot->SetRelativeLocation(myCameraRoot->GetRelativeLocation() + curCamVecMov);
		myCameraRoot->SetRelativeRotation(FRotator(myCameraRoot->GetRelativeRotation().Pitch + curCamVecRot.Y,
																			    myCameraRoot->GetRelativeRotation().Yaw + curCamVecRot.Z,
																			    myCameraRoot->GetRelativeRotation().Roll + curCamVecRot.X));
	}
}

void AMyCharacter::ArmPowerCameraChangeEnd()
{
	isArmPowerTimeDilating = false;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.3f, FColor::Green, FString("D I L A T I O N   E N D"), false); }
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

// Jump
void AMyCharacter::Jump()
{
	if (addMoveState == EMoveAddState::ES_None && actionState == EActionState::ES_None && !isDead) {
		// OnMovementModeChanged - as alterative
		movementComponent->bNotifyApex = true;
		addMoveState = EMoveAddState::ES_JumpStart;
		isVulnarable = false;
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, FString("--- Jump Begin ---"), false); }
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && montageJump) {
			animInst->Montage_Play(montageJump);
			animInst->Montage_JumpToSection(FName("jump_begin"), montageJump);
		}
		Super::Jump();
	}
}

void AMyCharacter::NotifyJumpApex() {
	Super::NotifyJumpApex();
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Cyan, FString("--- Falling Down ---"), false); }
	addMoveState = EMoveAddState::ES_JumpFalling;
}

void AMyCharacter::CrouchBegin()
{
	if (currentWeapon->GetWeaponName() != "minigun" && addMoveState == EMoveAddState::ES_None && actionState == EActionState::ES_None && !isDead) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, FString("--- Crouch Begin ---"), false); }
		addMoveState = EMoveAddState::ES_Crouch;
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && montageCrouch) {
			animInst->Montage_Play(montageCrouch);
			animInst->Montage_JumpToSection(FName("crouch_begin"), montageCrouch);
		}
		Crouch();
	}
}

void AMyCharacter::CrouchEnd()
{
	if (currentWeapon->GetWeaponName() != "minigun" && addMoveState == EMoveAddState::ES_Crouch && !isDead) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, FString("--- Crouch End ---"), false); }
		addMoveState = EMoveAddState::ES_None;
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && montageCrouch) {
			animInst->Montage_Play(montageCrouch);
			animInst->Montage_JumpToSection(FName("crouch_end"), montageCrouch);
		}
		UnCrouch();
	}
}

void AMyCharacter::DoAction()
{
	if (actionState == EActionState::ES_None && !isDead) {
		// If Open Box
		if (currentWeaponbox && (currentWeaponbox->GetIsBoxOpened() == false)) {
			actionState = EActionState::ES_DoAction;
			isVulnarable = false;
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageAction) {
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Cyan, FString("Start Open Box"), false); }
				animInst->Montage_Play(montageAction);
				animInst->Montage_JumpToSection(FName("open_box"), montageAction);
				GetWorldTimerManager().SetTimer(timerPickAmmo, this, &AMyCharacter::OnEndOpenBox, montageAction->GetSectionLength(1));
			}
		}
		// If Pick Ammo
		else if (pickedAmmo) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageAction) {
				actionState = EActionState::ES_DoAction;
				isVulnarable = false;
				animInst->Montage_Play(montageAction);
				animInst->Montage_JumpToSection(FName("pick"), montageAction);
			}

			// If Accessoire
			if (pickedAmmo->GetPickedType() == EPickedType::ES_Accessoire) {
				if (*pickedItems.Find(pickedAmmo->GetAmmoName()) == true) {
					// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("ACCESSOIRE PICK"), false); }
				}
			}
			// If Aidkit or Ammo or Grenade
			else {
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("AMMO PICK"), false); }
			}

			GetWorldTimerManager().SetTimer(timerPickAmmo, this, &AMyCharacter::OnPickedAmmoBegin, montageAction->GetSectionLength(1) + 0.1);
		}
		// If Pick Weapon
		else if (pickedWeapon && (*pickedItems.Find(pickedWeapon->GetWeaponName()) == true)) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageAction) {
				actionState = EActionState::ES_DoAction;
				isVulnarable = false;
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Equip Weapon Started"), false); }
				animInst->Montage_Play(montageAction);
				animInst->Montage_JumpToSection(FName("pick"), montageAction);
				// Wait End Pick Weapon Montage Section (#2)
				GetWorldTimerManager().SetTimer(timerPickWeapon, this, &AMyCharacter::OnPickedWeaponBegin, montageAction->GetSectionLength(2));
			}
		}
	}
	// If Ready To Climb
	if (isReadyToClimbLadder && addMoveState != EMoveAddState::ES_Climbing) ClimbBegin();
	if (isReadyToLeaveLadder && addMoveState == EMoveAddState::ES_Climbing) ClimbEnd();
}

void AMyCharacter::SetCanPickAmmo(FString ammoName)
{
	if (pickedItems.Contains(ammoName)) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Cyan, FString("FOUND NAME"), false); }
		pickedItems[ammoName] = true;
	}
}

void AMyCharacter::OnEndOpenBox() {
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.4f, FColor::Cyan, FString("End Open Box"), false); }
	actionState = EActionState::ES_None;
	isVulnarable = true;
}

void AMyCharacter::OnPickedAmmoBegin() {
	if (pickedAmmo) {
		int32 addAmmoCount = pickedAmmo->GetAmmoCount();
		int32 remainAmmo = addAmmoCount;

		if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoHealth) {
			remainAmmo = health + addAmmoCount - healthMax;
			int32 oldHealth = health;
			health = FMath::Min(health + addAmmoCount, healthMax);
			if (health != oldHealth) {
				textPickForHUD[0] = "You Picked Medicine";
				textPickForHUD[1] = FString::Printf(TEXT("x %d"), health - oldHealth);
			}
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoGrenade) {
			if (countGrenades < countGrenadesMax) {
				remainAmmo = countGrenades + addAmmoCount - countGrenadesMax;
				countGrenades = FMath::Min(countGrenades + addAmmoCount, countGrenadesMax);
				textPickForHUD[0] = "You Picked Grenade";
				textPickForHUD[1] = FString::Printf(TEXT("x %d"), addAmmoCount);
			}
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoBomb) {
			if (countBombs < countBombsMax) {
				remainAmmo = countBombs + addAmmoCount - countBombsMax;
				countBombs = FMath::Min(countBombs + addAmmoCount, countBombsMax);
				textPickForHUD[0] = "You Picked Bomb";
				textPickForHUD[1] = FString::Printf(TEXT("x %d"), addAmmoCount);
			}
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoLaserPistol) {
			isPistolLaserEnable = true;
			remainAmmo = 0;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Pistol Laser is now enable"), false); }
			textFoundForHUD[0] = "newAccessorire"; // Text For HUD
			textFoundForHUD[1] = pickedAmmo->GetAmmoName(); // Text For HUD
			accessoireNum |= PISTOL_LASER_ENABLE_MAKE;
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoAR4Scope) {
			isAR4ScopeLaserEnable = true;
			remainAmmo = 0;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("AR4 Laser Scope is now enable"), false); }
			textFoundForHUD[0] = "newAccessorire"; // Text For HUD
			textFoundForHUD[1] = pickedAmmo->GetAmmoName(); // Text For HUD
			accessoireNum |= AR4_LASER_ENABLE_MAKE;
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoAR4Sight) {
			isAR4ScopeOpticalEnable = true;
			remainAmmo = 0;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("AR4 Optical Scope is now enable"), false); }
			textFoundForHUD[0] = "newAccessorire"; // Text For HUD
			textFoundForHUD[1] = pickedAmmo->GetAmmoName(); // Text For HUD
			accessoireNum |= AR4_OPTICAL_ENABLE_MAKE;
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoAR4Suppressor) {
			isAR4SuppressorEnable = true;
			remainAmmo = 0;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("AR4 Suppressor is now enable"), false); }
			textFoundForHUD[0] = "newAccessorire"; // Text For HUD
			textFoundForHUD[1] = pickedAmmo->GetAmmoName(); // Text For HUD
			accessoireNum |= AR4_SUPPR_ENABLE_MAKE;
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoCardCanion) {
			hasCardCanion = true;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Canyon Gates Are Accessed"), false); }
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoCardScifi) {
			hasCardScifi = true;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Tower Doors Are Accessed"), false); }
		}
		else if (pickedAmmo->GetAmmoType() == EAmmoType::ES_AmmoCardSnow) {
			hasCardSnow = true;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Central Gates Are Accessed"), false); }
		}
		else {
			for (auto* weapon : weapons) {
				if ((weapon != nullptr) && (weapon->GetAmmoType() == pickedAmmo->GetAmmoType()) && (weapon->GetAmmoCurrentWeapon() < weapon->GetAmmoCapacityWeapon())) {
					remainAmmo = weapon->GetAmmoCurrentWeapon() + addAmmoCount - weapon->GetAmmoCapacityWeapon();
					int32 oldAmmoCount = weapon->GetAmmoCurrentWeapon();
					weapon->ResetAmmoCurrentWeapon(FMath::Min(weapon->GetAmmoCurrentWeapon() + addAmmoCount, weapon->GetAmmoCapacityWeapon()));
					textPickForHUD[0] = FString("You Picked Ammo For ") + weapon->GetWeaponName();
					textPickForHUD[1] = FString::Printf(TEXT("x %d"), weapon->GetAmmoCurrentWeapon() - oldAmmoCount);
					break;
				}
			}
		}
		if (remainAmmo != addAmmoCount) pickedAmmo->SetAmmoCount(remainAmmo);
		if (remainAmmo <= 0) {
			pickedAmmo->Destroy(true);
		}
		pickedAmmo = nullptr;

		if (textFoundForHUD[0] != "") GetWorldTimerManager().SetTimer(timerResetTextFound, this, &AMyCharacter::OnTextFoundResetEnd, 4.f);
		if (textPickForHUD[0] != "") GetWorldTimerManager().SetTimer(timerResetTextPick, this, &AMyCharacter::OnTextPickResetEnd, 4.f);
		GetWorldTimerManager().SetTimer(timerPickAmmo, this, &AMyCharacter::OnPickedAmmoEnd, timeEquip);
	}
}

void AMyCharacter::OnPickedWeaponBegin() {
	if (pickedWeapon && pickedWeapon->GetWeaponName() != "pistol" && pickedWeapon->GetWeaponSkeletalMesh()) {
		FString w_name = pickedWeapon->GetWeaponName();
		int idx;
		// TODO: alternatively make it from table
		if (w_name == FString("SMG11")) { idx = 1; }
		else if (w_name == FString("AR4")) { idx = 2; }
		else if (w_name == FString("sniper")) { idx = 3; }
		else if (w_name == FString("minigun")) { idx = 4; }

		textFoundForHUD[0] = "newWeapon"; // Text For HUD
		textFoundForHUD[1] = w_name; // Text For HUD

		FString socketName = w_name + "HolderSocket";
		weapons[idx] = pickedWeapon;
		weapons[idx]->Equip(GetMesh(), socketName);
		weapons[idx]->SetWeaponPickupPhysics();
		weapons[idx]->SetWeaponVisibility(false);

		GetWorldTimerManager().SetTimer(timerPickWeapon, this, &AMyCharacter::OnPickedWeaponEnd, timeEquip);
		GetWorldTimerManager().SetTimer(timerResetTextFound, this, &AMyCharacter::OnTextFoundResetEnd, 4.f);
	}
}

void AMyCharacter::OnPickedWeaponEnd() {
	IncreaseCurrentXP(10.5); // Increase XP
	pickedWeapon = nullptr;
	actionState = EActionState::ES_None;
	isVulnarable = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Cyan, FString("Equip Finished"), false); }
}

void AMyCharacter::OnPickedAmmoEnd() {
	IncreaseCurrentXP(8.5); // increase XP
	pickedAmmo = nullptr;
	actionState = EActionState::ES_None;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Cyan, FString("Equip Finished"), false); }
}

void AMyCharacter::OnTextFoundResetEnd() {
	textFoundForHUD[0] = textFoundForHUD[1] = "";
}

void AMyCharacter::OnTextPickResetEnd() {
	textPickForHUD[0] = textPickForHUD[1] = "";
}

void AMyCharacter::OnTextHeadshotResetEnd() {
	textHeadshotForHUD = "";
}

void AMyCharacter::OnTextActionResetEnd()
{
	textActionForHUD = "";
}

void AMyCharacter::FirePressed() {
	if (actionState == EActionState::ES_None && currentWeapon && !isDead) {
		actionState = EActionState::ES_Fire;
		if (currentWeapon->GetCurrentFireMode() != EFireModeState::ES_Full) {
			switch (currentWeapon->GetCurrentFireMode()) {
			case EFireModeState::ES_Single: countMaxBulletsToSend = 1; break;
			case EFireModeState::ES_Triple: countMaxBulletsToSend = 3; break;
			case EFireModeState::ES_Semi: countMaxBulletsToSend = 10; break;
			}
		}
		if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoMinigun && currentWeapon->GetAmmoCurrentMagazine()) {
			isMinigunRollerRotates = true;
			GetWorldTimerManager().SetTimer(timerFireStartMinigun, this, &AMyCharacter::StartFireMinigun, minigunRollerDelay);
		}
		else StartFire();
	}
}

void AMyCharacter::StartFireMinigun()
{
	StartFire();
}

void AMyCharacter::StartFire() {
	if (currentWeapon->GetAmmoCurrentMagazine() && addMoveState != EMoveAddState::ES_JumpEnd) {
		if (actionState == EActionState::ES_Fire) {
			Fire();
			currentWeapon->DescreaseAmmoByOne();
			isFiring = false;
			float fr = (currentWeapon->GetWeaponFireFrequency());
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString::Printf(TEXT("fire freq: %f"), fr)); }
			if (currentWeapon->GetCurrentFireMode() != EFireModeState::ES_Full) {
				--countMaxBulletsToSend;
				if (countMaxBulletsToSend) {
					if (currentWeapon->GetCurrentFireMode() == EFireModeState::ES_Triple) fr /= 1.8;
				}
				else { return; }
			}
			GetWorldTimerManager().SetTimer(timerFireRifle, this, &AMyCharacter::CheckFiring, fr);
		}
	}
	else {
		if (currentWeapon->GetAmmoCurrentWeapon()) {
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("PRESS [ R ] TO R E L O A D WEAPON")); }
		}
		else {
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("A M M O IS NEEDED!")); }
		}
	}
}

void AMyCharacter::CheckFiring() {
	if (isFiring == false) {
		isFiring = true;
		StartFire();
	}
}

void AMyCharacter::Fire() {
	if (fireSound) { UGameplayStatics::PlaySound2D(this, fireSound); }

	FName sockFireName = "";
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst) {
		switch (weaponState) {
			case EWeaponState::ES_One:
				sockFireName = "firePistolSocket";
				GetWorldTimerManager().SetTimer(currentWeapon->GetTimerClip(), currentWeapon, 
																	  &AMyWeapon::OnClipEnd, currentWeapon->GetWeaponFireFrequency());
				break;
			case EWeaponState::ES_Two: sockFireName = "fireSMG11Socket"; break;
			case EWeaponState::ES_Three: sockFireName = "fireAR4Socket"; break;
			case EWeaponState::ES_Four: 
				sockFireName = "fireSniperSocket"; 
				GetWorldTimerManager().SetTimer(currentWeapon->GetTimerClip(), currentWeapon,
					&AMyWeapon::OnClipEnd, currentWeapon->GetWeaponFireFrequency());
				break;
			case EWeaponState::ES_Five: sockFireName = "fireMinigunSocket"; break;
		}
		if (animInst && montageFire) {
			FString secName = currentWeapon->GetWeaponName();
			// Crouch Fire
			if (addMoveState == EMoveAddState::ES_Crouch && currentWeapon->GetWeaponName() != "minigun") {
				animInst->Montage_Play(montageFire);
				animInst->Montage_JumpToSection(FName(secName+"_crouch"), montageFire);
			}
			// Stand Fire
			else {
				animInst->Montage_Play(montageFire);
				animInst->Montage_JumpToSection(FName(secName), montageFire);
			}
		}
	}

	const USkeletalMeshSocket* fireStartSocket = GetMesh()->GetSocketByName(sockFireName);
	if (fireStartSocket) {
		// Muzzle Particles
		const FTransform socketTransform = fireStartSocket->GetSocketTransform(GetMesh());
		if (muzzleParticles) {
			if (GetWorld()) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleParticles, socketTransform);
		}

		// Bind To Crosshair
		// Get current size of the viewport
		FVector2D viewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(viewportSize);
		}
		// Get screen space location of crosshairs
		FVector2D crosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
		FVector crosshairWorldPosition;
		FVector crosshairWorldDirection;

		// Get world position and direction of crosshairs
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldPosition, crosshairWorldDirection);

		if (bScreenToWorld) // was deprojection successful?
		{
			FHitResult screenTraceHit;
			const FVector start{ crosshairWorldPosition };
			const FVector end{ crosshairWorldPosition + crosshairWorldDirection * fireDistTrace };

			// Set beam end point to line trace end point
			FVector traceEndPoint{ end };

			// Trace outward from crosshairs world location
			if (GetWorld()) GetWorld()->LineTraceSingleByChannel(screenTraceHit, start, end, ECollisionChannel::ECC_Visibility);
			if (screenTraceHit.bBlockingHit) // was there a trace hit?
			{
				if (isDebugFireTrace) {
					if (GetWorld()) DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 3.f);
					if (GetWorld()) DrawDebugPoint(GetWorld(), screenTraceHit.Location, 8.f, FColor::Red, false, 3.f);
				}
				// Beam end point is now trace hit location
				traceEndPoint = screenTraceHit.Location;

				// 1. Enemy Damage
				AMyEnemyCharacter* damagedActor = Cast<AMyEnemyCharacter>(screenTraceHit.HitObjectHandle.GetManagingActor());
				if (damagedActor) {
					FName damagedBoneName = screenTraceHit.BoneName;
					EDamage damageBodyPart;
					if (damagedBoneName == FName("head")) {
						damageBodyPart = EDamage::ES_Head;
						textHeadshotForHUD = "Headshot";
						GetWorldTimerManager().SetTimer(timerResetTextHeadshot, this, &AMyCharacter::OnTextHeadshotResetEnd, 2.f);
					}
					else if (damagedBoneName.ToString().Contains("spine") || damagedBoneName.ToString().Contains("pelvis") || damagedBoneName.ToString().Contains("root")) {
						damageBodyPart = EDamage::ES_Core;
					}
					else {
						damageBodyPart = EDamage::ES_ArmLeg;
					}

					// Blood Particles
					if (damagedActor->GetEnemyBloodParticles()) { 
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), damagedActor->GetEnemyBloodParticles(), traceEndPoint); 
					}

					// increase XP
					damagedActor->OnHit(currentWeapon->GetDamage(), damageBodyPart, damagedBoneName, traceEndPoint);
					if (damagedActor->GetEnemyHealth() <= 0) {
						if (killSound) { UGameplayStatics::PlaySound2D(this, killSound); }
						AMyEnemyBeastCharacter* damagedBeastActor = Cast<AMyEnemyBeastCharacter>(damagedActor);
						if (damagedBeastActor) {
							// TODO: Alternatively Make Values As Params
							damagedBeastActor->GetEnemyName().Contains("Hell") ? IncreaseCurrentXP(40.5) : IncreaseCurrentXP(15.7);
						}
						else {
							IncreaseCurrentXP(3.4);
						}
					}
				}
				else {
					// 2. Explosive Barrel Damage
					AMyExplosiveActor* damagedBarrel= Cast<AMyExplosiveActor>(screenTraceHit.HitObjectHandle.GetManagingActor());
					if (damagedBarrel) {
						int32 damageValue;
						switch (currentWeapon->GetAmmoType()) {
						// TODO : alternatively read from table
						case EAmmoType::ES_AmmoPistol: damageValue = 15; break;
						case EAmmoType::ES_AmmoSMG11: damageValue = 20; break;
						case EAmmoType::ES_AmmoAR4: damageValue = 25; break;
						case EAmmoType::ES_AmmoSniper: damageValue = 50; break;
						case EAmmoType::ES_AmmoMinigun: damageValue = 12; break;
						default: damageValue = 0; break;
						}
						if (damagedBarrel->GetBarrelType() == EExplosionType::ES_ExplosiveArmed) damageValue /= 2;
						damagedBarrel->DamageBarrel(damageValue);
					}
					// 3. Damage Trigger
					else {
						AMyTrigger* triggerBox = Cast<AMyTrigger>(screenTraceHit.HitObjectHandle.GetManagingActor());
						if (triggerBox) {
							if (!triggerBox->IsTriggerActivated() && !triggerBox->IsDamageOnlyByRocket()) {
								switch (currentWeapon->GetAmmoType()) {
								case EAmmoType::ES_AmmoSniper:
								case EAmmoType::ES_AmmoMinigun:
									triggerBox->IncrementCountShotsCurrent();
									break;
								}
								triggerBox->IncrementCountShotsCurrent();
								if (triggerBox->GetCountShotsCurrent() == triggerBox->GetCountShotsRequired()) {
									IncreaseCurrentXP(5.7); // increase XP
								}
								// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FFFF00FF"), 
								//FString::Printf(TEXT("Need %d Shots"), triggerBox->GetCountShotsRequired()- triggerBox->GetCountShotsCurrent()), false); }
							}
						}
					}
				}
			}

			// Second trace from the rifle barrel
			FHitResult weaponTraceHit;
			const FVector weaponTraceStart{ socketTransform.GetLocation() };
			const FVector weaponTraceEnd{ traceEndPoint };
			if (GetWorld()) GetWorld()->LineTraceSingleByChannel(weaponTraceHit, weaponTraceStart, weaponTraceEnd, ECollisionChannel::ECC_Visibility);
			if (weaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint?
			{
				traceEndPoint = weaponTraceHit.Location;
			} 

			// Spawn Particles
			if (targetParticles)
			{
				if (GetWorld()) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), targetParticles, traceEndPoint);
			}
			if (traceParticles)
			{
				if (GetWorld()) {
					UParticleSystemComponent* trace = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), traceParticles, socketTransform);
					trace->SetVectorParameter(FName("Target"), traceEndPoint);
				}
			}
		}
	}
}

void AMyCharacter::FireReleased() {
	if (actionState == EActionState::ES_Fire && addMoveState != EMoveAddState::ES_JumpEnd && !isDead) {
		actionState = EActionState::ES_None;
		isFiring = false;
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Cyan, FString("Fire Stop"), false); }

		if (weaponState == EWeaponState::ES_Five) isMinigunRollerRotates = false;

		if (currentWeapon && currentWeapon->GetCurrentFireMode() != EFireModeState::ES_Full) {
			switch (currentWeapon->GetCurrentFireMode()) {
			case EFireModeState::ES_Single: countMaxBulletsToSend = 1; break;
			case EFireModeState::ES_Triple: countMaxBulletsToSend = 3; break;
			case EFireModeState::ES_Semi: countMaxBulletsToSend = 10; break;
			}
		}
	}
}

void AMyCharacter::UpdateCrosshairState()
{
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport) { GEngine->GameViewport->GetViewportSize(viewportSize); }
	FVector2D crosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	FVector crosshairWorldPosition;
	FVector crosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), 
																											 crosshairLocation, crosshairWorldPosition, crosshairWorldDirection);

	if (bScreenToWorld)
	{
		FHitResult screenTraceHit;
		const FVector start{ crosshairWorldPosition };
		const FVector end{ crosshairWorldPosition + crosshairWorldDirection * fireDistTrace };
		FVector traceEndPoint{ end };
		if (GetWorld()) GetWorld()->LineTraceSingleByChannel(screenTraceHit, start, end, ECollisionChannel::ECC_Visibility);
		
		// If AR4 Optical Scope Active
		if (currentWeapon) {
			if ((currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoAR4 && isWeaponTargeting && isAR4ScopeOpticalActive) || (isRocketLaunchTargeting)) {
				crosshairState = EHUDAimState::ES_Sniper;
			}
			else {
				if (screenTraceHit.bBlockingHit)
				{
					traceEndPoint = screenTraceHit.Location;
					// 1. Enemy Damage
					AMyEnemyCharacter* damagedActor = Cast<AMyEnemyCharacter>(screenTraceHit.HitObjectHandle.GetManagingActor());
					if (damagedActor) { crosshairState = EHUDAimState::ES_Target; }
					else {
						// 2. Explosive Barrel Damage
						AMyExplosiveActor* damagedBarrel = Cast<AMyExplosiveActor>(screenTraceHit.HitObjectHandle.GetManagingActor());
						if (damagedBarrel) { crosshairState = EHUDAimState::ES_Target; }
						else {
							// 3. Damage Trigger
							AMyTrigger* triggerBox = Cast<AMyTrigger>(screenTraceHit.HitObjectHandle.GetManagingActor());
							if (triggerBox) { crosshairState = EHUDAimState::ES_Target; }
							// 4. Another Object
							else { crosshairState = EHUDAimState::ES_Regular; }
						}
					}
				}
				else {
					crosshairState = EHUDAimState::ES_Regular;
				}
			}
		}
	}
}

void AMyCharacter::Targeting() {
	if (currentWeapon && !isDead) {
		isWeaponTargeting = true;
		// Zoom (FOV)
		if (weaponState == EWeaponState::ES_Three) {
			// TODO: Alternatively Make Values As Params
			if (isAR4ScopeOpticalActive) { cameraFOVZoom = currentWeapon->GetWeaponFOV() - 30.f; }
			else if (isAR4ScopeLaserActive) { cameraFOVZoom = currentWeapon->GetWeaponFOV() - 15.f; }
		}
		else { cameraFOVZoom = currentWeapon->GetWeaponFOV(); }
		myCamera->SetFieldOfView(cameraFOVZoom);
		if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoSniper) {
			GetWorldTimerManager().SetTimer(currentWeapon->GetTimerSightMoveUp(), currentWeapon, &AMyWeapon::OnSightMoveUpEnd, currentWeapon->GetSightMoveDT());
		}
	}
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Targeting"), false); }	
}

void AMyCharacter::TargetingStop() {
	if (currentWeapon && !isDead) {
		if (currentWeapon->GetAmmoType() == EAmmoType::ES_AmmoSniper) {
			GetWorldTimerManager().SetTimer(currentWeapon->GetTimerSightMoveDown(), currentWeapon, &AMyWeapon::OnSightMoveDownEnd, currentWeapon->GetSightMoveDT());
		}
		isWeaponTargeting = false;
		myCamera->SetFieldOfView(cameraFOVDefault);
	}
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Cyan, FString("Targeting Stop"), false); }
}

void AMyCharacter::Grenade() {
	if (actionState == EActionState::ES_None && isThrowGrenadeEnable == true && currentWeapon->GetWeaponName() != "minigun" && !isDead) {
		if (countGrenades) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageGrenade) {
				if (!animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0))) {
					FTransform newThrowTransform = GetMesh()->GetSocketTransform("grenadeBombSocket");
					AMyGrenadeBomb* newGrenade = GetWorld()->SpawnActor<AMyGrenadeBomb>(GrenadeToSpawn, newThrowTransform);
					if (newGrenade) { 
						actionState = EActionState::ES_Grenade; 
						FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
						newGrenade->SetGrenadeThrowParams(0.5f, 2.5f, GetActorForwardVector() *1700.f);
						newGrenade->GetRootMeshSkeletal()->SetVisibility(false);
						newGrenade->GetRootMeshSkeletal()->AttachToComponent(this->GetMesh(), rulesA, FName("grenadeBombSocket"));
						animInst->Montage_Play(montageGrenade);
						animInst->Montage_JumpToSection(FName("grenade"), montageGrenade);
						--countGrenades;
						GetWorldTimerManager().SetTimer(timerGrenadeAmin, this, &AMyCharacter::OnGrenadeAnimReactivated, timeGrenadeAnimReactivation);
						GetWorldTimerManager().SetTimer(newGrenade->GetGrenadeTimerVisibility(), newGrenade, &AMyGrenadeBomb::SetGrenadeVisible, 0.2f); // experimental value from anim
					}
				}
			}
		}
		else {
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, FString("No Grenades!"), false); }
		}
	}
}

void AMyCharacter::OnGrenadeAnimReactivated() {
	actionState = EActionState::ES_None;
	isThrowGrenadeEnable = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Grenade Throw is Now Enable"), false); }
}

void AMyCharacter::Bomb() {
	if (actionState == EActionState::ES_None && isThrowBombEnable == true && currentWeapon->GetWeaponName() != "minigun" && !isDead) {
		if (countBombs) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageGrenade) {
				if (!animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0))) {
					FTransform newThrowTransform = GetMesh()->GetSocketTransform("grenadeBombSocket");
					AMyGrenadeBomb* newGrenade = GetWorld()->SpawnActor<AMyGrenadeBomb>(GrenadeToSpawn, newThrowTransform);
					if (newGrenade) {
						actionState = EActionState::ES_Bomb;
						FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
						newGrenade->SetGrenadeThrowParams(0.5f, 4.0f, GetActorForwardVector() * 900.f);
						newGrenade->GetRootMeshSkeletal()->SetVisibility(false);
						newGrenade->GetRootMeshSkeletal()->AttachToComponent(this->GetMesh(), rulesA, FName("grenadeBombSocket"));
						animInst->Montage_Play(montageGrenade);
						animInst->Montage_JumpToSection(FName("bomb"), montageGrenade);
						--countBombs;
						GetWorldTimerManager().SetTimer(timerBombAnim, this, &AMyCharacter::OnBombAnimReactivated, timeBombAnimReactivation);
						GetWorldTimerManager().SetTimer(newGrenade->GetGrenadeTimerVisibility(), newGrenade, &AMyGrenadeBomb::SetGrenadeVisible, 0.2f); // experimental value from anim
					}
				}
			}
		}
		else {
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, FString("No Bombs!"), false); }
		}
	}
}

void AMyCharacter::OnBombAnimReactivated() {
	actionState = EActionState::ES_None;
	isThrowBombEnable = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("Bomb Throw is Now Enable"), false); }
}

void AMyCharacter::ReloadBegin() {
	if (actionState == EActionState::ES_None && addMoveState == EMoveAddState::ES_None && actionState == EActionState::ES_None && !isDead) {
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst) {
			if (!animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0)) &&
				(currentWeapon->GetAmmoCurrentMagazine() < currentWeapon->GetAmmoCapacityMagazine())) {
				int32 deltaAmmoNeeded = currentWeapon->GetAmmoCapacityMagazine() - currentWeapon->GetAmmoCurrentMagazine();
				deltaAmmoEnable = (currentWeapon->GetAmmoCurrentWeapon() >= deltaAmmoNeeded ? deltaAmmoNeeded : currentWeapon->GetAmmoCurrentWeapon());
				if (deltaAmmoEnable) {
					actionState = EActionState::ES_Reload;
					// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Magenta, FString("Reload Start"), false); }
					animInst->Montage_Play(montageReloadWeapon);
					animInst->Montage_JumpToSection(FName(currentWeapon->GetWeaponName()), montageReloadWeapon);
					int32 secIdx = montageReloadWeapon->GetSectionIndex(FName(currentWeapon->GetWeaponName()));
					GetWorldTimerManager().SetTimer(timerReload, this, &AMyCharacter::OnReloadEnd, montageReloadWeapon->GetSectionLength(secIdx));
				}
			}
		}
	}
}

void AMyCharacter::OnReloadEnd() {
	currentWeapon->SetAmmoCurrentWeapon(deltaAmmoEnable);
	currentWeapon->SetAmmoCurrentMagazine(deltaAmmoEnable);
	actionState = EActionState::ES_None;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Magenta, FString("Reload End"), false); }
}

// --------------------------- Skill 1. Ulta Shoulder Rocket Launch -------------------------- 
void AMyCharacter::Ulta() {
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (!isDead && actionState == EActionState::ES_None && isUltaEnable && animInst && montageExtra && !animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0))) {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Magenta, FString("Ulta"), false); }
		actionState = EActionState::ES_Ulta;
		isVulnarable = false;
		isUltaEnable = false;
		animInst->Montage_Play(montageExtra);
		animInst->Montage_JumpToSection(FName("ulta"), montageExtra);
	}
}

void AMyCharacter::LaunchRocket()
{
	if (fireUltaSound) { UGameplayStatics::PlaySound2D(this, fireUltaSound); }
	const USkeletalMeshSocket* fireStartSocket = GetMesh()->GetSocketByName("fireStartUltaSocket");
	if (fireStartSocket) {
		// Muzzle Particles
		const FTransform socketTransform = fireStartSocket->GetSocketTransform(GetMesh());
		if (muzzleUltaParticles) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleUltaParticles, socketTransform); }
		// Bind To Crosshair
		// Get current size of the viewport
		FVector2D viewportSize;
		if (GEngine && GEngine->GameViewport) { GEngine->GameViewport->GetViewportSize(viewportSize); }
		// Get screen space location of crosshairs
		FVector2D crosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
		//crosshairLocation.Y -= 50.f;
		FVector crosshairWorldPosition;
		FVector crosshairWorldDirection;
		// Get world position and direction of crosshairs
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
			crosshairLocation,
			crosshairWorldPosition,
			crosshairWorldDirection);
		// was deprojection successful
		if (bScreenToWorld) {
			FHitResult screenTraceHit;
			const FVector start{ crosshairWorldPosition };
			const FVector end{ crosshairWorldPosition + crosshairWorldDirection * fireDistTrace };
			// Set beam end point to line trace end point
			FVector traceEndPoint{ end };
			// Trace outward from crosshairs world location
			GetWorld()->LineTraceSingleByChannel(screenTraceHit, start, end, ECollisionChannel::ECC_Visibility);
			if (screenTraceHit.bBlockingHit) // was there a trace hit?
			{
				if (isDebugFireTrace) {
					DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 3.f);
					DrawDebugPoint(GetWorld(), screenTraceHit.Location, 8.f, FColor::Blue, false, 3.f);
				}
				// Beam end point is now trace hit location
				traceEndPoint = screenTraceHit.Location;

				// Damage (100% Enemy Death -> Exception Beast)
				AMyEnemyCharacter* damagedActor = Cast<AMyEnemyCharacter>(screenTraceHit.HitObjectHandle.GetManagingActor());
				if (damagedActor) {
					damagedActor->Damage(EDamage::ES_Rocket, 0.0, traceEndPoint);
				}
				// Trigger Damage
				else {
					AMyTrigger* triggerBox = Cast<AMyTrigger>(screenTraceHit.HitObjectHandle.GetManagingActor());
					if (triggerBox) {
						if (!triggerBox->IsTriggerActivated()) {
							// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FFFF00FF"), FString("Trigger Is Under Fire"), false); }
							for (int i = 0; i < triggerBox->GetCountShotsRequired(); ++i) triggerBox->IncrementCountShotsCurrent();
						}
					}
				}
			}
			// Perform a second trace, this time from the gun barrel
			FHitResult weaponTraceHit;
			const FVector weaponTraceStart{ socketTransform.GetLocation() };
			const FVector weaponTraceEnd{ traceEndPoint };
			GetWorld()->LineTraceSingleByChannel(weaponTraceHit, weaponTraceStart, weaponTraceEnd, ECollisionChannel::ECC_Visibility);
			// is object between barrel and BeamEndPoint
			if (weaponTraceHit.bBlockingHit) { traceEndPoint = weaponTraceHit.Location; }
			if (targetUltaParticles) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), targetUltaParticles, traceEndPoint); }
			if (traceUltaParticles)
			{
				UParticleSystemComponent* trace = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), traceUltaParticles, socketTransform);
				if (trace) { trace->SetVectorParameter(FName("Target"), traceEndPoint); }
			}
		}
	}
	GetWorldTimerManager().SetTimer(timerEmoCaboom, this, &AMyCharacter::OnUltaDeactivated, timeUltaDeactivation);
}

void AMyCharacter::OnUltaDeactivated() {
	actionState = EActionState::ES_None;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("Ulta Rocket is Deactivated"), false); }
	GetWorldTimerManager().SetTimer(timerEmoCaboom, this, &AMyCharacter::OnUltaReactivated, timeUltaReactivation);
}

void AMyCharacter::OnUltaReactivated() {
	isUltaEnable = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("Ulta Rocket is Now Enable"), false); }
}

// --------------------------- Skill 2. Arm Shock Wave -------------------------- 
void AMyCharacter::ArmShockWave()
{
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (actionState == EActionState::ES_None && isArmShockEnable && montageExtra && !animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0)) && !isDead) {
		actionState = EActionState::ES_ArmShockWave;
		isVulnarable = false;
		HideWeapon();
	}
}

void AMyCharacter::OnArmShockWaveStart()
{
		isArmShockEnable = false;
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("Arm Shock Wave"), false); }
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		animInst->Montage_Play(montageExtra);
		animInst->Montage_JumpToSection(FName("armpower"), montageExtra);
}

void AMyCharacter::ApplyShockWaveStart()
{
	// Collect Enemies
	TArray<AActor*> allEnemies;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AMyEnemyCharacter::StaticClass(), allEnemies);
	FVector armLocation = this->GetActorLocation();
	// Spawn Explosion Wave
	ARadialForceActor* force = GetWorld()->SpawnActor<ARadialForceActor>();
	if (force) {
		force->GetRootComponent()->SetWorldLocation(armLocation);
		URadialForceComponent* forceComp = force->GetForceComponent();
		if (forceComp) {
			forceComp->Radius = armForceRadius;
			forceComp->ImpulseStrength = armImpulseStrength;
			forceComp->ForceStrength = armForceStrength;
			forceComp->Falloff = RIF_Linear;
			forceComp->bImpulseVelChange = true;
			forceComp->bIgnoreOwningActor = true;
			forceComp->SetActive(true);
			forceComp->FireImpulse();
		}
	}
	// Damage Enemies
	for (AActor* actor : allEnemies) {
		AMyEnemyCharacter* enemy = Cast<AMyEnemyCharacter>(actor);
		if (enemy) {
			float distToEpicenter = this->GetDistanceTo(enemy);
			if (distToEpicenter < radiusArmShockWave) {
				float damageByDist = 1.f - distToEpicenter / radiusArmShockWave;
				enemy->Damage(EDamage::ES_ArmShockwave, damageByDist);
			}
		}
	}
	if (force) force->Destroy();
	GetWorldTimerManager().SetTimer(timerArmShockWave, this, &AMyCharacter::OnArmShockWaveDeactivated, timeArmShockWaveDeactivation);
}

void AMyCharacter::OnOpenBox()
{
	if (currentWeaponbox) currentWeaponbox->OpenBox();
}

void AMyCharacter::Menu()
{
	isGameMenuShown = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AMyCharacter::OnArmShockWaveDeactivated() {
	actionState = EActionState::ES_None;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("Ulta Rocket is Deactivated"), false); }
	GetWorldTimerManager().SetTimer(timerArmShockWave, this, &AMyCharacter::OnArmShockWaveReactivated, timeArmShockWaveReactivation);
}

void AMyCharacter::OnArmShockWaveReactivated()
{
	isArmShockEnable = true;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("ArmShockWave is Now Enable"), false); }
}

// -----------------------------Skill 3. Ultra Vision ----------------
void AMyCharacter::UltraVision()
{
	if (actionState == EActionState::ES_None && !isDead) {
		if (isUltraVisionEnable) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageExtra) {
				if (!animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0))) {
					isUltraVisionEnable = false;
					actionState = EActionState::ES_UltraVision;
					isVulnarable = false;
					// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("Ultra Vision Activated"), false); }
					GetWorldTimerManager().SetTimer(timerUltraVision, this, &AMyCharacter::OnUltraVisionDeactivated, timeUltraVisionDuration);
					animInst->Montage_Play(montageExtra);
					animInst->Montage_JumpToSection(FName("ultravision"), montageExtra);
				}
			}
		}
	}
}

void AMyCharacter::OnUltraVisionDeactivated()
{
	isUltraVisionDeactivated = true;
	actionState = EActionState::ES_None;
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageExtra) {
		animInst->Montage_Play(montageExtra);
		animInst->Montage_JumpToSection(FName("ultravision"), montageExtra);
	}
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("UltraVision is Deactivated"), false); }
	GetWorldTimerManager().SetTimer(timerUltraVision, this, &AMyCharacter::OnUltraVisionReactivated, timeUltraVisionReactivation);
}

void AMyCharacter::OnUltraVisionReactivated()
{
	isUltraVisionEnable = true;
	isUltraVisionDeactivated = false;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("UltraVision is Enable Again"), false); }
}

// -----------------------------Skill 4. Dome Shield ----------------
void AMyCharacter::DomeShield()
{
	if (actionState == EActionState::ES_None && !isDead) {
		if (isDomeShieldEnable && p_DomeShield) {
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && montageExtra) {
				if (!animInst->IsAnyMontagePlaying() && (movementComponent->Velocity == FVector(0, 0, 0))) {
					isDomeShieldEnable = false;
					isVulnarable = false;
					actionState = EActionState::ES_DomeShield;
					p_DomeShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
					p_DomeShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
					// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("Dome Shield"), false); }
					animInst->Montage_Play(montageExtra);
					animInst->Montage_JumpToSection(FName("hexdome_start"), montageExtra);
					GetWorldTimerManager().SetTimer(timerDomeShield, this, &AMyCharacter::OnDomeShieldDeactivated, timeDomeShieldDuration);
				}
			}
		}
	}
}

void AMyCharacter::OnDomeShieldDeactivated()
{
	isDomeShieldDeactivated = true;
	actionState = EActionState::ES_None;
	p_DomeShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("DomeShield is Deactivated"), false); }
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageExtra) {
		animInst->Montage_Play(montageExtra);
		animInst->Montage_JumpToSection(FName("hexdome_end"), montageExtra);
	}
	GetWorldTimerManager().SetTimer(timerDomeShield, this, &AMyCharacter::OnDomeShieldReactivated, timeDomeShieldReactivation);
}

void AMyCharacter::OnDomeShieldReactivated()
{
	isDomeShieldEnable = true;
	isDomeShieldDeactivated = false;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("DomeShield is Enable Again"), false); }
}

void AMyCharacter::SetDamage(int32 deltaHealth, bool isDamageByEnemy)
{
	if (!isVulnarable || isDead) return;

	//isDamage = true;
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst) {
		health -= deltaHealth;
		if (health > 0) {
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("Hero Damage"), false); }
			animInst->Montage_Play(montageDamage);
			int32 idx = montageDamage->GetSectionIndex(FName(currentWeapon->GetWeaponName()));
			animInst->Montage_JumpToSection(FName(currentWeapon->GetWeaponName()), montageDamage);
			GetWorldTimerManager().SetTimer(timerDamageBegin, this, &AMyCharacter::OnEndDamage, montageDamage->GetSectionLength(idx));
		}
		else {
			health = 0;
			isDead = true;
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("!!! You are killed"), false); }
			if (montageDeath) { 
				animInst->Montage_Play(montageDeath);
				// If Not Killed By Enemy
				GetWorldTimerManager().SetTimer(timerDeathBegin, this, &AMyCharacter::OnDeathBegin, 4.f);
			}
		}
	}
}

void AMyCharacter::OnEndDamage()
{
	isDamage = false;
}

void AMyCharacter::OnDeathBegin() {
	isRespawnMenuShown = true;
	isPersWidgetShown = false;
}

void AMyCharacter::Respawn()
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("-- RESPAWN --"), false); }
	UGameplayStatics::OpenLevel(GetWorld(), "FirstPersonMap");
	this->SetActorLocation(lastAutosaveLocation);
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageRespawn) { animInst->Montage_Play(montageRespawn); }
}

// ------------------------------------------------------------ CLIMB BEGIN ----------------------------------------------------------- //
void AMyCharacter::ClimbBegin()
{
	addMoveState = EMoveAddState::ES_ClimbStart;
	isVulnarable = false;
	movementComponent->GravityScale = 0.f;
	movementComponent->MaxStepHeight = MaxStepHeightClimbing;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, FString("START ATTACHING TO LADDER"), false); }
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageHideWeapon) {
		animInst->Montage_Play(montageHideWeapon);
		animInst->Montage_JumpToSection(FName(currentWeapon->GetWeaponName()), montageHideWeapon);
	}
	GetWorldTimerManager().SetTimer(timerClimb, this, &AMyCharacter::OnClimbBegin, timeClimbBegin);
}

void AMyCharacter::OnClimbBegin()
{
	addMoveState = EMoveAddState::ES_Climbing;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("NOW READY TO MOVE UP/DOWN"), false); }
}

void AMyCharacter::ClimbEnd()
{
	addMoveState = EMoveAddState::ES_ClimbEnd;
	movementComponent->GravityScale = 1.0f;
	movementComponent->MaxStepHeight = 45.0f;
	// Jump Side From Ladder
	if (isReadyToLeaveLadder) {
		FVector upVector = GetRootComponent()->GetUpVector();
		FVector rightVector = GetRootComponent()->GetRightVector();
		FVector impulseVector = (upVector + rightVector) * 400.f; // 400 - force of jump
		movementComponent->AddImpulse(impulseVector, true);
	}
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, FString("START DETTACHING FROM LADDER"), false); }
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	OnUnHideWeapon();
	GetWorldTimerManager().SetTimer(timerClimb, this, &AMyCharacter::OnClimbEnd, timeClimbEnd);
}

void AMyCharacter::OnClimbEnd()
{
	addMoveState = EMoveAddState::ES_None;
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("DETTACHED FROM LADDER"), false); }
}
// ------------------------------------------------------------ CLIMB END  ----------------------------------------------------------- //

void AMyCharacter::MoveForward(float Value) {
	if (Controller && Value != 0.f && !isDead) {
		// Moving Forward & Back
		if (addMoveState != EMoveAddState::ES_Climbing) {
			if ((addMoveState == EMoveAddState::ES_Crouch || addMoveState == EMoveAddState::ES_None) &&
				(actionState == EActionState::ES_None || actionState == EActionState::ES_Fire || 
				 actionState == EActionState::ES_UltraVision || actionState == EActionState::ES_DomeShield)) {
					 if (Value > 0.f) { baseMoveXState = EMoveXState::ES_MoveForward; }
					 else if (Value < 0.f) { baseMoveXState = EMoveXState::ES_MoveBack; }
					 FVector forw = GetActorForwardVector();
					 AddMovementInput(forw, Value);
				}
		}
		// Moving Up & Down
		else {
			climbVelZ = Value * MaxStepHeightClimbing;
			AddActorWorldOffset(FVector(0.f, 0.f, climbVelZ));
		}
	}
	// No Move
	else {
		baseMoveXState = EMoveXState::ES_Idle;
		climbVelZ = 0.f;
	}
}

void AMyCharacter::MoveRight(float Value) {
	if (Controller && Value != 0.f && (addMoveState == EMoveAddState::ES_Crouch || addMoveState == EMoveAddState::ES_None) && 
		(actionState == EActionState::ES_None || actionState == EActionState::ES_Fire || actionState == EActionState::ES_UltraVision || actionState == EActionState::ES_DomeShield) && !isDead) {
		if (Value > 0.f) {
			baseMoveYState = EMoveYState::ES_MoveRight;
			if (baseMoveYState == EMoveYState::ES_MoveRight) {
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("Right"), false); }
			}
		}
		else if (Value < 0.f) {
			baseMoveYState = EMoveYState::ES_MoveLeft;
			if (baseMoveYState == EMoveYState::ES_MoveLeft) {
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("Left"), false); }
			}
		}
		FVector right = GetActorRightVector();
		AddMovementInput(right, Value);
		return;
	}
	baseMoveYState = EMoveYState::ES_Idle;
}

void AMyCharacter::LookUp(float Value) {
	if (!isDead) {
		double pitch = GetActorRotation().Pitch;
		float delta = 0; //(isRidingSnowboard ? 0 : ((pitch <= 35.f && pitch >= -20.f) ? 0 : (pitch > 0 ? 1 : -1)));
		AddControllerPitchInput(-Value + delta);
	}
}

void AMyCharacter::Turn(float Value) {
	if (!isDead) {
		AddControllerYawInput(Value);
	}
}

void AMyCharacter::FireMode() {
	if (!isDead && currentWeapon) {
		currentWeapon->ChangeFireMode();
		switch (currentWeapon->GetCurrentFireMode()) {
		case EFireModeState::ES_Single:
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("FM: SINGLE"), false); }
			break;
		case EFireModeState::ES_Triple:
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("FM: TRIPLE"), false); }
			break;
		case EFireModeState::ES_Semi:
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("FM: SEMI"), false); }
			break;
		case EFireModeState::ES_Full:
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("FM: FULL"), false); }
			break;
		}
	}
}

void AMyCharacter::ChangeWeapon(FString weaponName, bool isAfterAction) {
	if (currentWeapon && addMoveState == EMoveAddState::ES_None && !isDead) {
		if ((currentWeapon->GetWeaponName() == weaponName) && !isAfterAction) return;
		isVulnarable = false;
		movementComponent->MaxWalkSpeed = 650.f;
		for (auto* w : weapons) {
			if (w) {
				if (w->GetWeaponName() == weaponName) {
					// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, weaponName, false); }

					if (weaponName == "pistol") weaponState = EWeaponState::ES_One;
					else if (weaponName == "SMG11") weaponState = EWeaponState::ES_Two;
					else if (weaponName == "AR4") weaponState = EWeaponState::ES_Three;
					else if (weaponName == "sniper") weaponState = EWeaponState::ES_Four;
					else if (weaponName == "minigun") {
						weaponState = EWeaponState::ES_Five;
						movementComponent->MaxWalkSpeed = 280.f;
					}

					currentWeapon->SetWeaponVisibility(false);
					currentWeapon = w;
					//currentWeapon->SetWeaponVisibility(true);

					movementComponent->MaxWalkSpeed = currentWeapon->GetSpeedWalk();

					FString socketName = currentWeapon->GetWeaponName() + "ArmSocket";
					currentWeapon->Equip(GetMesh(), socketName);
					UAnimInstance* animInst = GetMesh()->GetAnimInstance();
					if (animInst && montageChangeWeapon) {
						animInst->Montage_Play(montageChangeWeapon);
						animInst->Montage_JumpToSection(FName(currentWeapon->GetWeaponName()), montageChangeWeapon);
					}
					return;
				}
			}
		}
	}
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString("No Weapon In Stack"), false); }
}

void AMyCharacter::ChooseWeaponOne() {
	ChangeWeapon("pistol", false);
}

void AMyCharacter::ChooseWeaponTwo() {
	ChangeWeapon("SMG11", false);
}

void AMyCharacter::ChooseWeaponThree() {
	ChangeWeapon("AR4", false);
}

void AMyCharacter::ChooseWeaponFour() {
	ChangeWeapon("sniper", false);
}

void AMyCharacter::ChooseWeaponFive() {
	ChangeWeapon("minigun", false);
}

void AMyCharacter::HideWeapon()
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Cyan, FString("Hide Weapon"), false); }
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && montageHideWeapon) {
		animInst->Montage_Play(montageHideWeapon);
		animInst->Montage_JumpToSection(FName(currentWeapon->GetWeaponName()), montageHideWeapon);
	}
}

void AMyCharacter::OnHideWeapon()
{
	if (currentWeapon) {
		currentWeapon->SetWeaponVisibility(false);
	}
}

void AMyCharacter::OnUnHideWeapon()
{
	if (currentWeapon) {
		currentWeapon->SetWeaponVisibility(true);
	}
}

void AMyCharacter::OnHandleMagazineBegin()
{
	if (currentWeapon && magazineComponent) {
		isMagazineMoves = true;
		FAttachmentTransformRules rulesA(EAttachmentRule::KeepRelative, true);
		int32 idx = currentWeapon->GetWeaponSkeletalMesh()->GetBoneIndex("b_gun_mag");
		transformMagazine = currentWeapon->GetWeaponSkeletalMesh()->GetBoneTransform(idx);
		magazineComponent->AttachToComponent(this->GetMesh(), rulesA, FName("reloadMagSocket"));
		magazineComponent->SetWorldTransform(transformMagazine);
	}
}

void AMyCharacter::OnHandleMagazineEnd()
{
	if (currentWeapon && magazineComponent) {
		isMagazineMoves = false;
		FAttachmentTransformRules rulesA(EAttachmentRule::KeepRelative, true);
		int32 idx = currentWeapon->GetWeaponSkeletalMesh()->GetBoneIndex("b_gun_mag");
		transformMagazine = currentWeapon->GetWeaponSkeletalMesh()->GetBoneTransform(idx);
		magazineComponent->AttachToComponent(currentWeapon->GetWeaponSkeletalMesh(), rulesA, FName("magWeaponSocket"));
		magazineComponent->SetWorldTransform(transformMagazine);
	}
}

void AMyCharacter::WeaponConfigure()
{
	isWeaponConfig = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AMyCharacter::OnMovieActionBegin()
{
	movieState = EMovieActionState::ES_Start;
	GetWorldTimerManager().SetTimer(timerMovieAction, this, &AMyCharacter::OnMovieActionPeak, targetingMovieBeginDelay);
}

void AMyCharacter::OnMovieActionPeak()
{
	movieState = EMovieActionState::ES_Peak;
	GetWorldTimerManager().SetTimer(timerMovieAction, this, &AMyCharacter::OnMovieActionEnd, targetingMoviePeakDelay);
}

void AMyCharacter::OnMovieActionEnd()
{
	movieState = EMovieActionState::ES_End;
}

void AMyCharacter::UpdateMovieActionFOV(float DeltaTime)
{
	if (movieState == EMovieActionState::ES_Start) {
		cameraFOVCurrent = FMath::FInterpTo(cameraFOVCurrent, cameraMovieZoom, DeltaTime, targetingMovieSpeed);
		myCamera->SetFieldOfView(cameraFOVCurrent);

		camRootXCur = FMath::FInterpTo(myCameraRoot->GetRelativeLocation().X, myCameraRoot->GetRelativeLocation().X + camRootXDelta, DeltaTime, targetingMovieSpeed * 2.f);
		myCameraRoot->SetRelativeLocation(FVector(camRootXCur, myCameraRoot->GetRelativeLocation().Y, myCameraRoot->GetRelativeLocation().Z));
	}
	else if (movieState == EMovieActionState::ES_Peak) {
		cameraFOVCurrent = FMath::FInterpTo(cameraFOVCurrent, cameraFOVDefault, DeltaTime, targetingMovieSpeed);
		myCamera->SetFieldOfView(cameraFOVCurrent);

		camRootXCur = FMath::FInterpTo(myCameraRoot->GetRelativeLocation().X, camRootXDefault, DeltaTime, targetingMovieSpeed * 2.f);
		myCameraRoot->SetRelativeLocation(FVector(camRootXCur, myCameraRoot->GetRelativeLocation().Y, myCameraRoot->GetRelativeLocation().Z));
	}
}