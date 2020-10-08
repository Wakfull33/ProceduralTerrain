// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DisplayMesh.generated.h"

class UMapGeneratorComponent;
class UProceduralMeshComponent;

UCLASS()
class PROCEDURALTERRAIN_API ADisplayMesh : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADisplayMesh();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UMapGeneratorComponent* MapGeneratorComp = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UProceduralMeshComponent* CustomMesh = nullptr;

	TArray<float> NoiseMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostActorCreated() override;

	virtual void PostInitializeComponents() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddTriangle(const int BottomLeft, const int TopLeft, const int BottomRight, int32& TriIndex, TArray<int32> &Triangles);

	void GenerateNoiseMapData();

	void GenerateTerrainMesh();

	void GenerateMeshTexture();

	void GenerateNormals(int Width, int Height, int TopLeftX, int TopLeftY, TArray<FVector> &Normals);
	
	void AddNormal(const int Vertice1, const int Vertice2, const int Vertice3, const float TopLeftX, const float TopLeftY, TArray<FVector> &Normals);

	FLinearColor GetNoiseColor(float noiseValue);

	FIntPoint GetIndexValue(const int Vertice);
};
