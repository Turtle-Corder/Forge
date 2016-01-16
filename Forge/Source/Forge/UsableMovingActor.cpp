// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableMovingActor.h"
#include "ForgeCharacter.h"



AUsableMovingActor::AUsableMovingActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Super Class Character -> Base Root Component is Capsule <> Actor Root Component is Scene
	MeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	MeshComp->AttachParent = RootComponent;

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collision"));
	CollisionComp->AttachParent = RootComponent;
	CollisionComp->SetSphereRadius(110.0f);
	CollisionComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AUsableMovingActor::OnOverlapBegin);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AUsableMovingActor::OnOverlapEnd);
}

void AUsableMovingActor::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AForgeCharacter* MyPawn = Cast<AForgeCharacter>(OtherActor);

	if (MyPawn)
	{
		if (UnderMarkerFX)
		{
			UnderMarkerPSC = UGameplayStatics::SpawnEmitterAtLocation(this, UnderMarkerFX, GetActorLocation());
			UnderMarkerPSC->ActivateSystem(true);

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Spawn Activate"));

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, CollisionComp->GetComponentLocation().ToString());
		}
	}
}

void AUsableMovingActor::OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AForgeCharacter* MyPawn = Cast<AForgeCharacter>(OtherActor);

	if (MyPawn)
	{
		if (UnderMarkerPSC)
		{
			UnderMarkerPSC->DeactivateSystem();

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Spawn Deactivate"));
		}
	}
}