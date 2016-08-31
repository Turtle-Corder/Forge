// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableStaticActor.h"


AUsableStaticActor::AUsableStaticActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComp = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	MeshComp->SetupAttachment(GetRootComponent());

	MeshComp->SetSimulatePhysics(true);
}