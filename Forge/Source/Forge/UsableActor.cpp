// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableActor.h"


// Sets default values
AUsableActor::AUsableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AUsableActor::OnBeginFocus()
{
	MeshComp->SetRenderCustomDepth(true);
}

void AUsableActor::OnEndFocus()
{
	MeshComp->SetRenderCustomDepth(false);
}

void AUsableActor::OnUsed(APawn* InstigatorPawn)
{
	//Nothing to do here..
}