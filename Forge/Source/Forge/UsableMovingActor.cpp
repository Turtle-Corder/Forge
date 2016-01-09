// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableMovingActor.h"



AUsableMovingActor::AUsableMovingActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComp = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));

}
