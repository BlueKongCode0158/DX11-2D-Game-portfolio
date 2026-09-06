#include "pch.h"
#include "Vector3D.h"
#include "Matrix.h"

FVector3D FVector3D::Zero = FVector3D(0.f, 0.f, 0.f);
FVector3D FVector3D::Axis_X = FVector3D(1.f, 0.f, 0.f);
FVector3D FVector3D::Axis_Y = FVector3D(0.f, 1.f, 0.f);
FVector3D FVector3D::Axis_Z = FVector3D(0.f, 0.f, 1.f);


FVector3D::FVector3D() : _x(0), _y(0), _z(0)
{}

FVector3D::FVector3D(float x, float y, float z) : _x(x), _y(y), _z(z)
{}

FVector3D::FVector3D(const FVector3D & other) : _x(other._x), _y(other._y), _z(other._z)
{}

FVector3D::FVector3D(FVector3D && other) noexcept : _x(other._x), _y(other._y), _z(other._z)
{
}

FVector3D::FVector3D(const DirectX::XMVECTOR v)
{
    DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)this, v);
}

const FVector3D& FVector3D::operator=(const FVector3D & other)
{
    _x = other._x;
    _y = other._y;
    _z = other._z;

    return *this;
}

const FVector3D& FVector3D::operator=(float value)
{
    _x = value;
    _y = value;
    _z = value;

    return *this;
}

const FVector3D& FVector3D::operator=(double value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;
    _z = val;

    return *this;
}

const FVector3D& FVector3D::operator=(int32 value)
{
    float val = static_cast<float>(value);
    _x = val;
    _y = val;
    _z = val;

    return *this;
}

FVector3D FVector3D::operator+(const FVector3D& other) const
{
    FVector3D retVal;
    retVal._x = _x + other._x;
    retVal._y = _y + other._y;
    retVal._z = _z + other._z;

    return retVal;
}

FVector3D FVector3D::operator+(const float value) const
{
    FVector3D retVal;
    retVal._x = _x + value;
    retVal._y = _y + value;
    retVal._z = _z + value;

    return retVal;
}

FVector3D FVector3D::operator+(const double value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;
    retVal._z = _z + val;

    return retVal;
}

FVector3D FVector3D::operator+(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x + val;
    retVal._y = _y + val;
    retVal._z = _z + val;

    return retVal;
}

const FVector3D& FVector3D::operator+=(const FVector3D& other)
{
    _x += other._x;
    _y += other._y;
    _z += other._z;

    return *this;
}

const FVector3D& FVector3D::operator+=(const float value)
{
    _x += value;
    _y += value;
    _z += value;

    return *this;
}

const FVector3D& FVector3D::operator+=(const double value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;
    _z += val;

    return *this;
}

const FVector3D& FVector3D::operator+=(const int32 value)
{
    float val = static_cast<float>(value);
    _x += val;
    _y += val;
    _z += val;

    return *this;
}

const FVector3D& FVector3D::operator++()
{
    ++_x;
    ++_y;
    ++_z;

    return *this;
}

FVector3D FVector3D::operator++(int)
{
    FVector3D retVal(_x, _y, _z);
    ++_x;
    ++_y;
    ++_z;

    return *this;
}

FVector3D FVector3D::operator-(const FVector3D& other) const
{
    FVector3D retVal;
    retVal._x = _x - other._x;
    retVal._y = _y - other._y;
    retVal._z = _z - other._z;

    return retVal;
}

FVector3D FVector3D::operator-(const float value) const
{
    FVector3D retVal;
    retVal._x = _x - value;
    retVal._y = _y - value;
    retVal._z = _z - value;

    return retVal;
}

FVector3D FVector3D::operator-(const double value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x - val;
    retVal._y = _y - val;
    retVal._z = _z - val;

    return retVal;
}

FVector3D FVector3D::operator-(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x - value;
    retVal._y = _y - value;
    retVal._z = _z - value;

    return retVal;
}

FVector3D FVector3D::operator-() const
{
    return FVector3D(-_x, -_y, -_z);
}

const FVector3D& FVector3D::operator-=(const FVector3D& other)
{
    _x -= other._x;
    _y -= other._y;
    _z -= other._z;

    return *this;
}

const FVector3D& FVector3D::operator-=(const float value)
{
    _x -= value;
    _y -= value;
    _z -= value;

    return *this;
}

const FVector3D& FVector3D::operator-=(const double value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;
    _z -= val;

    return *this;
}

const FVector3D& FVector3D::operator-=(const int32 value)
{
    float val = static_cast<float>(value);
    _x -= val;
    _y -= val;
    _z -= val;

    return *this;
}

const FVector3D& FVector3D::operator--()
{
    --_x;
    --_y;
    --_z;

    return *this;
}

