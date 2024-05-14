#include "MyPickedActor.h"
#include "MyCharacter.h"
#include "../../Engine/Classes/Components/BoxComponent.h"
#include <Kismet/KismetSystemLibrary.h>

AMyPickedActor::AMyPickedActor() {
	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	collisionBox->SetupAttachment(GetRootComponent());
	collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	collisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	collisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	startPoint = CreateDefaultSubobject<USceneComponent>(TEXT("start Point"));
	startPoint->SetupAttachment(GetRootComponent());

	endPoint = CreateDefaultSubobject<USceneComponent>(TEXT("end Point"));
	endPoint->SetupAttachment(GetRootComponent());
}

void AMyPickedActor::BeginPlay()
{
	Super::BeginPlay();
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMyPickedActor::OnBoxOverlap);
}

void AMyPickedActor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const FVector start = startPoint->GetComponentLocation();
	const FVector end = endPoint->GetComponentLocation();

	TArray<AActor*> actorsIgnored;
	actorsIgnored.Add(this);

	FHitResult hitResult;
	UKismetSystemLibrary::BoxTraceSingle(this, start, end, FVector(10.f, 10.f, 10.f),
										  collisionBox->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
										  false, actorsIgnored, EDrawDebugTrace::ForDuration,
										  hitResult, true);
}

void AMyPickedActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Always Without KeyPress Event
	/*AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
	if (p_myCharacter) {
		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
		rootMesh->AttachToComponent(p_myCharacter->GetMesh(), rules, FName("pickedSocket"));
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::FromHex("#00FF00FF"), FString("MyActor Overlap Begin. Attached OK"), false); }
	}
	else {
		// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::FromHex("#FF0000FF"), FString("MyPickedActor Overlap Begin. NOT attached -> character is NULL"), false); }
	}*/
}

void AMyPickedActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	//Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::FromHex("#00FF00FF"), FString("MyActor Overlap End."), false); }
}

void AMyPickedActor::Equip(USceneComponent* parentBind, FName socketNameBind) {
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	//rootMesh->AttachToComponent(parentBind, rules, socketNameBind);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::FromHex("#00FF00FF"), FString("Equip Called."), false); }
}

void AMyPickedActor::Drop(USceneComponent* parentBind, FName socketNameBind) {
	FAttachmentTransformRules rulesA(EAttachmentRule::SnapToTarget, true);
	FDetachmentTransformRules rulesD(rulesA, true);
	//rootMesh->DetachFromComponent(rulesD);
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::FromHex("#00FF00FF"), FString("UnEquip Called."), false); }
}