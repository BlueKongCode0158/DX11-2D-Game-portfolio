#include "pch.h"
#include "Vector4D.h"

FVector4D::FVector4D() : _x(0), _y(0), _z(0), _w(0)
{}

FVector4D::FVector4D(float x, float y, float z, float w) : _x(x), _y(y), _z(z), _w(w)
{}

FVector4D::FVector4D(const FVector4D& other) : _x(other._x), _y(other._y), _z(other._z), _w(other._w)
{}

FVector4D::FVector4D(FVector4D&& other) noexcept : _x(other._x), _y(other._y), _z(other._z), _w(other._w)
{}

const FVector4D& FVector4D::operator=(const FVector4D& other)
{
    _x = other._x;
    _y = other._y;
    _z = other._z;
    _w = other._w;

    return *this;
}

const FVector4D& FVector4D::operator=(float value)
{
    _x = value;
    _y = value;
    _z = value;
    _w = value;

    return *this;
}

const FVector4D& FVector4D::operator=(double value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;
    _z = val;
    _w = val;

    return *this;
}

const FVector4D& FVector4D::operator=(int32 value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;
    _z = val;
    _w = val;

    return *this;
}

FVector4D FVector4D::operator+(const FVector4D& other) const
{
    FVector4D retVal;
    retVal._x = _x + other._x;
    retVal._y = _y + other._y;
    retVal._z = _z + other._z;
    retVal._w = _w + other._w;

    return retVal;
}

FVector4D FVector4D::operator+(const float value) const
{
    FVector4D retVal;
    retVal._x = _x + value;
    retVal._y = _y + value;
    retVal._z = _z + value;
    retVal._w = _w + value;

    return retVal;
}

FVector4D FVector4D::operator+(const double value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;
    retVal._z = _z + val;
    retVal._w = _w + val;

    return retVal;
}

FVector4D FVector4D::operator+(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;
    retVal._z = _z + val;
    retVal._w = _w + val;

    return retVal;
}

const FVector4D& FVector4D::operator+=(const FVector4D& other)
{
    _x += other._x;
    _y += other._y;
    _z += other._z;
    _w += other._w;

    return *this;
}

const FVector4D& FVector4D::operator+=(const float value)
{
    _x += value;
    _y += value;
    _z += value;
    _w += value;

    return *this;
}

const FVector4D& FVector4D::operator+=(const double value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;
    _z += val;
    _w += val;

    return *this;
}

const FVector4D& FVector4D::operator+=(const int32 value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;
    _z += val;
    _w += val;

    return *this;
}

const FVector4D& FVector4D::operator++()
{
    ++_x;
    ++_y;
    ++_z;
    ++_w;

    return *this;
}

FVector4D FVector4D::operator++(int32)
{
    FVector4D retVal(_x, _y, _z, _w);
    ++_x;
    ++_y;
    ++_z;
    ++_w;

    return *this;
}

FVector4D FVector4D::operator-(const FVector4D& other) const
{
    FVector4D retVal;
    retVal._x = _x - other._x;
    retVal._y = _y - other._y;
    retVal._z = _z - other._z;
    retVal._w = _w - other._w;

    return retVal;
}

FVector4D FVector4D::operator-(const float value) const
{
    FVector4D retVal;
    retVal._x = _x - value;
    retVal._y = _y - value;
    retVal._z = _z - value;
    retVal._w = _w - value;
    
    return retVal;
}

FVector4D FVector4D::operator-(const double value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x - val;
    retVal._y = _y - val;
    retVal._z = _z - val;
    retVal._w = _w - val;

    return retVal;
}

FVector4D FVector4D::operator-(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x - value;
    retVal._y = _y - value;
    retVal._z = _z - value;
    retVal._w = _w - value;

    return retVal;
}

const FVector4D& FVector4D::operator-=(const FVector4D& other)
{
    _x -= other._x;
    _y -= other._y;
    _z -= other._z;
    _w -= other._w;

    return *this;
}

const FVector4D& FVector4D::operator-=(const float value)
{
    _x -= value;
    _y -= value;
    _z -= value;
    _w -= value;

    return *this;
}

