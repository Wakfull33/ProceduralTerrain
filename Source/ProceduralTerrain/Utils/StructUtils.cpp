// Fill out your copyright notice in the Description page of Project Settings.


#include "StructUtils.h"

FNoiseParameters::FNoiseParameters()
{
	if (MapWidth < 1)
		MapWidth = 1;
	if (MapHeight < 1)
		MapHeight = 1;
	if (Lacunarity < 1)
		Lacunarity = 1;
	if (Octaves < 0)
		Octaves = 0;
	if (NoiseScale <= 0)
		NoiseScale = 0.0001f;
}
