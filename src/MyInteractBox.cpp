#include "MyInteractBox.h"
#include "MyInfoDataTable.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"


AMyInteractBox::AMyInteractBox()
{
	PrimaryActorTick.bCanEverTick = true;

	boxInteract = CreateDefaultSubobject<UBoxComponent>(TEXT("boxInteract"));
	boxInteract->SetupAttachment(GetRootComponent());
	boxInteract->SetGenerateOverlapEvents(true);

	isEnableToUseAgain = true;
}

void AMyInteractBox::BeginPlay()
{
	Super::BeginPlay();
	boxInteract->OnComponentBeginOverlap.AddDynamic(this, &AMyInteractBox::OnPlayerBeginOverlap);
	boxInteract->OnComponentEndOverlap.AddDynamic(this, &AMyInteractBox::OnPlayerEndOverlap);
}

void AMyInteractBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyInteractBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString tablePath{ TEXT("/Script/Engine.DataTable'/Game/MyBPs/HUDInfoDT.HUDInfoDT'") };
	UDataTable* table = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *tablePath));
	if (table)
	{
		FHUDInfoDataTable* eventTableRow = nullptr;
		eventTableRow = table->FindRow<FHUDInfoDataTable>(keyName, TEXT(""));
		if (eventTableRow)
		{
			valueText = eventTableRow->textInfo.ToString();
		}
		else {
			valueText = "ROW ???";
		}
	}
	else {
		valueText = "TABLE ???";
	}
}

void AMyInteractBox::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (isEnableToUseAgain) {
		AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
		if (p_myCharacter) {
			if (keyName.ToString().Contains("action")) {
				p_myCharacter->SetTextActionForHUD(valueText);
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("ACTION ") + keyName.ToString() + " -> " + valueText, false); }
			}
			else if (keyName.ToString().Contains("task")) {
				p_myCharacter->SetTextTaskForHUD(valueText);
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("TASK ") + keyName.ToString() + " -> " + valueText, false); }
				isEnableToUseAgain = false;
			}
			else if (keyName.ToString().Contains("level")) {
				p_myCharacter->SetTextDiscoveredNewLevelForHUD("newLocation", keyName.ToString());
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("LEVEL ") + keyName.ToString() + " -> " + valueText, false); }
				// Autosave Point
				p_myCharacter->SetLastAutosaveLocation(p_myCharacter->GetActorLocation());
				isEnableToUseAgain = false;
				GetWorldTimerManager().SetTimer(timerReset, this, &AMyInteractBox::OnTextShowEnd, 4.f);
			}
		}
	}
}

void AMyInteractBox::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (isEnableToUseAgain) {
		AMyCharacter* p_myCharacter = Cast<AMyCharacter>(OtherActor);
		if (p_myCharacter) {
			if (keyName.ToString().Contains("action")) {
				p_myCharacter->SetTextActionForHUD("");
				// if (GEngine) { GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::FromHex("#00FF00FF"), FString("NO ACTION "), false); }
			}
		}
	}
}

void AMyInteractBox::OnTextShowEnd()
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (p_myCharacter) {
		p_myCharacter->SetTextDiscoveredNewLevelForHUD("", "");
		p_myCharacter->SetIsAutosaveAnimating(true);
		GetWorldTimerManager().SetTimer(timerReset, this, &AMyInteractBox::OnAutosaveEnd, 7.f);
	}
}

void AMyInteractBox::OnAutosaveEnd()
{
	AMyCharacter* p_myCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (p_myCharacter) {
		p_myCharacter->SetIsAutosaveAnimating(false);
	}
}