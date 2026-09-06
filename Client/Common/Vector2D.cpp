#include "pch.h"
#include "Vector2D.h"

FVector2D::FVector2D() : _x(0), _y(0)
{}

FVector2D::FVector2D(float x, float y) : _x(x), _y(y)
{}

FVector2D::FVector2D(const FVector2D & other) : _x(other._x), _y(other._y)
{}

FVector2D::FVector2D(FVector2D && other) noexcept : _x(other._x), _y(other._y) 
{}

const FVector2D& FVector2D::operator=(const FVector2D & other)
{
    _x = other._x;
    _y = other._y;

    return *this;
}

const FVector2D& FVector2D::operator=(float value)
{
    _x = value;
    _y = value;

    return *this;
}

const FVector2D& FVector2D::operator=(double value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;

    return *this;
}

const FVector2D& FVector2D::operator=(int32 value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;

    return *this;
}

FVector2D FVector2D::operator+(const FVector2D& other) const
{
    FVector2D retVal;
    retVal._x = _x + other._x;
    retVal._y = _y + other._y;

    return retVal;
}

FVector2D FVector2D::operator+(const float value) const
{
    FVector2D retVal;
    retVal._x = _x + value;
    retVal._y = _y + value;

    return retVal;
}

FVector2D FVector2D::operator+(const double value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;

    return retVal;
}

FVector2D FVector2D::operator+(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;

    return retVal;
}

const FVector2D& FVector2D::operator+=(const FVector2D& other)
{
    _x += other._x;
    _y += other._y;

    return *this;
}

const FVector2D& FVector2D::operator+=(const float value)
{
    _x += value;
    _y += value;

    return *this;
}

const FVector2D& FVector2D::operator+=(const double value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;

    return *this;
}

const FVector2D& FVector2D::operator+=(const int32 value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;

    return *this;
}

const FVector2D& FVector2D::operator++()
{
    ++_x;
    ++_y;

    return *this;
}

FVector2D FVector2D::operator++(int)
{
    FVector2D retVal(_x, _y);
    ++_x;
    ++_y;

    return retVal;
}

FVector2D FVector2D::operator-(const FVector2D& other) const
{
    FVector2D retVal;
    retVal._x = _x - other._x;
    retVal._y = _y - other._y;

    return retVal;
}

FVector2D FVector2D::operator-(const float value) const
{
    FVector2D retVal;
    retVal._x = _x - value;
    retVal._y = _y - value;

    return retVal;

}

FVector2D FVector2D::operator-(const double value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x - val;
    retVal._y = _y - val;

    return retVal;
}

FVector2D FVector2D::operator-(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x - val;
    retVal._y = _y - val;

    return retVal;
}

const FVector2D& FVector2D::operator-=(const FVector2D& other)
{
    _x -= other._x;
    _y -= other._y;

    return *this;
}

const FVector2D& FVector2D::operator-=(const float value)
{
    _x -= value;
    _y -= value;

    return *this;
}

const FVector2D& FVector2D::operator-=(const double value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;

    return *this;
}

const FVector2D& FVector2D::operator-=(const int32 value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;

    return *this;
}

const FVector2D& FVector2D::operator--()
{
    --_x;
    --_y;

    return *this;
}

FVector2D FVector2D::operator--(int32)
{
    FVector2D retVal(_x, _y);
    --_x;
    --_y;

    return retVal;
}

FVector2D FVector2D::operator*(const FVector2D& other) const
{
    FVector2D retVal;
    retVal._x = _x * other._x;
    retVal._y = _y * other._y;

    return retVal;
}

FVector2D FVector2D::operator*(const float value) const
{
    FVector2D retVal;
    retVal._x = _x * value;
    retVal._y = _y * value;

    return retVal;
}

FVector2D FVector2D::operator*(const double value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;

    return retVal;
}

FVector2D FVector2D::operator*(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;

    return retVal;
}

const FVector2D& FVector2D::operator*=(const FVector2D& other)
{
    _x *= other._x;
    _y *= other._y;

    return *this;
}

const FVector2D& FVector2D::operator*=(const float value)
{
    _x *= value;
    _y *= value;

    return *this;
}

const FVector2D& FVector2D::operator*=(const double value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;

    return *this;
}

const FVector2D& FVector2D::operator*=(const int32 value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;

    return *this;
}

FVector2D FVector2D::operator/(const FVector2D& other) const
{
    FVector2D retVal;
    retVal._x = _x / other._x;
    retVal._y = _y / other._y;

    return retVal;
}

FVector2D FVector2D::operator/(const float value) const
{
    FVector2D retVal;
    retVal._x = _x / value;
    retVal._y = _y / value;

    return retVal;
}

FVector2D FVector2D::operator/(const double value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;

    return retVal;
}

FVector2D FVector2D::operator/(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector2D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;

    return retVal;
}

const FVector2D& FVector2D::operator/=(const FVector2D& other)
{
    _x /= other._x;
    _y /= other._y;

    return *this;
}

const FVector2D& FVector2D::operator/=(const float value)
{
    _x /= value;
    _y /= value;

    return *this;
}

const FVector2D& FVector2D::operator/=(const double value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;

    return *this;
}

const FVector2D& FVector2D::operator/=(const int32 value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;

    return *this;
}

bool FVector2D::operator==(const FVector2D& other)
{
    return (fabsf(_x - other._x) < FLT_EPSILON &&
        fabsf(_y - other._y) < FLT_EPSILON);
}

bool FVector2D::operator!=(const FVector2D& other)
{
    return !(*this == other);
}

float FVector2D::Length() const
{
    return sqrtf(_x * _x + _y * _y);
}

float FVector2D::Size() const
{
    return Length();
}

void FVector2D::Normalize()
{
    float size = Length();
    if (size == 0.f)
        return;

    _x /= size;
    _y /= size;
}

float FVector2D::Dot(const FVector2D & other) const
{
    return _x * other._x + _y * other._y;
}

float FVector2D::Cross(const FVector2D& other) const
{
    return _x * other._y - _y * other._x;
}

float FVector2D::Distance(const FVector2D& other) const
{
    FVector2D v = *this - other;
    return v.Length();
}

FVector2D FVector2D::Normalize(const FVector2D& other)
{
    FVector2D v(other);
    v.Normalize();

    return v;
}
