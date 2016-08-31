// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Forge.h"
#include "ForgeCharacter.h"
#include "EngineUtils.h"
#include "UsableActor.h"



AForgeCharacter::AForgeCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	GetCharacterMovement()->JumpZVelocity = 620;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionRange = 500.0f;
}


void AForgeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AForgeCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AForgeCharacter::MoveRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AForgeCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AForgeCharacter::OnStopJump);

	InputComponent->BindAction("Interaction", IE_Pressed, this, &AForgeCharacter::Interaction);
}


void AForgeCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.0f)
	{
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = /*bLimitRotation ? GetActorRotation() :*/ Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}


void AForgeCharacter::MoveRight(float Val)
{
	if (Controller && Val != 0.0f)
	{
		const FRotator Rotation = /*GetActorRotation()*/ Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		AddMovementInput(Direction, Val);
	}
}


void AForgeCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == EMovementMode::MOVE_Falling && GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		SetIsJumping(false);
	}
}


void AForgeCharacter::OnStartJump()
{
	bPressedJump = true;

	SetIsJumping(true);
}


void AForgeCharacter::OnStopJump()
{
	bPressedJump = false;
}


bool AForgeCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}


void AForgeCharacter::SetIsJumping(bool NewJumping)
{
	if (bIsCrouched && NewJumping)
	{
		UnCrouch();
	}

	else
	{
		bIsJumping = NewJumping;
	}

	if (Role < ROLE_Authority)
	{
		ServerSetIsJumping(NewJumping);
	}
}


void AForgeCharacter::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}


bool AForgeCharacter::ServerSetIsJumping_Validate(bool NewJumping)
{
	return true;
}


void AForgeCharacter::Interaction()
{
	/*
	float targetLength = 999.0f;
	FVector targetVector;

	bool bNewInteractiveActor = false;
	
	for (TActorIterator<AUsableActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		float tmpLength = FVector::Dist(ActorItr->GetActorLocation(), GetActorLocation());

		if (tmpLength < InteractionRange)
		{
			if (tmpLength < targetLength)
			{
				targetLength = tmpLength;
				targetVector = ActorItr->GetActorLocation();
//				targetVector = ActorItr->GetActorLocation() + ActorItr->GetMeshComponent()->GetForwardVector() * 20.0f;

				bNewInteractiveActor = true;
			}
		}
	}
	

	GetWorld()->Trace

	if (bNewInteractiveActor)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		if (NavSys)
		{
			NavSys->SimpleMoveToLocation(Controller, targetVector);
		}
	}
	*/
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = TraceStart + (Direction * 50.0f);

	TArray<FHitResult> HitOuts = TraceSphere(TraceStart, TraceEnd, InteractionRange, this, ECollisionChannel::ECC_WorldDynamic);
	
	AUsableActor* NearActor = nullptr;
	float NearLength = 999.0f;
	bool bNewInteractiveActor = false;

	for (int32 index = 0; index != HitOuts.Num(); ++index)
	{
		AUsableActor* IActor = Cast<AUsableActor>(HitOuts[index].GetActor());

		float Delta = FVector::Dist(TraceStart, IActor->GetActorLocation());

		if (NearLength > Delta)
		{
			NearActor = IActor;
			NearLength = Delta;

			bNewInteractiveActor = true;
		}
	}

	if (NearActor)
	{
		if (bNewInteractiveActor)
		{
			UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
			if (NavSys)
			{
				NavSys->SimpleMoveToLocation(Controller, NearActor->GetActorLocation());
			}
		}

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, NearActor->GetActorLocation().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, bNewInteractiveActor ? TEXT("true") : TEXT("false"));
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("NearActor Null!"));
	}
}


TArray<FHitResult> AForgeCharacter::TraceSphere(const FVector& TraceFrom, const FVector& TraceTo, const float Radius, AActor* ActorToIgnore, ECollisionChannel TraceChannel)
{
	// FName InTraceTag, bool bInTraceComplex, const AActor * InIgnoreActor
	FCollisionQueryParams TraceParams(FName(TEXT("Instant Sphere Trace")), true, ActorToIgnore);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(ActorToIgnore);

	TArray<FHitResult> HitOut;

	GetWorld()->SweepMultiByObjectType(HitOut, TraceFrom, TraceTo, FQuat(), TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);

	return HitOut;
}


void AForgeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AForgeCharacter, bIsJumping, COND_SkipOwner);
}