#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MyEStates.h"
#include "MyActor.h"
#include "Components/BoxComponent.h"
#include "MyAmmo.generated.h"

class AMyCharacter;
class USoundCue;
class UParticleSystem;
class UWidgetComponent;

// Ammo Data Table
USTRUCT(BlueprintType)
struct FAmmoDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickedType pickedType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* rootMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ammoCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ammoIconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* ammoSoundEquip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ammoEffects;
};

// Ammo Class
UCLASS()
class WRAITH_PATH_API AMyAmmo : public AMyActor
{
	GENERATED_BODY()
	
public:	
	AMyAmmo();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable) FORCEINLINE int32 GetAmmoCount() const { return ammoCount; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetAmmoCount(int32 newCount) { ammoCount = newCount; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EAmmoType GetAmmoType() const { return ammoType; }
	UFUNCTION(BlueprintCallable) FORCEINLINE EPickedType GetPickedType() const { return pickedType; }
	FORCEINLINE FString GetAmmoName() const { return ammoName; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void ActivateAmmoBoxInteract() { boxInteract->SetGenerateOverlapEvents(true); }
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
													  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Base" , meta = (AllowPrivateAccess = "true"))
	EAmmoType ammoType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Base", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ammoWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Base", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* boxInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Base", meta = (AllowPrivateAccess = "true"))
	FString ammoName;

	// DATA TABLE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	EPickedType pickedType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* rootMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	int32 ammoCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ammoIconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	USoundCue* ammoSoundEquip;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ammoEffects;
};
