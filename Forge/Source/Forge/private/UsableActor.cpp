// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "UsableActor.h"


// Sets default values
AUsableActor::AUsableActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSceneComp = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = RootSceneComp;
}

void AUsableActor::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<IInterface_PostProcessVolume*> PPvList = GetWorld()->PostProcessVolumes;

	for (IInterface_PostProcessVolume* Iter : PPvList)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("For loop chk"));

		if (Iter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Iter empty chk"));

			APostProcessVolume* PPvChecker = Cast<APostProcessVolume>(Iter);

			if (PPvChecker->ActorHasTag(FName(TEXT("OutlineProcess"))))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Tag chk"));
				PPvolume = PPvChecker;
				break;
			}
		}
	}

	if (PPvolume && PPMat)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("volume & Mat chk"));
		PPvInst = UMaterialInstanceDynamic::Create(PPMat, this);
		PPvInst->SetVectorParameterValue(PPColorParam, OutlineColor);
	}
}

void AUsableActor::OnBeginFocus()
{	
	PPvolume->Settings.AddBlendable(PPvInst, 1.0f);

	MeshComp->SetRenderCustomDepth(true);
}

void AUsableActor::OnEndFocus()
{
	MeshComp->SetRenderCustomDepth(false);

	PPvolume->Settings.RemoveBlendable(PPvInst);
}

void AUsableActor::OnUsed(APawn* InstigatorPawn)
{
	//Nothing to do here..
}