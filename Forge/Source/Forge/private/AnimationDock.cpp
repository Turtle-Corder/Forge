// Fill out your copyright notice in the Description page of Project Settings.

#include "Forge.h"
#include "AnimationDock.h"


// Sets default values
AAnimationDock::AAnimationDock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnimationDock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnimationDock::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AAnimationDock::TestFunc()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("AnimDock Cast Success!"));
}
