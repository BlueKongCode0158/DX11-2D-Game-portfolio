#pragma once

struct FTileInstanceData
{
	FVector2D _worldPos;
	FVector2D _uvLT;
	FVector2D _uvRB;
	FVector2D _size;
};

struct FTileLineInstanceData
{
	FVector2D _worldPos;
	FVector4D _color;
	FVector2D _size;
};

struct FParticleInstance
{
	FVector3D _position;
	float	  _size;
	FVector4D _color;
	float	  _rotation;
	FVector2D _uvLT;
	FVector2D _uvRB;
};