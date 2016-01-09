// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UsableActor.generated.h"

UCLASS()
class FORGE_API AUsableActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, category = "Mesh")
	UMeshComponent* MeshComp;

public:	
	// Sets default values for this actor's properties
	AUsableActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnBeginFocus();

	virtual void OnEndFocus();

	virtual void OnUsed(APawn* InstigatorPawn);

	FORCEINLINE UMeshComponent* GetMeshComponent() const
	{
		return MeshComp;
	}
};