const FVector4D& FVector4D::operator-=(const double value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;
    _z -= val;
    _w -= val;

    return *this;
}

const FVector4D& FVector4D::operator-=(const int32 value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;
    _z -= val;
    _w -= val;
 
    return *this;
}

const FVector4D& FVector4D::operator--()
{
    --_x;
    --_y;
    --_z;
    --_w;

    return *this;
}

FVector4D FVector4D::operator--(int)
{
    FVector4D retVal(_x, _y, _z, _w);
    --_x;
    --_y;
    --_z;
    --_w;

    return retVal;
}

FVector4D FVector4D::operator*(const FVector4D& other) const
{
    FVector4D retVal;
    retVal._x = _x * other._x;
    retVal._y = _y * other._y;
    retVal._z = _z * other._z;
    retVal._w = _w * other._w;

    return retVal;
}

FVector4D FVector4D::operator*(const float value) const
{
    FVector4D retVal;
    retVal._x = _x * value;
    retVal._y = _y * value;
    retVal._z = _z * value;
    retVal._w = _w * value;

    return retVal;
}

FVector4D FVector4D::operator*(const double value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;
    retVal._z = _z * val;
    retVal._y = _y * val;

    return retVal;
}

FVector4D FVector4D::operator*(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;
    retVal._z = _z * val;
    retVal._w = _w * val;

    return retVal;
}

const FVector4D& FVector4D::operator*=(const FVector4D& other)
{
    _x *= other._x;
    _y *= other._y;
    _z *= other._z;
    _w *= other._w;


    return *this;
}

const FVector4D& FVector4D::operator*=(const float value)
{
    _x *= value;
    _y *= value;
    _z *= value;
    _w *= value;

    return *this;
}

const FVector4D& FVector4D::operator*=(const double value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;
    _z *= val;
    _w *= val;

    return *this;
}

const FVector4D& FVector4D::operator*=(const int32 value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;
    _z *= val;
    _w *= val;

    return *this;
}

FVector4D FVector4D::operator/(const FVector4D& other) const
{
    FVector4D retVal;
    retVal._x = _x / other._x;
    retVal._y = _y / other._y;
    retVal._z = _z / other._z;
    retVal._w = _w / other._w;

    return retVal;
}

FVector4D FVector4D::operator/(const float value) const
{
    FVector4D retVal;
    retVal._x = _x / value;
    retVal._y = _y / value;
    retVal._z = _z / value;
    retVal._w = _w / value;

    return retVal;
}

FVector4D FVector4D::operator/(const double value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;
    retVal._z = _z / val;
    retVal._w = _w / val;

    return retVal;
}

FVector4D FVector4D::operator/(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector4D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;
    retVal._z = _z / val;
    retVal._w = _w / val;

    return retVal;
}

const FVector4D& FVector4D::operator/=(const FVector4D& other)
{
    _x /= other._x;
    _y /= other._y;
    _z /= other._z;
    _w /= other._w;

    return *this;
}

const FVector4D& FVector4D::operator/=(const float value)
{
    _x /= value;
    _y /= value;
    _z /= value;
    _w /= value;

    return *this;
}

const FVector4D& FVector4D::operator/=(const double value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;
    _z /= val;
    _w /= val;

    return *this;
}

const FVector4D& FVector4D::operator/=(const int32 value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;
    _z /= val;
    _w /= val;

    return *this;
}

bool FVector4D::operator==(const FVector4D& other)
{
    return (fabsf(_x - other._x) < FLT_EPSILON &&
        fabsf(_y - other._y) < FLT_EPSILON &&
        fabsf(_z - other._z) < FLT_EPSILON &&
        fabsf(_w - other._w) < FLT_EPSILON);
}

bool FVector4D::operator!=(const FVector4D& other)
{
    return !(*this == other);
}

float& FVector4D::operator[](int idx)
{
    switch (idx)
    {
    case 0:
        return _x;
    case 1:
        return _y;
    case 2:
        return _z;
    case 3:
        return _w;
    default:
        return _x;
    }
}
