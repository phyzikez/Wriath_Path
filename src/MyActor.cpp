#include "MyActor.h"

AMyActor::AMyActor() : canMove{ true }, runTime { 0.f }, ampl{ 0.3f }, phase{ 2.f }, rotRate{ 8.f }
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (canMove) {
		runTime += DeltaTime;
		AddActorWorldOffset(FVector(0.f, 0.f, ampl * FMath::Sin(runTime * phase)));
		AddActorWorldRotation(FRotator(0.f, rotRate * DeltaTime, 0.f));
	}
}