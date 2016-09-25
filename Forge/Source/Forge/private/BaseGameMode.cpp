// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "BaseGameMode.h"
#include "BasePlayerState.h"
#include "BaseCharacter.h"
#include "ForgePlayerController.h"



ABaseGameMode::ABaseGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// use our custom PlayerController class
	PlayerControllerClass = AForgePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter2"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bAllowFriendlyAtkDamage = false;
}

bool ABaseGameMode::CanDealDamage(class ABasePlayerState* DamageCauser, class ABasePlayerState* DamagePlayer) const
{
	// °°Àº ÆÀ °ø°Ý
	if (bAllowFriendlyAtkDamage)
		return true;

	// ¼¿ÇÁ °ø°Ý
	if (DamageCauser == DamagePlayer)
		return true;

	return DamageCauser && DamagePlayer && (DamageCauser->GetTeamNumber() != DamagePlayer->GetTeamNumber());
}

float ABaseGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ABaseCharacter* DamagedPawn = Cast<ABaseCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ABasePlayerState* DamagedPlayerState = Cast<ABasePlayerState>(DamagedPawn->PlayerState);
		ABasePlayerState* InstigatorPalyerState = Cast<ABasePlayerState>(EventInstigator->PlayerState);

		if (!CanDealDamage(InstigatorPalyerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}
	}

	return ActualDamage;
}