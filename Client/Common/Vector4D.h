#pragma once
#include "EngineMath.h"

struct FVector4D
{
    float _x = 0.f;
    float _y = 0.f;
    float _z = 0.f;
    float _w = 0.f;

    FVector4D();
    FVector4D(float x, float y, float z, float w);
    FVector4D(const FVector4D& other);
    FVector4D(FVector4D&& other) noexcept;

    //operator =
    const FVector4D& operator=(const FVector4D& other);
    const FVector4D& operator=(float value);
    const FVector4D& operator=(double value);
    const FVector4D& operator=(int32 value);

    //operator +
    FVector4D operator+(const FVector4D& other) const;
    FVector4D operator+(const float value) const;
    FVector4D operator+(const double value) const;
    FVector4D operator+(const int32 value) const;

    //operator +=
    const FVector4D& operator+=(const FVector4D& other);
    const FVector4D& operator+=(const float value);
    const FVector4D& operator+=(const double value);
    const FVector4D& operator+=(const int32 value);

    //operator ++
    const FVector4D& operator ++();
    FVector4D operator ++(int32);

    //operator -
    FVector4D operator-(const FVector4D& other) const;
    FVector4D operator-(const float value) const;
    FVector4D operator-(const double value) const;
    FVector4D operator-(const int32 value) const;

    //operator -=
    const FVector4D& operator-=(const FVector4D& other);
    const FVector4D& operator-=(const float value);
    const FVector4D& operator-=(const double value);
    const FVector4D& operator-=(const int32 value);

    //operator --
    const FVector4D& operator --();
    FVector4D operator --(int);

    //operator *
    FVector4D operator*(const FVector4D& other) const;
    FVector4D operator*(const float value) const;
    FVector4D operator*(const double value) const;
    FVector4D operator*(const int32 value) const;

    //operator *=
    const FVector4D& operator*=(const FVector4D& other);
    const FVector4D& operator*=(const float value);
    const FVector4D& operator*=(const double value);
    const FVector4D& operator*=(const int32 value);

    //operator /
    FVector4D operator/(const FVector4D& other) const;
    FVector4D operator/(const float value) const;
    FVector4D operator/(const double value) const;
    FVector4D operator/(const int32 value) const;

    //operator /=
    const FVector4D& operator/=(const FVector4D& other);
    const FVector4D& operator/=(const float value);
    const FVector4D& operator/=(const double value);
    const FVector4D& operator/=(const int32 value);

    //operator ==
    bool operator ==(const FVector4D& other);

    //operator !=
    bool operator !=(const FVector4D& other);

    float& operator[](int idx);


};
