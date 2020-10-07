// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoise.h"
#include "Kismet/KismetMathLibrary.h"

/**
 *
 */
class PROCEDURALTERRAIN_API NoiseGenerator
{

private:
	static NoiseGenerator *s_instance;

public:
	static NoiseGenerator *instance()
	{
		if (!s_instance)
			s_instance = new NoiseGenerator;
		return s_instance;
	}

	static TArray<float> GenerateNoiseMap(int mapWidth, int mapHeight, int seed, float scale, int octaves, float persistance, float lacunarity, FVector2D offset)
	{
		FastNoise noise;
		noise.SetNoiseType(FastNoise::Perlin);

		TArray<float> noiseMap;
		noiseMap.Reserve(mapWidth * mapHeight);
		noiseMap.Init(0.0f, mapWidth * mapHeight);


		FRandomStream stream;
		stream.Initialize(seed);

		TArray<FVector2D> octaveOffsets;
		octaveOffsets.Reserve(octaves);
		octaveOffsets.Init(FVector2D::ZeroVector, octaves);

		for (int i = 0; i < octaves; ++i)
		{
			float offsetX = stream.FRandRange(-100000.f, 100000.f) + offset.X;
			float offsetY = stream.FRandRange(-100000.f, 100000.f) + offset.Y;
			octaveOffsets[i] = FVector2D(offsetX, offsetY);
		}


		if (scale <= 0)
		{
			scale = 0.0001f;
		}

		float maxNoiseHeight = std::numeric_limits<float>::min();
		float minNoiseHeight = std::numeric_limits<float>::max();

		float halfWidth = mapWidth / 2.f;
		float halfHeigh = mapHeight / 2.f;

		for (int y = 0; y < mapHeight; ++y)
		{
			for (int x = 0; x < mapWidth; ++x)
			{

				float amplitude = 1;
				float frequency = 1;
				float noiseHeight = 0;

				for (int i = 0; i < octaves; ++i)
				{
					float sampleX = (x - halfWidth) / scale * frequency + octaveOffsets[i].X;
					float sampleY = (y - halfHeigh) / scale * frequency + octaveOffsets[i].Y;

					float perlinValue = noise.GetNoise(sampleX, sampleY) * 2 - 1;
					noiseHeight += perlinValue * amplitude;

					amplitude *= persistance;
					frequency *= lacunarity;
				}

				if (noiseHeight > maxNoiseHeight)
				{
					maxNoiseHeight = noiseHeight;
				}
				else if (noiseHeight < minNoiseHeight)
				{
					minNoiseHeight = noiseHeight;
				}
				noiseMap[x + y * mapWidth] = noiseHeight;

			}
		}

		for (int y = 0; y < mapHeight; ++y)
		{
			for (int x = 0; x < mapWidth; ++x)
			{
				noiseMap[x + y * mapWidth] = UKismetMathLibrary::NormalizeToRange(noiseMap[x + y * mapWidth], minNoiseHeight, maxNoiseHeight);
			}
		}

		return noiseMap;
	}
};

NoiseGenerator* NoiseGenerator::s_instance = nullptr;
