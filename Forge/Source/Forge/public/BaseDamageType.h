// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "BaseDamageType.generated.h"

/**
 * 
 */
UCLASS()
class FORGE_API UBaseDamageType : public UDamageType
{
	GENERATED_BODY()
	
	
	UBaseDamageType(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	bool bCanDieFrom;

	UPROPERTY(EditDefaultsOnly)
	float CritDmgModifier;

	UPROPERTY(EditDefaultsOnly)
	float NormalDmgModifier;

public:
	bool GetCanDieFrom();
	
	float GetCritDmgModifier();

	float GetNormalDmgModifier();
};
