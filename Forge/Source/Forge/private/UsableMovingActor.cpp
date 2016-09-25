// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableMovingActor.h"
#include "MeleeCharacter.h"



AUsableMovingActor::AUsableMovingActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Super Class Character -> Base Root Component is Capsule <> Actor Root Component is Scene
	MeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	MeshComp->SetupAttachment(GetRootComponent());

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision"));
	CollisionComp->SetupAttachment(GetRootComponent());
	CollisionComp->SetSphereRadius(110.0f);
	CollisionComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AUsableMovingActor::OnOverlapBegin);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AUsableMovingActor::OnOverlapEnd);
	
	MarkerColor = FLinearColor::White;
}


void AUsableMovingActor::OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMeleeCharacter* MyPawn = Cast<AMeleeCharacter>(OtherActor);

	if (MyPawn)
	{
		ActivateMarker();
	}
}

void AUsableMovingActor::OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMeleeCharacter* MyPawn = Cast<AMeleeCharacter>(OtherActor);

	if (MyPawn)
	{
		DeActivateMarker();
	}
}


void AUsableMovingActor::ActivateMarker()
{
	if (UnderMarkerFX)
	{
		UnderMarkerPSC = UGameplayStatics::SpawnEmitterAtLocation(this, UnderMarkerFX, GetActorLocation());
		UnderMarkerPSC->ActivateSystem(true);

		FName ColorParam = TEXT("SetupColor");
		UnderMarkerPSC->SetVectorParameter(ColorParam, FVector(MarkerColor));

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Spawn Activate"));

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, CollisionComp->GetComponentLocation().ToString());
	}
}

void AUsableMovingActor::DeActivateMarker()
{
	if (UnderMarkerPSC)
	{
		UnderMarkerPSC->DeactivateSystem();

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Spawn Deactivate"));
	}
}