// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AnimationDock.generated.h"

UCLASS(ABSTRACT, Blueprintable)
class FORGE_API AAnimationDock : public AActor
{
	GENERATED_BODY()	

public:	

	/* ↓나중에 Path로 바꾸는것 고려 */

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = "Basic Attack")
	UAnimMontage* Attack01;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = "Basic Attack")
	UAnimMontage* Attack02;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, category = "Basic Attack")
	UAnimMontage* Attack03;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Sets default values for this actor's properties
	AAnimationDock();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void TestFunc();
};
