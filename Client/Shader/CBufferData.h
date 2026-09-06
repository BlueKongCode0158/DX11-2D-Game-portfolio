#pragma once
/*
공간 변환
로컬		[ 고유 공간 ]
월드		[ 객체들이 움직이는 공간 ]
카메라	[ 카메라 시점가 바라보는 공간 ]
투영		[ 2D로 변환하는 공간 ]
*/

// GPU는 열우선으로 읽으려고 한다.

struct FTransformCBufferData
{
	FMatrix _world;
	FMatrix _view;
	FMatrix _proj;
	FMatrix _wv;
	FMatrix _wvp;
};

struct FColorCBufferData
{
	FVector4D _color;
};

struct FMaterialCBufferData
{
	FVector4D _baseColor;
	float _opacity;
	int32 _textureWidth;
	int32 _textureHeight;
	float _empty;
};
// dummy 자료형은 딱히 상관없음. 크기만 맞추면 됨.

struct FAnimationCBufferData
{
	FVector2D _LTUV;
	FVector2D _RBUV;
	FVector2D _offset;
	int32 _animFlip;
	int32 _animRotated90;
};

struct FSpriteCBufferData
{
	FVector4D _tint;
	FVector2D _ParallaxOffset;
	FVector2D _ParallaxPivot;
	float	  _ParallaxScale;
	FVector3D _empty;
};


struct FTileMapCBufferData
{
	FVector2D _uvLT;
	FVector2D _uvRB;
};

struct FUIBrushData
{
	FVector4D _tint;
	FVector2D _uvLT;
	FVector2D _uvRB;
	int32 _animEnable;
	int32 _textureEnable;
	int32 _isRotated;
	int32 _empty;
};

struct FProgressBarData
{
	float _percent = 1.f;
	int32 _barDir = 0;
	FVector2D _empty;
};

struct FFogPlaneData
{
	FVector4D _tint;
	FVector2D _screemSize;
	float _Time;
	float _Dist;
};

struct FPartocleCBufferData
{
	int32		_useTexture;
	FVector3D	_pad;
};

struct FCompisiteDebugData
{
	int32 _showScene;
	int32 _showLight; 
	FVector2D _empty;
};