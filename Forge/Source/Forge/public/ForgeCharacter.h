// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "ForgeCharacter.generated.h"

UCLASS(Blueprintable)
class AForgeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	AForgeCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Key Input */

	virtual void MoveForward(float Val);
	virtual void MoveRight(float Val);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void OnStartJump();
	void OnStopJump();

	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Interaction */

	float InteractionRange;

	void Interaction();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Instant Tracing */
	TArray<FHitResult> TraceSphere(const FVector& TraceFrom, const FVector& TraceTo, const float Radius, AActor* ActorToIgnore, ECollisionChannel TraceChannel = ECC_WorldDynamic);
};

