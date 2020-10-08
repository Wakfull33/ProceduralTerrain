// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayMesh.h"
#include "MapGeneratorComponent.h"
#include "ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "ImageUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "NoiseGenerator.h"

// Sets default values
ADisplayMesh::ADisplayMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MapGeneratorComp = CreateDefaultSubobject<UMapGeneratorComponent>(TEXT("MapGeneratorComp"));
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));

	RootComponent = RootComp;
	CustomMesh->SetupAttachment(RootComponent);

	UE_LOG(LogTemp, Warning, TEXT("ADisplayMesh"))


}

// Called when the game starts or when spawned
void ADisplayMesh::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay"))

	GenerateMeshTexture();
	//MapGeneratorComp->GenerateMap();
}

// Called every frame
void ADisplayMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADisplayMesh::GenerateNoiseMapData()
{
	//UE_LOG(LogTemp, Warning, TEXT("GenerateNoiseMapData"))

	NoiseMap = NoiseGenerator::GenerateNoiseMap(
		MapGeneratorComp->MapWidth,
		MapGeneratorComp->MapHeight,
		MapGeneratorComp->Seed,
		MapGeneratorComp->NoiseScale,
		MapGeneratorComp->Octaves,
		MapGeneratorComp->Persistance,
		MapGeneratorComp->Lacunarity,
		MapGeneratorComp->Offset);
}

void ADisplayMesh::GenerateTerrainMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("GenerateTerrainMesh"))

	const int Width = MapGeneratorComp->MapWidth;
	const int Height = MapGeneratorComp->MapHeight;
	const float TopLeftX = (Width - 1) / -2.f;
	const float TopLeftY = (Height - 1) / 2.f;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UVs;
	TArray<FColor> Colors;

	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	Tangents.Reset();
	UVs.Reset();
	Colors.Reset();

	Vertices.Init(FVector(0, 0, 0), Width * Height);
	Triangles.Init(0, (Width - 1) * (Height - 1) * 6);
	Normals.Reserve(Width * Height);
	Tangents.Init(FProcMeshTangent(), (Width - 1) * (Height - 1) * 6);
	UVs.Init(FVector2D(0, 0), Width * Height);
	Colors.Init(FColor(0, 0, 0), Width * Height);

	int32 TriangleIndexCount = 0;
	int32 NormalIndexCount = 0;
	int32 TangentIndexCount = 0;

	FVector DefinedShape[3];
	FProcMeshTangent TangentSetup = FProcMeshTangent(0.0f, 1.0f, 0.0f);

	int vertexIndex = 0;

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			Vertices[x + y * Width] = (FVector((TopLeftX + x) * 10, (TopLeftY - y) * 10, NoiseMap[x + y * Width] * 100));
			UVs[x + y * Width] = (FVector2D(x / (float)Width, y / (float)Height));
			Colors[x + y * Width] = (GetNoiseColor(NoiseMap[x + y * Width]).ToFColor(false));


			if (x < Width - 1 && y < Height - 1)
			{
				AddTriangle(vertexIndex + 1, vertexIndex + Width + 1, vertexIndex + Width, TriangleIndexCount, Triangles);
				AddTriangle(vertexIndex + Width, vertexIndex, vertexIndex + 1, TriangleIndexCount, Triangles);
			}
			vertexIndex++;
		}
	}

	GenerateNormals(Width, Height, TopLeftX, TopLeftY, Normals);

	CustomMesh->ClearAllMeshSections();
	CustomMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void ADisplayMesh::AddTriangle(const int BottomLeft, const int TopLeft, const int BottomRight, int32& TriIndex, TArray<int32> &Triangles)
{

	int32 Point1 = TriIndex++;
	int32 Point2 = TriIndex++;
	int32 Point3 = TriIndex++;

	Triangles[Point1] = BottomLeft;
	Triangles[Point2] = TopLeft;
	Triangles[Point3] = BottomRight;
}

void ADisplayMesh::PostActorCreated()
{
	Super::PostActorCreated();
	//UE_LOG(LogTemp, Warning, TEXT("PostActorCreated"))
}

void ADisplayMesh::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//UE_LOG(LogTemp, Warning, TEXT("PostInitializeComponents"))

	GenerateNoiseMapData();
	GenerateTerrainMesh();
}

