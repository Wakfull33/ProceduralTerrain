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

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<int32> Triangles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FColor> Colors;

	TArray<float> NoiseMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostActorCreated() override;

	virtual void PostInitializeComponents() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddTriangle(const int BottomLeft, const int TopLeft, const int BottomRight, int32& TriIndex);

	void GenerateNoiseMapData();

	void GenerateTerrainMesh();

	void GenerateMeshTexture();

	void AddNormal(const int Vertice1, const int Vertice2, const int Vertice3, const float TopLeftX, const float TopLeftY);

	FLinearColor GetNoiseColor(float noiseValue);

	FIntPoint GetIndexValue(const int Vertice);
};
