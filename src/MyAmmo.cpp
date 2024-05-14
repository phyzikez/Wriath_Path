#include "MyAmmo.h"
#include "Components/WidgetComponent.h"
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AMyAmmo::AMyAmmo()
{
	PrimaryActorTick.bCanEverTick = true;

	rootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshRoot"));
	RootComponent = rootMesh;
	boxInteract = CreateDefaultSubobject<UBoxComponent>(TEXT("boxInteract"));
	boxInteract->SetupAttachment(GetRootComponent());
}

void AMyAmmo::BeginPlay()
{
	Super::BeginPlay();
	boxInteract->OnComponentBeginOverlap.AddDynamic(this, &AMyAmmo::OnBoxBeginOverlap);
	boxInteract->OnComponentEndOverlap.AddDynamic(this, &AMyAmmo::OnBoxEndOverlap);

	// Disable Collision
	if (pickedType == EPickedType::ES_Accessoire) {
		boxInteract->SetGenerateOverlapEvents(false);
		SetCanMove(false);
	}
}

void AMyAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyAmmo::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString tablePath{ TEXT("/Script/Engine.DataTable'/Game/MyBPs/AmmoDT.AmmoDT'") };
	UDataTable* table = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *tablePath));
	if (table)
	{
		FAmmoDataTable* ammoTableRow = nullptr;
		switch (ammoType) {
		case EAmmoType::ES_AmmoPistol: ammoName = "ammoPistol"; break;
		case EAmmoType::ES_AmmoSMG11: ammoName = "ammoSMG11"; break;
		case EAmmoType::ES_AmmoAR4: ammoName = "ammoAR4"; break;
		case EAmmoType::ES_AmmoSniper: ammoName = "ammoSniper"; break;
		case EAmmoType::ES_AmmoMinigun: ammoName = "ammoMinigun"; break;
		case EAmmoType::ES_AmmoGrenade: ammoName = "ammoGrenade"; break;
		case EAmmoType::ES_AmmoBomb: ammoName = "ammoBomb"; break;
		case EAmmoType::ES_AmmoHealth: ammoName = "ammoHealth"; break;
		case EAmmoType::ES_AmmoAR4Scope: ammoName = "ammoAR4Scope"; break;
		case EAmmoType::ES_AmmoAR4Sight: ammoName = "ammoAR4Sight"; break;
		case EAmmoType::ES_AmmoAR4Suppressor: ammoName = "ammoAR4Suppressor"; break;
		case EAmmoType::ES_AmmoLaserPistol: ammoName = "ammoLaserPistol"; break;
		case EAmmoType::ES_AmmoCardCanion: ammoName = "ammoCardCanion"; break;
		case EAmmoType::ES_AmmoCardScifi: ammoName = "ammoCardScifi"; break;
		case EAmmoType::ES_AmmoCardSnow: ammoName = "ammoCardSnow"; break;
		}
		ammoTableRow = table->FindRow<FAmmoDataTable>(FName(ammoName), TEXT(""));
		if (ammoTableRow)
		{
			// Set From Table 
			rootMesh->SetStaticMesh(ammoTableRow->rootMesh);
			pickedType = ammoTableRow->pickedType;
			ammoCount = ammoTableRow->ammoCount;
			ammoIconTexture = ammoTableRow->ammoIconTexture;
			ammoSoundEquip = ammoTableRow->ammoSoundEquip;
			ammoEffects = ammoTableRow->ammoEffects;
		}
	}
}

void AMyAmmo::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
														  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		p_myCharacter->SetOverlappingAmmo(this);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.7f, FColor::FromHex("#FFFF00FF"), FString("ENTER AMMO") + this->ammoName, false); }
	}
}

void AMyAmmo::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		p_myCharacter->SetOverlappingAmmo(nullptr);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.7f, FColor::FromHex("#FFFF00FF"), FString("LEAVE AMMO") + this->ammoName, false); }
	}
}