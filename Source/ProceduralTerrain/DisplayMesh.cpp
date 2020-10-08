// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayMesh.h"
#include "ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "ImageUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "FastNoise.h"

// Sets default values
ADisplayMesh::ADisplayMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));

	RootComponent = RootComp;
	CustomMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADisplayMesh::BeginPlay()
{
	Super::BeginPlay();

	GenerateMeshTexture();
}

// Called every frame
void ADisplayMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADisplayMesh::GenerateTerrainMesh()
{
	const int Width = NoiseParameters.MapWidth;
	const int Height = NoiseParameters.MapHeight;
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

	FProcMeshTangent TangentSetup = FProcMeshTangent(0.0f, 1.0f, 0.0f);

	int vertexIndex = 0;

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			Vertices[x + y * Width] = (FVector((TopLeftX + x) * NoiseParameters.MapScale, (TopLeftY - y) * NoiseParameters.MapScale, GetNoiseValueAtIndex(x, y)));
			UVs[x + y * Width] = (FVector2D(x / (float)Width, y / (float)Height));
			//Colors[x + y * Width] = (GetNoiseColor(GetNoiseValueAtIndex(x, y)).ToFColor(false));

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
}

void ADisplayMesh::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//GenerateNoiseMapData();
	GenerateTerrainMesh();
}

void ADisplayMesh::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("Property: %s Changed !!!"), *PropertyChangedEvent.GetPropertyName().ToString())
	//GenerateNoiseMapData();
	//GenerateTerrainMesh();
}

FColor ADisplayMesh::GetNoiseColor(const float noiseValue)
{
	FColor color;

	for (auto& region : NoiseParameters.regions)
	{
		if (noiseValue < region.height)
		{
			color = region.color;
			break;
		}
	}

	return color;
}

void ADisplayMesh::GenerateMeshTexture()
{
	int width = NoiseParameters.MapWidth;
	int height = NoiseParameters.MapHeight;

	TArray<FColor> ColorMap;
	ColorMap.Init(FColor::Black, width * height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			ColorMap[x + y * width] = GetNoiseColor(GetNoiseValueAtIndex(x, y));
		}
	}

	UTexture2D* NoiseTexture = nullptr;
	
	FCreateTexture2DParameters Texture2DParameters;
	Texture2DParameters.TextureGroup = TEXTUREGROUP_Pixels2D;
	
	NoiseTexture = FImageUtils::CreateTexture2D(width, height, ColorMap, this, FString("NoiseTexture"), EObjectFlags::RF_Public | EObjectFlags::RF_Transient, Texture2DParameters);

	UMaterialInstanceDynamic* Material = nullptr;
	UMaterialInterface* LoadedMaterial = CustomMesh->GetMaterial(0);
	if (LoadedMaterial)
	{
		Material = UMaterialInstanceDynamic::Create(LoadedMaterial, nullptr);

		if (Material)
		{
			Material->SetTextureParameterValue(TEXT("NoiseTexture"), NoiseTexture);

			if (CustomMesh)
				CustomMesh->SetMaterial(0, Material);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Material Not Found :/"))
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
	const FIntPoint Point1 = GetIndexValue(Vertice1);
	const FIntPoint Point2 = GetIndexValue(Vertice2);
	const FIntPoint Point3 = GetIndexValue(Vertice3);

	const FVector Vertex1 = FVector((TopLeftX + Point1.X) * NoiseParameters.MapScale, (TopLeftY - Point1.Y) * NoiseParameters.MapScale, GetNoiseValueAtIndex(Point1.X, Point1.Y));
	const FVector Vertex2 = FVector((TopLeftX + Point2.X) * NoiseParameters.MapScale, (TopLeftY - Point2.Y) * NoiseParameters.MapScale, GetNoiseValueAtIndex(Point2.X, Point2.Y));
	const FVector Vertex3 = FVector((TopLeftX + Point3.X) * NoiseParameters.MapScale, (TopLeftY - Point3.Y) * NoiseParameters.MapScale, GetNoiseValueAtIndex(Point3.X, Point3.Y));

	FVector Normal1 = FVector::CrossProduct((Vertex3 - Vertex1), (Vertex2 - Vertex1));

	Normal1.Normalize(1.0f);

	Normals.Add(Normal1);
}

FIntPoint ADisplayMesh::GetIndexValue(const int Vertice)
{
	return { Vertice % NoiseParameters.MapWidth, Vertice / NoiseParameters.MapWidth };
}

float ADisplayMesh::GetNoiseValueAtIndex(int x, int y)
{
	FastNoise noise;
	noise.SetNoiseType(FastNoise::Perlin);

	FRandomStream stream;
	stream.Initialize(NoiseParameters.Seed);

	TArray<FVector2D> octaveOffsets;
	octaveOffsets.Reserve(NoiseParameters.Octaves);
	octaveOffsets.Init(FVector2D::ZeroVector, NoiseParameters.Octaves);

	for (int i = 0; i < NoiseParameters.Octaves; ++i)
	{
		float offsetX = stream.FRandRange(-100000.f, 100000.f) + NoiseParameters.Offset.X;
		float offsetY = stream.FRandRange(-100000.f, 100000.f) + NoiseParameters.Offset.Y;
		octaveOffsets[i] = FVector2D(offsetX, offsetY);
	}

	float maxNoiseHeight = std::numeric_limits<float>::min();
	float minNoiseHeight = std::numeric_limits<float>::max();

	float halfWidth = NoiseParameters.MapWidth / 2.f;
	float halfHeigh = NoiseParameters.MapHeight / 2.f;

	float amplitude = 1;
	float frequency = 1;
	float noiseHeight = 0;

	for (int i = 0; i < NoiseParameters.Octaves; ++i)
	{
		float sampleX = (x - halfWidth) / NoiseParameters.NoiseScale * frequency;
		float sampleY = (y - halfHeigh) / NoiseParameters.NoiseScale * frequency;

		float perlinValue = noise.GetNoise(sampleX, sampleY);
		noiseHeight += perlinValue * amplitude;

		amplitude *= NoiseParameters.Persistance;
		frequency *= NoiseParameters.Lacunarity;
	}

	noiseHeight = FMath::Abs(noiseHeight);
	noiseHeight *= NoiseParameters.MapHeightScale;

	if(ShowNoiseValue)
		UE_LOG(LogTemp, Warning, TEXT("NoiseValue [%d, %d]: %.3f    [%d]"), x, y, noiseHeight)

	if (noiseHeight > NoiseParameters.MaxHeight)
		noiseHeight = NoiseParameters.MaxHeight;
	if (noiseHeight < NoiseParameters.MinHeight)
		noiseHeight = NoiseParameters.MinHeight;
	
	return noiseHeight;
}