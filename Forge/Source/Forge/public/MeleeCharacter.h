// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseCharacter.h"
#include "MeleeCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FORGE_API AMeleeCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	AMeleeCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Input */
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	void InputReset();

	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);

	void Interaction();

	void LeftMouseAction();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Animation */



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Sensing */
private:
	float LastNoiseLoudness;
	float LastMakeNoiseTime;

	UPawnNoiseEmitterComponent* NoiseEmitterComp;

public:
	FORCEINLINE 

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetLastNoiseLoudness() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetLastMakeNoiseTime() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void MakePawnNoise(float Loudness);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Camera */
private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;

	float CamShakeAmount;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<class UCameraShake> CamShaker;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoomComponent() { return CameraBoomComp; }
	
	FORCEINLINE UCameraComponent* GetCameraComponent() { return CameraComp; }

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCamShakeAmount(float NewAmount);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void CameraShaking();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Weapon */

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Combat */
private:
	bool ChainAttack;

	int32 ComboNumber;

	// L.Click -> ComboAttack Branch -> SwordCombo or Not.. ChainCombo & ResetCombo Notify Call
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombat();

	//Can delete macro
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ComboAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SwordCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ChainCombo();
};
