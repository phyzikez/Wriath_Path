#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule
	myCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("theCapsule"));
	myCapsule->SetCapsuleHalfHeight(100.f);
	myCapsule->SetCapsuleRadius(50.f);
	SetRootComponent(myCapsule);

	// Skeletal
	myMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("theSkeletalMesh"));
	myMesh->SetupAttachment(GetRootComponent());
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Camera Foot
	myCameraRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("theCameraRoot"));
	myCameraRoot->SetupAttachment(myCapsule);

	// Camera
	myCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("theCamera"));
	myCamera->SetupAttachment(myCameraRoot);
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPawn::MoveForward(float Value) {
	if (Controller && Value != 0.f) {
		FVector forw = GetActorForwardVector();
		AddMovementInput(forw, Value);
	}
}

void AMyPawn::MoveLeft(float Value) {
	if (Controller && Value != 0.f) {
		FVector right = GetActorRightVector();
		AddMovementInput(right, -Value);
	}
}

void AMyPawn::LookUp(float Value) {
	AddControllerPitchInput(-Value);
}

void AMyPawn::Turn(float Value) {
	AddControllerYawInput(Value);
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveLeft"), this, &AMyPawn::MoveLeft);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyPawn::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyPawn::Turn);
}