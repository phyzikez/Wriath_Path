#pragma once
#include "CoreMinimal.h"
#include "MyActor.h"
#include "MyPickedActor.generated.h"

class UBoxComponent;

UCLASS()
class WRAITH_PATH_API AMyPickedActor : public AActor
{
	GENERATED_BODY()
private:
protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override; 

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	AMyPickedActor();

	void Equip(USceneComponent* ParentBind, FName SocketNameBind);
	void Drop(USceneComponent* ParentBind, FName SocketNameBind);

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* collisionBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* startPoint;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* endPoint;
};