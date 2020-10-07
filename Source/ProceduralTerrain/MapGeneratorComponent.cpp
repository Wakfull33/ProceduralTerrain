// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGeneratorComponent.h"
#include "DisplayMesh.h"

// Sets default values for this component's properties
UMapGeneratorComponent::UMapGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	regions.Add(FTerrainType(TEXT("DeepWater"), 0.3f, FLinearColor(0.1f, 0.45f, 1.0f)));
	regions.Add(FTerrainType(TEXT("WaterShallow"), 0.4f, FLinearColor(0.0f, 0.75f, 1.0f)));
	regions.Add(FTerrainType(TEXT("Sand"), 0.45f, FLinearColor(1.0f, 1.0f, 0.0f)));
	regions.Add(FTerrainType(TEXT("Grass"), 0.55f, FLinearColor(0.25f, 1.0f, 0.1f)));
	regions.Add(FTerrainType(TEXT("Rock"), 0.85f, FLinearColor(0.8f, 0.25f, 0.0f)));
	regions.Add(FTerrainType(TEXT("Snow"), 1.0f, FLinearColor(1.0f, 1.0f, 1.0f)));

	Owner = (ADisplayMesh*)GetOwner();
	if (Owner == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("Cast Failed !!! NULLPTR"));

	// ...
}


// Called when the game starts
void UMapGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (MapWidth < 1)
		MapWidth = 1;
	if (MapHeight < 1)
		MapHeight = 1;
	if (Lacunarity < 1)
		Lacunarity = 1;
	if (Octaves < 0)
		Octaves = 0;
}


// Called every frame
void UMapGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMapGeneratorComponent::GenerateMap()
{
	Owner->GenerateNoiseMapData();
	Owner->GenerateTerrainMesh();
}
