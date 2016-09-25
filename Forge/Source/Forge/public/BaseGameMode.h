// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FORGE_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bAllowFriendlyAtkDamage;
	
public:
	ABaseGameMode(const FObjectInitializer& ObjectInitializer);

	virtual bool CanDealDamage(class ABasePlayerState* DamageCauser, class ABasePlayerState* DamagePlayer) const;

	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;
};
