// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "MeleeCharacter.h"
#include "UsableActor.h"
#include "ForgePlayerController.h"
#include "AnimationDock.h" //*Base 헤더로 고려
#include "Kismet/KismetMathLibrary.h"


AMeleeCharacter::AMeleeCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
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

//	GetCharacterMovement()->JumpZVelocity = 620;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom...
	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomComp"));
	CameraBoomComp->SetupAttachment(RootComponent);
	CameraBoomComp->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoomComp->TargetArmLength = 800.f;
	CameraBoomComp->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoomComp->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

										  // Create a camera...
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CameraBoomComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//	 Add Cursor	 //
	//    Section	 //
	//				 //
	///////////////////

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Health = 150.0f;
	InteractionRange = 500.0f;
	ComboNumber = 1;
}

void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!CamShaker)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("CamShaker is Null"));
		return;
	}

}

void AMeleeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMeleeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMeleeCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMeleeCharacter::MoveRight);

	InputComponent->BindAction("Interaction", IE_Pressed, this, &AMeleeCharacter::Interaction);
	InputComponent->BindAction("Attack/Rotate", IE_Pressed, this, &AMeleeCharacter::LeftMouseAction);
}

void AMeleeCharacter::InputReset()
{
	//Stop
	GetCharacterMovement()->StopMovementImmediately();

	//CombatReset

	// 나중에 합치자..
}

void AMeleeCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.0f)
	{
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = /*bLimitRotation ? GetActorRotation() :*/ Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}

void AMeleeCharacter::MoveRight(float Val)
{
	if (Controller && Val != 0.0f)
	{
		const FRotator Rotation = /*GetActorRotation()*/ Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		AddMovementInput(Direction, Val);
	}
}

void AMeleeCharacter::Interaction()
{
	InputReset();

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

void AMeleeCharacter::LeftMouseAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("L.Click - Rotate"));

	AForgePlayerController* PC = Cast<AForgePlayerController>(Controller);

	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.Location);
	const FRotator FixRot = FRotator(0.0f, Rot.Yaw, 0.0f);

	SetActorRotation(FixRot);

	if (PC)
	{
		if (PC->GetFocusedUsableActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("L.Click - Attack"));
			ComboAttack();
		}
	}
}

void AMeleeCharacter::SetCamShakeAmount(float NewAmount)
{
	CamShakeAmount = NewAmount;
}

void AMeleeCharacter::CameraShaking()
{
	if (CamShaker)
	{
		AForgePlayerController* PC = Cast<AForgePlayerController>(Controller);
		if (PC)
		{
			PC->ClientPlayCameraShake(CamShaker, CamShakeAmount);
		}
	}
}

void AMeleeCharacter::ResetCombat()
{
	Attacking = false;
	ChainAttack = false;
	ComboNumber = 1;

	RegenOn();
}

void AMeleeCharacter::ComboAttack()
{
	if (IsAlive())
	{
		RegenOff();

//		bUseControllerRotationYaw = true;
		if (IsAttacking())
		{
			ChainAttack = true;
		}

		else
		{
			Attacking = true;
			SwordCombo();
		}
	}
}

void AMeleeCharacter::SwordCombo()
{
	// 나중에 쪼개자.. 일단 하드코딩
	switch (ComboNumber)
	{
	case 1:
		SetCurrentAnim(AnimDock->Attack01);
		LaunchAmount = 600.0f;
		SetCamShakeAmount(0.3f);
		SetDamages(15.0f, 18.0f);
		break;

	case 2:
		SetCurrentAnim(AnimDock->Attack02);
		LaunchAmount = 1000.0f;
		SetCamShakeAmount(0.4f);
		SetDamages(15.0f, 18.0f);
		break;

	case 3:
		SetCurrentAnim(AnimDock->Attack03);
		LaunchAmount = 600.0f;
		SetCamShakeAmount(0.7f);
		SetDamages(15.0f, 18.0f);
		break;
	}

	ComboNumber++;

	PlayAnimMontage(CurrentAnim);
}

void AMeleeCharacter::ChainCombo()
{
	if (ChainAttack)
	{
		ChainAttack = false;

		SwordCombo();
	}
}

float AMeleeCharacter::GetLastNoiseLoudness() const
{
	return LastNoiseLoudness;
}

float AMeleeCharacter::GetLastMakeNoiseTime() const
{
	return LastMakeNoiseTime;
}

void AMeleeCharacter::MakePawnNoise(float Loudness)
{
}