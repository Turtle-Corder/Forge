// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UsableActor.generated.h"

UCLASS(ABSTRACT)
class FORGE_API AUsableActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category = "Root Scene")
	USceneComponent* RootSceneComp;

	UPROPERTY(VisibleAnywhere, category = "Mesh")
	UMeshComponent* MeshComp;

	UPROPERTY()
	APostProcessVolume* PPvolume;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = "PostProcess")
	UMaterial* PPMat;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = "PostProcess")
	FName PPColorParam;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, category = "PostProcess")
	FLinearColor OutlineColor;
	
	UPROPERTY(BlueprintReadOnly, category = "PostProcess")
	UMaterialInstanceDynamic* PPvInst;

	virtual void BeginPlay() override;

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
