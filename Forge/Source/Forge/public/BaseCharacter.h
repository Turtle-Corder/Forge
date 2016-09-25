// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CustomTypes.h"
#include "BaseCharacter.generated.h"

UCLASS()
class FORGE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const class FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Combat Variables */

private:
	FTimerHandle RestoreTimerHandle;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	bool Attacking;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float RestoreAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MinDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MaxDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float LaunchAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float JumpAmount;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Launcher();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegenOn();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegenOff();

	void SetDamages(float NewMin, float NewMax);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Instant Tracing */

public:
	TArray<FHitResult> TraceSphere(const FVector& TraceFrom, const FVector& TraceTo, const float Radius, AActor* ActorToIgnore, ECollisionChannel TraceChannel = ECC_WorldDynamic);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Animation */

protected:
	UPROPERTY(EditDefaultsOnly, category = "Animation")
	TSubclassOf<class AAnimationDock> AnimConnector;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	class AAnimationDock* AnimDock;

	UPROPERTY(BlueprintReadOnly, category = "Animation")
	UAnimMontage* CurrentAnim;

public:
	void StopAllAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "Custom|Animation")
	void SetCurrentAnim(UAnimMontage* NewAnim);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Sounds */

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* Damage.. See the Ref: https://www.unrealengine.com/ko/blog/damage-in-ue4 */

protected:
	// Take Damage -> Die -> CanDie.. OnDeath
	//             -> PlayHit -> ReplicateHit -> OnRep -> OnDeath(local x)
	//                                                 -> PlayHit(local x)
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	// Boardcast Replicate & Some Action
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	void SetRagdollPhysics();
};
