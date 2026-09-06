#include "pch.h"
#include "Matrix.h"

FMatrix::FMatrix()
{
    _m = DirectX::XMMatrixIdentity();
}

FMatrix::FMatrix(const FMatrix& m)
{
    _m = m._m;
}

FMatrix::FMatrix(FMatrix&& m) noexcept
{
    _m = m._m;
}

FMatrix::FMatrix(const DirectX::XMMATRIX& m)
{
    _m = m;
}

FMatrix::FMatrix(DirectX::XMMATRIX&& m) noexcept
{
    _m = m;
}

FMatrix::FMatrix(const FVector4D v[4])
{
    memcpy(_v, v, sizeof(FVector4D) * 4);
}

FVector4D& FMatrix::operator[](int32 index)
{
    return _v[index];
}

const FMatrix& FMatrix::operator=(const FMatrix& m)
{
    _m = m._m;
    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

const FMatrix& FMatrix::operator=(const DirectX::XMMATRIX& m)
{
    _m = m;
    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

const FMatrix& FMatrix::operator=(const FVector4D v[4])
{
    memcpy(_v, v, sizeof(FVector4D) * 4);
    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

FMatrix FMatrix::operator*(const FMatrix& m) const
{
    return FMatrix(_m * m._m);
}

FMatrix FMatrix::operator*(const DirectX::XMMATRIX& m) const
{
    return FMatrix(_m * m);
}

void FMatrix::Indentity()
{
    _m = DirectX::XMMatrixIdentity();
}

void FMatrix::Transpose()
{
    _m = DirectX::XMMatrixTranspose(_m);
}

void FMatrix::Inverse()
{
    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(_m);
    _m = DirectX::XMMatrixInverse(&det, _m);
}

void FMatrix::Scaling(const FVector3D& v)
{
    _m = DirectX::XMMatrixScaling(v._x, v._y, v._z);
}

void FMatrix::Scaling(float x, float y, float z)
{
    _m = DirectX::XMMatrixScaling(x, y, z);
}

void FMatrix::Scaling(const FVector2D& v)
{
    _m = DirectX::XMMatrixScaling(v._x, v._y, 1);
}

void FMatrix::Scaling(float x, float y)
{
    _m = DirectX::XMMatrixScaling(x, y, 1);
}

void FMatrix::Rotation(const FVector3D& v)
{
    float xVal = DirectX::XMConvertToRadians(v._x);
    float yVal = DirectX::XMConvertToRadians(v._y);
    float zVal = DirectX::XMConvertToRadians(v._z);
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xVal, yVal, zVal);

    _m = DirectX::XMMatrixRotationQuaternion(quat);
}

void FMatrix::Rotation(float x, float y, float z)
{
    float xVal = DirectX::XMConvertToRadians(x);
    float yVal = DirectX::XMConvertToRadians(y);
    float zVal = DirectX::XMConvertToRadians(z);
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xVal, yVal, zVal);

    _m = DirectX::XMMatrixRotationQuaternion(quat);
}

void FMatrix::RotationX(float x)
{
    float xVal = DirectX::XMConvertToRadians(x);
    _m = DirectX::XMMatrixRotationX(xVal);
}

void FMatrix::RotationY(float y)
{
    float yVal = DirectX::XMConvertToRadians(y);
    _m = DirectX::XMMatrixRotationY(yVal);
}

void FMatrix::RotationZ(float z)
{
    float zVal = DirectX::XMConvertToRadians(z);
    _m = DirectX::XMMatrixRotationZ(zVal);
}

void FMatrix::RotationAxis(const FVector3D& axis, float angle)
{
    float angleVal = DirectX::XMConvertToRadians(angle);
    DirectX::XMVECTOR axisVal = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)&axis);
    _m = DirectX::XMMatrixRotationAxis(axisVal, angleVal);
}

void FMatrix::Translation(const FVector3D& v)
{
    _m = DirectX::XMMatrixTranslation(v._x, v._y, v._z);
}

void FMatrix::Translation(float x, float y, float z)
{
    _m = DirectX::XMMatrixTranslation(x, y, z);
}

void FMatrix::Translation(const FVector2D& v)
{
    _m = DirectX::XMMatrixTranslation(v._x, v._y, 0.f);
}

void FMatrix::Translation(float x, float y)
{
    _m = DirectX::XMMatrixTranslation(x, y, 0.f);
}

void FMatrix::ExtractScale(OUT FVector3D& outVal) const
{
    outVal._x = sqrtf(_11 * _11 + _12 * _12 + _13 * _13);
    outVal._y = sqrtf(_21 * _21 + _22 * _22 + _23 * _23);
    outVal._z = sqrtf(_31 * _31 + _32 * _32 + _33 * _33);
}

void FMatrix::ExtractPosition(OUT FVector3D& outVal) const
{
    outVal._x = _41;
    outVal._y = _42;
    outVal._z = _43;
}

void FMatrix::ExtractEuler(OUT FRotator& outVal) const
{
    FVector3D scale; 
    ExtractScale(scale);
    float r13 = _13 / scale._x;
    float r21 = _21 / scale._y;
    float r22 = _22 / scale._y;
    float r23 = _23 / scale._y;
    float r33 = _33 / scale._z;

    if (fabsf(r23) < 0.999f)
    {
        outVal._x = DirectX::XMConvertToDegrees(asinf(-r23));
        outVal._y = DirectX::XMConvertToDegrees(atan2f(r13, r33));
        outVal._z = DirectX::XMConvertToDegrees(atan2f(-r21, r22));
    }
    else
    {
        // [설명] 짐벌락 근처
        // 짐벌락이 발생할 때는 축이 고정 돼서, 다른 축들이 서로 의존적이게 된다.]
        float r11 = _11 / scale._x;
        float r12 = _12 / scale._x;
        outVal._x = DirectX::XMConvertToDegrees(asinf(-r23));
        outVal._y = DirectX::XMConvertToDegrees(atan2f(-r12, r11));
        outVal._z = 0.f;
    }
}
