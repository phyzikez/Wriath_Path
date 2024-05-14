// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class WRAITH_PATH_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMyPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveLeft(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	UPROPERTY(VisibleAnywhere);
	UCapsuleComponent* myCapsule;

	UPROPERTY(VisibleAnywhere);
	USkeletalMeshComponent* myMesh;

	UPROPERTY(VisibleAnywhere);
	USpringArmComponent* myCameraRoot;

	UPROPERTY(VisibleAnywhere);
	UCameraComponent* myCamera;
};
