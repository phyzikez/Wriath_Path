#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MyEStates.h"
#include "MyActor.h"
#include "MyWeaponBox.generated.h"

class UBoxComponent;
class AMyCharacter;
class AMyWeapon;
class USoundCue;
class UParticleSystem;
class UWidgetComponent;

UCLASS()
class WRAITH_PATH_API AMyWeaponBox : public AActor
{
	GENERATED_BODY()	
public:	
	AMyWeaponBox();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable) FORCEINLINE FString GetWeaponCanPickName() { return ammoCanPickName; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool GetIsBoxOpened() { return isOpened; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetBoxOpened() { isOpened = true; }
	UFUNCTION(BlueprintCallable) void OpenBox();
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnOpenboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
													  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOpenboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnImpulseApplied();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Openbox Params", meta = (AllowPrivateAccess = "true"))
	FString ammoCanPickName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Openbox Params", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* openboxBody;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Openbox Params", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* openboxCap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Openbox Params", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* openboxInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Openbox Params", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* openboxWidget;

	FTimerHandle timerOpenBox;
	bool isOpened;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	//USoundCue* openSound;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Table Data ", meta = (AllowPrivateAccess = "true"))
	//UParticleSystem* openEffects;
};