// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "UsableMovingActor.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT, Blueprintable)
class FORGE_API AUsableMovingActor : public AUsableActor
{
	GENERATED_BODY()

	UPROPERTY()
	UParticleSystemComponent* UnderMarkerPSC;

	UPROPERTY(EditDefaultsOnly, Category = "Marker")
	UParticleSystem* UnderMarkerFX;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Marker")
	FLinearColor MarkerColor;

public:
	AUsableMovingActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Marker")
	void ActivateMarker();

	UFUNCTION(BlueprintCallable, Category = "Marker")
	void DeActivateMarker();
};
