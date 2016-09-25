// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "BaseDamageType.h"



UBaseDamageType::UBaseDamageType(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CritDmgModifier = 2.5f;
	NormalDmgModifier = 1.0f;
}

bool UBaseDamageType::GetCanDieFrom() 
{
	return bCanDieFrom;
}

float UBaseDamageType::GetCritDmgModifier()
{
	return CritDmgModifier;
}

float UBaseDamageType::GetNormalDmgModifier()
{
	return NormalDmgModifier;
}