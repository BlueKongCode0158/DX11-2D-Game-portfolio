#pragma once
#include <math.h>

//벡터
//힘(크기)과 방향을 가진 물리량
//게임에서는 주로 이걸 통해 객체의 크기, 위치를 표현

//단위벡터
//크기가 1인 벡터
//벡터 --> 단위벡터(정규화) normalize

//벡터 +, -

//벡터 * 
//벡터의 내적
//A * B = |A||B|cos0
//만약에 A와 B벡터가 둘 다 단위벡터라면
//두 벡터의 내적의 값은 cos0

//벡터의 외적
//A * B = A 와 B의 직교벡터
//A, B에 전부 수직인 벡터

//반사벡터
//V - 2(V*N)N
//V : 입사벡터
//N : 법선벡터

struct FVector2D
{
    float _x = 0.f;
    float _y = 0.f;

    FVector2D();
    FVector2D(float x, float y);
    FVector2D(const FVector2D& other);
    FVector2D(FVector2D&& other) noexcept ;

    //operator =
    const FVector2D& operator=(const FVector2D& other);
    const FVector2D& operator=(float value);
    const FVector2D& operator=(double value);
    const FVector2D& operator=(int32 value);

    //operator +
    FVector2D operator+(const FVector2D& other) const;
    FVector2D operator+(const float value) const;
    FVector2D operator+(const double value) const;
    FVector2D operator+(const int32 value) const;

    //operator +=
    const FVector2D& operator+=(const FVector2D& other);
    const FVector2D& operator+=(const float value);
    const FVector2D& operator+=(const double value);
    const FVector2D& operator+=(const int32 value);

    //operator ++
    const FVector2D& operator ++();
    FVector2D operator ++(int);

    //operator -
    FVector2D operator-(const FVector2D& other) const;
    FVector2D operator-(const float other) const;
    FVector2D operator-(const double other) const;
    FVector2D operator-(const int32 other) const;

    //operator -=
    const FVector2D& operator-=(const FVector2D& other);
    const FVector2D& operator-=(const float value);
    const FVector2D& operator-=(const double value);
    const FVector2D& operator-=(const int32 value);

    //operator --
    const FVector2D& operator --();
    FVector2D operator --(int32);

    //operator *
    FVector2D operator*(const FVector2D& other) const;
    FVector2D operator*(const float other) const;
    FVector2D operator*(const double other) const;
    FVector2D operator*(const int32 other) const;

    //operator *=
    const FVector2D& operator*=(const FVector2D& other);
    const FVector2D& operator*=(const float value);
    const FVector2D& operator*=(const double value);
    const FVector2D& operator*=(const int32 value);

    //operator /
    FVector2D operator/(const FVector2D& other) const;
    FVector2D operator/(const float other) const;
    FVector2D operator/(const double other) const;
    FVector2D operator/(const int32 other) const;

    //operator /=
    const FVector2D& operator/=(const FVector2D& other);
    const FVector2D& operator/=(const float value);
    const FVector2D& operator/=(const double value);
    const FVector2D& operator/=(const int32 value);

    //operator ==
    bool operator ==(const FVector2D& other);

    //operator !=
    bool operator !=(const FVector2D& other);

    //둘 다 벡터의 크기
    float Length() const;
    float Size() const;

    //벡터의 정규화
    void Normalize();

    //벡터의 내적
    float Dot(const FVector2D& other) const;

    //벡터의 외적 (힘) [ 평행사변형의 유향 넓이(Signed Area) ]
    float Cross(const FVector2D& other) const;

    //두 벡터 사이의 거리
    float Distance(const FVector2D& other) const;


    //벡터의 정규화
    static FVector2D Normalize(const FVector2D& other);

    static FVector2D Zero;
};

