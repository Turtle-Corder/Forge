// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FORGE_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(Transient, Replicated)
	int32 TeamNumber;
	
public:
	ABasePlayerState(const FObjectInitializer& ObjectInitializer);

	void SetTeamNumber(int32 NewTeamNumber);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetTeamNumber() const;
};
