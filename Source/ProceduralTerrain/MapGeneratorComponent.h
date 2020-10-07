// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapGeneratorComponent.generated.h"

class ADisplayMesh;

UENUM(Blueprintable)
enum DrawMode
{
	DM_Mesh	UMETA(DisplayName = "Mesh"),
	DM_Color UMETA(DisplayName = "Color"),
	DM_Noise UMETA(DisplayName = "Noise"),
};

USTRUCT(BlueprintType)
struct FTerrainType
{
	GENERATED_BODY();

public:
	FString typeName;
	float height;
	FLinearColor color;

	FTerrainType() {};
	FTerrainType(const FString& typeName, const float& height, const FLinearColor& color) : typeName(typeName), height(height), color(color) {};
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALTERRAIN_API UMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMapGeneratorComponent();

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
		TArray<FTerrainType> regions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Draw)
		TEnumAsByte<DrawMode> drawMode;

	UPROPERTY()
		ADisplayMesh* Owner = nullptr;

	UFUNCTION(BlueprintCallable)
		void GenerateMap();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};