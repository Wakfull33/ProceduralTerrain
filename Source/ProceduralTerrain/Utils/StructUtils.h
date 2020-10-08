// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALTERRAIN_API UStructUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};


USTRUCT(BlueprintType)
struct FTerrainType
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString typeName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float height;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor color;

	FTerrainType() {};
	FTerrainType(const FString& typeName, const float& height, const FColor& color) : typeName(typeName), height(height), color(color) {};

};

USTRUCT(BlueprintType)
struct FNoiseParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MapWidth = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MapHeight = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MapScale = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MapHeightScale = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float NoiseScale = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Seed = 12.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D Offset = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Octaves = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Persistance = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Lacunarity = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxHeight = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinHeight = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTerrainType> regions;

	FNoiseParameters();
};
