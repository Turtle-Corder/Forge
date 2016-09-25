// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "BaseCharacter.h"
#include "AnimationDock.h"
#include "BaseGameMode.h"
#include "BaseDamageType.h"


// Sets default values
ABaseCharacter::ABaseCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Health = 100.0f;
	Attacking = false;
	RestoreAmount = 1.0f;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AnimConnector && !AnimDock)
	{
		AnimDock = AnimConnector->GetDefaultObject<AAnimationDock>();

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, AnimConnector->GetDefaultObject<AAnimationDock>()->Attack01->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, AnimDock->Attack01->GetName());
	}
}

bool ABaseCharacter::IsAlive() const
{
	return Health > 0;
}

bool ABaseCharacter::IsAttacking() const
{
	return Attacking;
}

float ABaseCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ABaseCharacter>()->Health;
}

float ABaseCharacter::GetHealth() const
{
	return Health;
}

TArray<FHitResult> ABaseCharacter::TraceSphere(const FVector& TraceFrom, const FVector& TraceTo, const float Radius, AActor* ActorToIgnore, ECollisionChannel TraceChannel)
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

void ABaseCharacter::StopAllAnimMontage()
{
	USkeletalMeshComponent* Mesh3P = GetMesh();
	if (Mesh3P && Mesh3P->AnimScriptInstance)
	{
		Mesh3P->AnimScriptInstance->Montage_Stop(0.0f); //_Pause, _Resume, _Play...
	}
}

void ABaseCharacter::SetCurrentAnim(UAnimMontage* NewAnim)
{
	// More Some Action..?
	CurrentAnim = NewAnim;
}

// Take Damage -> Die -> CanDie.. OnDeath
//             -> PlayHit -> ReplicateHit ->OnRep
float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		return 0.0f;
	}

	//If Custom GameType?
	ABaseGameMode* MyGameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	Damage = MyGameMode ? MyGameMode->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : Damage;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			bool bCanDie = true;

			if (DamageEvent.DamageTypeClass)
			{
				//Damage Event Possible Killing (this)?
				UBaseDamageType* DmgType = Cast<UBaseDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
				bCanDie = (DmgType == nullptr || (DmgType && DmgType->GetCanDieFrom()));
			}

			if (bCanDie)
			{
				//Gonna Diying
				Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				Health = 1.0f;
			}
		}

		else
		{
			// Replicating & Other Action
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(ActualDamage, DamageEvent, Pawn, DamageCauser, false);
		}
	}

	return ActualDamage;
}
bool ABaseCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	// More Some Condition
	if (Role != ROLE_Authority ||
		GetWorld()->GetAuthGameMode() == NULL ||
		GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)
	{
		return false;
	}

	return true;
}

bool ABaseCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);

	return true;
}

void ABaseCharacter::OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	//Stop Replicates
	bReplicateMovement = false;
	bTearOff = true;

	PlayHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

	DetachFromControllerPendingDestroy();

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	USkeletalMeshComponent* Mesh3P = GetMesh();
	if (Mesh3P)
	{
		Mesh3P->SetCollisionProfileName(TEXT("Ragdoll"));
	}
	SetActorEnableCollision(true);

	SetRagdollPhysics();

	//Falling Way?
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent PointDmg = *((FPointDamageEvent*)(&DamageEvent));
		{
			Mesh3P->AddImpulseAtLocation(PointDmg.ShotDirection * 12000, PointDmg.HitInfo.ImpactPoint, PointDmg.HitInfo.BoneName);
		}
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent RadialDmg = *((FRadialDamageEvent*)(&DamageEvent));
		{
			Mesh3P->AddRadialImpulse(RadialDmg.Origin, RadialDmg.Params.GetMaxRadius(), 100000 /*RadialDmg.DamageTypeClass->DamageImpulse*/, ERadialImpulseFalloff::RIF_Linear);
		}
	}
}

void ABaseCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, bKilled);
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		if (bKilled && SoundDeath)
		{
			UGameplayStatics::SpawnSoundAttached(SoundDeath, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
		}
		else if (SoundTakeHit)
		{
			UGameplayStatics::SpawnSoundAttached(SoundTakeHit, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
		}
	}
}

void ABaseCharacter::ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if (PawnInstigator == LastTakeHitInfo.PawnInstigator.Get() && LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass)
	{
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			return;
		}

		DamageTaken += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = DamageTaken;
	LastTakeHitInfo.PawnInstigator = Cast<ABaseCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();
}

void ABaseCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}

	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get(), LastTakeHitInfo.bKilled);
	}
}

void ABaseCharacter::SetRagdollPhysics()
{
	// Ragdoll <- Physics Material Check
	// Ragdoll yes -> Physics Simul + movement set + lifespan
	//         not -> Physics Skip  + movement set + lifespan

	bool bInRagdoll = false;
	USkeletalMeshComponent* Mesh3P = GetMesh();

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}

	else if (!Mesh3P || !Mesh3P->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}

	else
	{
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetSimulatePhysics(true);
		Mesh3P->WakeAllRigidBodies();
		Mesh3P->bBlendPhysics = true;

		bInRagdoll = true;
	}

	UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
		MoveComp->SetComponentTickEnabled(false);
	}

	if (!bInRagdoll)
	{
		// https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/GameFramework/ACharacter/TurnOff/index.html
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}

	else
	{
		SetLifeSpan(10.0f);
	}
}

void ABaseCharacter::Launcher()
{
	const FVector LaunchVector = GetActorForwardVector() * LaunchAmount;
	const FVector JumpVector = GetActorUpVector() * JumpAmount;
	const FVector OutVector = LaunchVector + JumpVector;

//	GetCharacterMovement()->GroundFriction = 0.0f;

	LaunchCharacter(OutVector, false, false);
}

void ABaseCharacter::RegenOn()
{
}

void ABaseCharacter::RegenOff()
{
}

void ABaseCharacter::SetDamages(float NewMin, float NewMax)
{
	MinDamage = NewMin;
	MaxDamage = NewMax;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, LastTakeHitInfo);
}