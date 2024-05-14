#include "MyWeaponBox.h"
#include "MyWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AMyWeaponBox::AMyWeaponBox()
{
 	PrimaryActorTick.bCanEverTick = true;

	openboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("body"));
	RootComponent = openboxBody;

	openboxInteract = CreateDefaultSubobject<UBoxComponent>(TEXT("boxInteract"));
	openboxInteract->SetupAttachment(openboxBody);

	openboxCap = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("cap"));
	openboxCap->SetupAttachment(openboxBody);

	openboxCap->SetSimulatePhysics(true);
	openboxCap->SetMassOverrideInKg("", 20.f);
	openboxCap->SetEnableGravity(false);
	openboxCap->SetGenerateOverlapEvents(true);
	openboxCap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	openboxCap->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	openboxCap->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	isOpened = false;
}

void AMyWeaponBox::BeginPlay()
{
	Super::BeginPlay();
}

void AMyWeaponBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyWeaponBox::OnOpenboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{}

void AMyWeaponBox::OnOpenboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{}

void AMyWeaponBox::OpenBox()
{
	FVector impluseVec{ 2000.f, 700.f, 300.f };
	openboxCap->AddImpulseAtLocation(impluseVec, this->GetActorLocation());
	GetWorldTimerManager().SetTimer(timerOpenBox, this, &AMyWeaponBox::OnImpulseApplied, 0.6f);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, FString("IMPULSE APPLIED"), false); }
}

void AMyWeaponBox::OnImpulseApplied()
{
	openboxCap->SetEnableGravity(true);
	isOpened = true;
	
	AMyCharacter* p_Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (p_Player && (ammoCanPickName.IsEmpty() == false)) {
		p_Player->SetCanPickAmmo(ammoCanPickName);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, FString("IMPULSE FINISHED"), false); }
	
		// TODO: Can be simplified
		// Activate Weapon Interact
		TArray<AActor*> weapons;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AMyWeapon::StaticClass(), weapons);
		for (AActor* w : weapons) {
			AMyWeapon* weapon = Cast<AMyWeapon>(w);
			if (weapon->GetWeaponName() == ammoCanPickName) {
				weapon->ActivateWeaponBoxInteract();
				break;
			}
		}

		// Activate Ammo Interact
		TArray<AActor*> ammos;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AMyAmmo::StaticClass(), ammos);
		for (AActor* am : ammos) {
			AMyAmmo* ammo = Cast<AMyAmmo>(am);
			if (ammo->GetAmmoName()  == ammoCanPickName) {
				ammo->ActivateAmmoBoxInteract();
				break;
			}
		}
	}
}