// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "UsableMovingActor.generated.h"

/**
 * 
 */
UCLASS()
class FORGE_API AUsableMovingActor : public AUsableActor
{
	GENERATED_BODY()

	UPROPERTY()
	UParticleSystemComponent* UnderMarkerPSC;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Marker")
	UParticleSystem* UnderMarkerFX;

public:
	AUsableMovingActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