FLinearColor ADisplayMesh::GetNoiseColor(const float noiseValue)
{
	FLinearColor color;

	for (auto& region : MapGeneratorComp->regions)
	{
		if (noiseValue < region.height)
			color = region.color;
		break;
	}

	return color;
}

void ADisplayMesh::GenerateMeshTexture()
{
	//UE_LOG(LogTemp, Warning, TEXT("GenerateMeshTexture"))

	int width = MapGeneratorComp->MapWidth;
	int height = MapGeneratorComp->MapHeight;

	TArray<FColor> ColorMap;
	ColorMap.Init(FColor::Black, NoiseMap.Num());

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			ColorMap[x + y * width] = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, NoiseMap[x + (y * width)]).ToFColor(false);
		}
	}

	UTexture2D* NoiseTexture = nullptr;

	NoiseTexture = FImageUtils::CreateTexture2D(width, height, ColorMap, this, FString("NoiseTexture"), EObjectFlags::RF_Public | EObjectFlags::RF_Transient, FCreateTexture2DParameters());
	NoiseTexture->Filter = TextureFilter::TF_Nearest;

	UMaterialInstanceDynamic* Material = nullptr;
	UMaterialInterface* LoadedMaterial = CustomMesh->GetMaterial(0);
	if (LoadedMaterial)
	{
		Material = UMaterialInstanceDynamic::Create(LoadedMaterial, nullptr);

		if (Material)
		{
			//Here we're applying your texture to the material
			Material->SetTextureParameterValue(TEXT("NoiseTexture"), NoiseTexture);

			//Finally, apply the material to your mesh component
			if (CustomMesh)
				CustomMesh->SetMaterial(0, Material);
		}
	}
}

void ADisplayMesh::GenerateNormals(int Width, int Height, int TopLeftX, int TopLeftY, TArray<FVector> &Normals)
{
	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			if (y != Height - 1)
			{
				if (x != Width - 1)
					AddNormal(x + (y * Width), (x + 1) + (y * Width), x + ((y + 1) * Width), TopLeftX, TopLeftY, Normals);
				else
					AddNormal(x + (y * Width), x + ((y + 1) * Width), (x - 1) + (y * Width), TopLeftX, TopLeftY, Normals);
			}
			else
			{
				if (x != Width - 1)
					AddNormal(x + (y * Width), x + ((y - 1) * Width), (x + 1) + ((y - 1) * Width), TopLeftX, TopLeftY, Normals);
				else
					AddNormal(x + (y * Width), (x - 1) + (y * Width), x + ((y - 1) * Width), TopLeftX, TopLeftY, Normals);
			}
		}
	}
}


void ADisplayMesh::AddNormal(const int Vertice1, const int Vertice2, const int Vertice3, const float TopLeftX, const float TopLeftY, TArray<FVector> &Normals)
{
	//UE_LOG(LogTemp, Warning, TEXT("AddNormal"))

	const FIntPoint Point1 = GetIndexValue(Vertice1);
	const FIntPoint Point2 = GetIndexValue(Vertice2);
	const FIntPoint Point3 = GetIndexValue(Vertice3);

	const FVector Vertex1 = FVector((TopLeftX + Point1.X) * MapGeneratorComp->MapScale, (TopLeftY - Point1.Y) * MapGeneratorComp->MapScale, NoiseMap[Point1.X + Point1.Y * MapGeneratorComp->MapWidth] * MapGeneratorComp->MapHeightScale);
	const FVector Vertex2 = FVector((TopLeftX + Point2.X) * MapGeneratorComp->MapScale, (TopLeftY - Point2.Y) * MapGeneratorComp->MapScale, NoiseMap[Point2.X + Point2.Y * MapGeneratorComp->MapWidth] * MapGeneratorComp->MapHeightScale);
	const FVector Vertex3 = FVector((TopLeftX + Point3.X) * MapGeneratorComp->MapScale, (TopLeftY - Point3.Y) * MapGeneratorComp->MapScale, NoiseMap[Point3.X + Point3.Y * MapGeneratorComp->MapWidth] * MapGeneratorComp->MapHeightScale);

	FVector Normal1 = FVector::CrossProduct((Vertex3 - Vertex1), (Vertex2 - Vertex1));

	Normal1.Normalize(1.0f);

	Normals.Add(Normal1);
}

FIntPoint ADisplayMesh::GetIndexValue(const int Vertice)
{
	return { Vertice % MapGeneratorComp->MapWidth, Vertice / MapGeneratorComp->MapWidth };
}

