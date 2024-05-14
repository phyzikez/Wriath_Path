#include "MyWeapon.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

AMyWeapon::AMyWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	weaponName = "";
	rootSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshRoot"));
	RootComponent = rootSkeletalMesh;
	boxInteract = CreateDefaultSubobject<UBoxComponent>(TEXT("boxInteract"));
	boxInteract->SetupAttachment(GetRootComponent());
	boxInteract->SetGenerateOverlapEvents(false);

	// Skeleton Params
	deltaClipXPislolMax = 3.5f;
	deltaClipXSniperMax = 10.f;
	deltaSightSniperDownLoc = FVector(0.82f, 0.f, -2.1f);
	deltaSightSniperUpperLoc = FVector(0.f, 0.f, 0.f);
	deltaSightLensSniperDownLoc = FVector(-3.2f, 0.f, 0.f);
	deltaSightLensSniperUpperLoc = FVector(-5.8f, 0.f, 0.f);
	sightMoveDT = 0.2f;
	sightLensMoveDT = 0.2f;
	deltaRotXRollerMinigunMax = 1.f;
}

void AMyWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString tablePath{ TEXT("/Script/Engine.DataTable'/Game/MyBPs/WeaponDT.WeaponDT'")};
	UDataTable* table = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *tablePath));
	if (table)
	{
		//// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("T A B L E"), false); }
		//else { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FF000FF"), FString(" N O T A B L E"), false); }
		FWeaponDataTable* weaponTableRow = nullptr;
		weaponTableRow = table->FindRow<FWeaponDataTable>(FName(weaponName), TEXT(""));
		if (weaponTableRow)
		{
			//// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("R O W"), false); }
			//else { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FF000FF"), FString(" N O R O W"), false); }
			weaponName = weaponTableRow->weaponName;
			weaponSkeletalMesh = weaponTableRow->weaponSkeletalMesh;

			rootSkeletalMesh->SetSkeletalMesh(weaponSkeletalMesh);

			weaponStaticMesh = weaponTableRow->weaponStaticMesh;
			fireFrequency = weaponTableRow->fireFrequency;
			speedWalk = weaponTableRow->speedWalk;
			cameraFOVZoom = weaponTableRow->cameraFOVZoom;

			ammoType = weaponTableRow->ammoType;
			ammoCurrentMagazine = weaponTableRow->ammoCurrentMagazine;
			ammoCurrentWeapon = weaponTableRow->ammoCurrentWeapon;
			ammoCapacityMagazine = weaponTableRow->ammoCapacityMagazine;
			ammoCapacityWeapon = weaponTableRow->ammoCapacityWeapon;
			 
			damageValue = weaponTableRow->damageValue;

			fireModeBase = weaponTableRow->fireModeBase;
			fireModeAlt = weaponTableRow->fireModeAlt;
			currentFireMode = fireModeBase;

			soundEquip = weaponTableRow->soundEquip;
			soundFire = weaponTableRow->soundFire;
			soundFireAlt = weaponTableRow->soundFireAlt;
			soundReload = weaponTableRow->soundReload;

			weaponIcon = weaponTableRow->weaponIcon;
			weaponCrosshair = weaponTableRow->weaponCrosshair;

			materialInstance = weaponTableRow->materialInstance;
			materialIndex = weaponTableRow->materialIndex;

			effectFireMuzzle = weaponTableRow->effectFireMuzzle;
			effectFireTarget = weaponTableRow->effectFireTarget;
			effectFireTrace = weaponTableRow->effectFireTrace;

			if (weaponName == "pistol") {
				this->rootSkeletalMesh->HideBoneByName(FName("b_gun_laser"), EPhysBodyOp::PBO_None);
			}
		}
	}
}

void AMyWeapon::OnSightMoveUpEnd()
{
}

void AMyWeapon::OnSightMoveDownEnd()
{
}

void AMyWeapon::OnSightLensEnd()
{
}

void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	boxInteract->OnComponentBeginOverlap.AddDynamic(this, &AMyWeapon::OnBoxBeginOverlap);
	boxInteract->OnComponentEndOverlap.AddDynamic(this, &AMyWeapon::OnBoxEndOverlap);
	SetCanMove(false);

	if (weaponName == "pistol") {
		boxInteract->SetGenerateOverlapEvents(true);
	}
}

void AMyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		if (this->weaponName != "pistol") { p_myCharacter->SetOverlappingWeapon(this); }
		if (p_myCharacter->isFirstEquip && this->weaponName == "pistol") {
			p_myCharacter->EquipPistol(this);
			Equip(p_myCharacter->GetMesh(), "pistolArmSocket");
			p_myCharacter->isFirstEquip = false;
		}
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#FFFF00FF"), FString("Enter to PICK WEAPON"), false); }
	}
}

void AMyWeapon::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		p_myCharacter->SetOverlappingWeapon(nullptr);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("Leave PICK WEAPON"), false); }
	}
}

void AMyWeapon::Equip(USceneComponent* parentBind, FString socketNameBind) {
									   FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	if (rootSkeletalMesh) {
		this->SetCanMove(false);
		rootSkeletalMesh->AttachToComponent(parentBind, rules, FName(socketNameBind));
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("PICKED : ") + socketNameBind, false); }
		if (weaponName == "sniper") {
			this->SetDeltaSightSniper(deltaSightSniperDownLoc);
			this->SetDeltaSightLensSniper(deltaSightLensSniperDownLoc);
			// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Silver, FString("CORRECT SIGHT POS"), false); }
		}
	}
	else {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("BAD SKELETAL MESH : "), false); }
	}
}

void AMyWeapon::Drop(USceneComponent* parentBind, FString socketNameBind) {
	FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
	FDetachmentTransformRules rulesD(rulesA, true);
	if (rootSkeletalMesh) {
		rootSkeletalMesh->DetachFromComponent(rulesD);
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString("DROPPED : ") + socketNameBind, false); }
	}
	else {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString("BAD SKELETAL MESH : "), false); }
	}
}

void AMyWeapon::SetWeaponVisibility(bool isVisible) {
	//TArray<USceneComponent*> arr;
	if (this->GetRootComponent()) {
		//this->GetRootComponent()->GetChildrenComponents(true, arr);
		//for (auto* comp : arr) { if (comp) comp->SetVisibility(isVisible); }
		this->GetRootComponent()->SetVisibility(isVisible, true);
	}
}

void AMyWeapon::SetWeaponPickupPhysics()
{
	if (this->rootSkeletalMesh) {
		rootSkeletalMesh->SetSimulatePhysics(false);
		rootSkeletalMesh->SetEnableGravity(false);
		rootSkeletalMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
		rootSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		boxInteract->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		boxInteract->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}