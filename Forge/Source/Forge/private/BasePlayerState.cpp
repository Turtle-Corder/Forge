// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "BasePlayerState.h"


ABasePlayerState::ABasePlayerState(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TeamNumber = 0;
}

void ABasePlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}

int32 ABasePlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, TeamNumber);
}