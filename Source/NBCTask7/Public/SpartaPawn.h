// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpartaPawn.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
struct FInputActionValue;


UCLASS()
class NBCTASK7_API ASpartaPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpartaPawn();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerMovement")
	UFloatingPawnMovement* FloatingPawnMovement;


	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Flying(const FInputActionValue& value);
	UFUNCTION()
	void FlyEnd(const FInputActionValue& value);
	UFUNCTION()
	void Grounded(float DeltaTime);

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
private:
	float NormalSpeed;
	float MaxFallSpeed;
	float FallSpeed;
	float GravityValue;
	float FlyInput;
	float FlySpeed;

	bool bIsFly;
};