FVector3D FVector3D::operator--(int32)
{
    FVector3D retVal(_x, _y, _z);
    --_x;
    --_y;
    --_z;

    return retVal;
}

FVector3D FVector3D::operator*(const FVector3D& other) const
{
    FVector3D retVal;
    retVal._x = _x * other._x;
    retVal._y = _y * other._y;
    retVal._z = _z * other._z;

    return retVal;
}

FVector3D FVector3D::operator*(const float value) const
{
    FVector3D retVal;
    retVal._x = _x * value;
    retVal._y = _y * value;
    retVal._z = _z * value;

    return retVal;
}

FVector3D FVector3D::operator*(const double value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;
    retVal._z = _z * val;

    return retVal;
}

FVector3D FVector3D::operator*(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x * val;
    retVal._y = _y * val;
    retVal._z = _z * val;

    return retVal;
}

const FVector3D& FVector3D::operator*=(const FVector3D& other)
{
    _x *= other._x;
    _y *= other._y;
    _z *= other._z;

    return *this;
}

const FVector3D& FVector3D::operator*=(const float value)
{
    _x *= value;
    _y *= value;
    _z *= value;

    return *this;
}

const FVector3D& FVector3D::operator*=(const double value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;
    _z *= val;

    return *this;
}

const FVector3D& FVector3D::operator*=(const int32 value)
{
    float val = static_cast<float>(value);
    _x *= val;
    _y *= val;
    _z *= val;

    return *this;
}

FVector3D FVector3D::operator/(const FVector3D& other) const
{
    FVector3D retVal;
    retVal._x = _x / other._x;
    retVal._y = _y / other._y;
    retVal._z = _z / other._z;

    return retVal;
}

FVector3D FVector3D::operator/(const float value) const
{
    FVector3D retVal;
    retVal._x = _x / value;
    retVal._y = _y / value;
    retVal._z = _z / value;

    return retVal;
}

FVector3D FVector3D::operator/(const double value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;
    retVal._z = _z / val;

    return retVal;
}

FVector3D FVector3D::operator/(const int32 value) const
{
    float val = static_cast<float>(value);
    FVector3D retVal;
    retVal._x = _x / val;
    retVal._y = _y / val;
    retVal._z = _z / val;

    return retVal;
}

const FVector3D& FVector3D::operator/=(const FVector3D& other)
{
    _x /= other._x;
    _y /= other._y;
    _z /= other._z;

    return *this;
}

const FVector3D& FVector3D::operator/=(const float value)
{
    _x /= value;
    _y /= value;
    _z /= value;

    return *this;
}

const FVector3D& FVector3D::operator/=(const double value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;
    _z /= val;

    return *this;
}

const FVector3D& FVector3D::operator/=(const int32 value)
{
    float val = static_cast<float>(value);
    _x /= val;
    _y /= val;
    _z /= val;

    return *this;
}

bool FVector3D::operator==(const FVector3D& other)
{
    return (fabsf(_x - other._x) < FLT_EPSILON &&
        fabsf(_y - other._y) < FLT_EPSILON &&
        fabsf(_z - other._z) < FLT_EPSILON);
}

bool FVector3D::operator!=(const FVector3D& other)
{
    return !(*this == other);
}

float FVector3D::Length() const
{
    return sqrtf(_x * _x + _y * _y + _z * _z);
}

float FVector3D::Size() const
{
    return Length();
}

void FVector3D::Normalize()
{
    float size = Length();
    if (size == 0.f)
        return;

    _x /= size;
    _y /= size;
    _z /= size;
}

float FVector3D::Dot(const FVector3D & other) const
{
    return _x * other._x + _y * other._y + _z * other._z;
}

FVector3D FVector3D::Cross(const FVector3D& other) const
{
    return FVector3D(
        _y * other._z - _z * other._y,
        _z * other._x - _x * other._z,
        _x * other._y - _y * other._x);
}

float FVector3D::Distance(const FVector3D& other) const
{
    FVector3D v = *this - other;

    return v.Length();
}

FVector3D FVector3D::TransformNormal(const FMatrix& mat) const
{
    DirectX::XMMATRIX invTranspos = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, mat._m));
    DirectX::XMVECTOR ret = DirectX::XMVector3TransformNormal(Convert(), invTranspos);

    return ret;
}

FVector3D FVector3D::TransformCoord(const FMatrix& mat) const
{
    DirectX::XMVECTOR ret = DirectX::XMVector3TransformCoord(Convert(), mat._m);

    return ret;
}

bool FVector3D::IsZero() const
{
    return 0.f == this->_x && 0.f == this->_y && 0.f == this->_z ;
}

FVector3D FVector3D::Normalize(const FVector3D& other)
{
    FVector3D retVal(other);
    retVal.Normalize();

    return retVal;
}
