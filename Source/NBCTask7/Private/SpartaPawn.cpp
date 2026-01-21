// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaPawn.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/FloatingPawnMovement.h"

// Sets default values
ASpartaPawn::ASpartaPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComp);
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetBoxExtent(FVector(26.0f, 26.0f, 26.0));
	

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(BoxComp);
	SkeletalMeshComp->SetSimulatePhysics(false);
	SkeletalMeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(BoxComp);
	SpringArmComp->TargetArmLength = 700.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;

	NormalSpeed = 1200.0f;
	FloatingPawnMovement->MaxSpeed = NormalSpeed;
	GravityValue = 980.0f;
	MaxFallSpeed = -5000.0f;
	FallSpeed = 0.0f;
	FlyInput = 0.0f;
	FlySpeed = 600.0f;
	bIsFly = false;
}

void ASpartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Look
				);
			}

			if (PlayerController->FlyAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("Call FlyAction"))
				EnhancedInput->BindAction(
					PlayerController->FlyAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Flying
				);
			}

			if (PlayerController->FlyAction)
			{
				UE_LOG(LogTemp, Warning, TEXT("Call EndFlyAction"))
					EnhancedInput->BindAction(
						PlayerController->FlyAction,
						ETriggerEvent::Completed,
						this,
						&ASpartaPawn::FlyEnd
					);
			}
		}
	}
}

void ASpartaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Grounded(DeltaTime);
	bIsFly = false;
	
	if (!FMath::IsNearlyZero(FlyInput))
	{
		const float UpActor = FlyInput * FlySpeed * DeltaTime;
		
		AddActorLocalOffset(FVector(0.0f, 0.0f, UpActor));
	}
}


//---------------------------필수기능------------------------------//

//void ASpartaPawn::Move(const FInputActionValue& value)
//{
//	if (!Controller) return;
//	const FVector2D MoveInput = value.Get<FVector2D>();
//
//	if (!FMath::IsNearlyZero(MoveInput.X))
//	{
//		AddMovementInput(GetActorForwardVector(), MoveInput.X);
//	}
//
//	if (!FMath::IsNearlyZero(MoveInput.Y))
//	{
//		AddMovementInput(GetActorRightVector(), MoveInput.Y);
//	}
//
//}

//void ASpartaPawn::Look(const FInputActionValue& value)
//{
//	FVector2D LookInput = value.Get<FVector2D>();
//	FRotator NewRotationValue = FRotator(LookInput.Y, LookInput.X, 0.0f);
//	
//	if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
//	{
//		PlayerController->SetControlRotation(GetControlRotation() + NewRotationValue);
//	}
//	
//}


//-------------------------------도전기능---------------------------//

void ASpartaPawn::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	const FVector2D MoveInput = value.Get<FVector2D>();
	
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaPawn::Look(const FInputActionValue& value)
{
	FVector LookInput = value.Get<FVector>();
	FRotator NewRotationValue = FRotator(LookInput.Y, LookInput.X, LookInput.Z);

	if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
	{
		PlayerController->SetControlRotation(GetControlRotation() + NewRotationValue);
	}
}

void ASpartaPawn::Flying(const FInputActionValue& value)
{
	FlyInput = value.Get<float>();
	UE_LOG(LogTemp, Warning, TEXT("Value : %f"), FlyInput);
	bIsFly = true;
	
	
}

void ASpartaPawn::FlyEnd(const FInputActionValue& value)
{
	FlyInput = value.Get<float>();
}

void ASpartaPawn::Grounded(float DeltaTime)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace + (FVector(0.0f, 0.0f, -1.0f) * (GetActorUpVector() + 50.0f));
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Blue, false, 5.0f);
	
	bool HitResult = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartTrace,
		EndTrace,
		ECC_GameTraceChannel2,
		Params);

	if (bIsFly == false)
	{
		if (!HitResult)
		{
			FallSpeed = GravityValue * DeltaTime;
			FallSpeed = FMath::Max(FallSpeed, MaxFallSpeed);
			AddActorWorldOffset(GetGravityDirection() * FallSpeed);
			
		}
		else
		{
			FallSpeed = 0.0f;
			FloatingPawnMovement->MaxSpeed = NormalSpeed * 0.5f;
		}
	}
	else
	{
		FloatingPawnMovement->MaxSpeed = NormalSpeed;
	}
}


