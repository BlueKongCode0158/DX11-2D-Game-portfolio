#pragma once
#include <math.h>
#include "EngineMath.h"

union FMatrix;
struct FVector3D
{
    float _x = 0.f;
    float _y = 0.f;
    float _z = 0.f;

    FVector3D();
    FVector3D(float x, float y, float z);
    FVector3D(const FVector3D& other);
    FVector3D(FVector3D&& other) noexcept;
    FVector3D(const DirectX::XMVECTOR v);

    //operator =
    const FVector3D& operator=(const FVector3D& other);
    const FVector3D& operator=(float value);
    const FVector3D& operator=(double value);
    const FVector3D& operator=(int32 value);

    //operator +
    FVector3D operator+(const FVector3D& other) const;
    FVector3D operator+(const float value) const;
    FVector3D operator+(const double value) const;
    FVector3D operator+(const int32 value) const;

    //operator +=
    const FVector3D& operator+=(const FVector3D& other);
    const FVector3D& operator+=(const float value);
    const FVector3D& operator+=(const double value);
    const FVector3D& operator+=(const int32 value);

    //operator ++
    const FVector3D& operator ++();
    FVector3D operator ++(int);

    //operator -
    FVector3D operator-(const FVector3D& other) const;
    FVector3D operator-(const float other) const;
    FVector3D operator-(const double other) const;
    FVector3D operator-(const int32 other) const;
    FVector3D operator-() const;

    //operator -=
    const FVector3D& operator-=(const FVector3D& other);
    const FVector3D& operator-=(const float value);
    const FVector3D& operator-=(const double value);
    const FVector3D& operator-=(const int32 value);

    //operator --
    const FVector3D& operator --();
    FVector3D operator --(int32);

    //operator *
    FVector3D operator*(const FVector3D& other) const;
    FVector3D operator*(const float other) const;
    FVector3D operator*(const double other) const;
    FVector3D operator*(const int32 other) const;

    //operator *=
    const FVector3D& operator*=(const FVector3D& other);
    const FVector3D& operator*=(const float value);
    const FVector3D& operator*=(const double value);
    const FVector3D& operator*=(const int32 value);

    //operator /
    FVector3D operator/(const FVector3D& other) const;
    FVector3D operator/(const float other) const;
    FVector3D operator/(const double other) const;
    FVector3D operator/(const int32 other) const;

    //operator /=
    const FVector3D& operator/=(const FVector3D& other);
    const FVector3D& operator/=(const float value);
    const FVector3D& operator/=(const double value);
    const FVector3D& operator/=(const int32 value);

    //operator ==
    bool operator ==(const FVector3D& other);

    //operator !=
    bool operator !=(const FVector3D& other);

    //둘 다 벡터의 크기
    float Length() const;
    float Size() const;

    //벡터의 정규화
    void Normalize();

    //벡터의 내적
    float Dot(const FVector3D& other) const;

    FVector3D Cross(const FVector3D& other) const;

    //두 벡터 사이의 거리
    float Distance(const FVector3D& other) const;

    FVector3D TransformNormal(const FMatrix& mat) const;    // 위치
    FVector3D TransformCoord(const  FMatrix& mat) const;     // 축방향변환
    bool IsZero() const;

    //벡터의 정규화
    static FVector3D Normalize(const FVector3D& other);
    DirectX::XMVECTOR Convert() const
    {
        return DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)this);
    }
    static FVector3D Zero;
    static FVector3D Axis_X;
    static FVector3D Axis_Y;
    static FVector3D Axis_Z;
} typedef FRotator;

