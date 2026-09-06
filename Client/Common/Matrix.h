#pragma once

// 행렬
// 게임에서 행렬을 많이 써요.
// 행렬 왜 씀?? 여러 변환(크기, 이동, 회전)을 하나의 연산으로 묶어서 처리하려고
// 동일한 기능을 하는 함수들은 (동일한 함수들)
// 특정한 곳( 그 중 하나 )에 구현을 몰아두는게 낫다.


/*
[4 * 4]
1 2 3 4
5 6 7 8
1 2 3 4
5 6 7 8

단위 행렬 : 대각선이 1, 나머지는 0
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1
(항등행렬)

행렬의 곱 ---> 교환법칙 X

전치행렬
행렬의 열과 행을 뒤집는걸 전치행렬

크기 행렬
x 0 0 0
0 y 0 0
0 0 z 0
0 0 0 1

X축 회전 행렬
1   0   0   0
0  cos -sin 0
0  sin  cos 0
0   0   0   1

Y축 회전 행렬
cos  0   sin 0
0    1   0   0
-sin 0   cos 0
0    0   0   1

Z축 회전 행렬
cos -sin    0   0
sin cos     0   0
0   0       1   0
0   0       0   1

최종 회전 행렬 : x축 회전행렬 * Y축 회전 행렬 * Z 회전 행렬

이동 행렬
1   0   0   0
0   1   0   0
0   0   1   0
x   y   z   1

월드행렬
--> 크기 * 자전 * 이동 * 부모행렬
    --> 부모 행렬은 상속받은 클래스가 아닌 나에게 영향을 줄 수 있는 다른 객체

오일러 각
3D 회전을 x, y, z 축 가각의 회전의 값으로 표현
짐벌락이 발생할 수 있다.
짐벌락 : 한 축이 90도 회전하게 되면 나머지 두 축이 겹치게 되서
회전이 제대로 되지 않는걸 의미합니다.

쿼터니언(사원수)
숫자가 4개
x축 회전 : pitch
y축 회전 : yaw
z축 회전 : roll

x, y, z, 회전양

*/
_declspec(align(16)) union FMatrix
{
    DirectX::XMMATRIX _m;
    struct
    {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    };
    FVector4D _v[4] = {};

    FMatrix();
    FMatrix(const FMatrix& m);
    FMatrix(FMatrix&& m) noexcept;
    FMatrix(const DirectX::XMMATRIX& m);
    FMatrix(DirectX::XMMATRIX&& m) noexcept;
    FMatrix(const FVector4D v[4]);

    FVector4D& operator[] (int32 index);

    const FMatrix& operator = (const FMatrix& m);
    const FMatrix& operator = (const DirectX::XMMATRIX& m);
    const FMatrix& operator = (const FVector4D v[4]);

    FMatrix operator * (const FMatrix& m) const;
    FMatrix operator * (const DirectX::XMMATRIX& m) const;

    void Indentity();
    void Transpose();
    void Inverse();

    void Scaling(const FVector3D& v);
    void Scaling(float x, float y, float z);
    void Scaling(const FVector2D& v);
    void Scaling(float x, float y);

    void Rotation(const FVector3D& v);
    void Rotation(float x, float y, float z);
    void RotationX(float x);
    void RotationY(float y);
    void RotationZ(float z);
    void RotationAxis(const FVector3D& axis, float angle /* z */);

    void Translation(const FVector3D& v);
    void Translation(float x, float y, float z);
    void Translation(const FVector2D& v);
    void Translation(float x, float y);

    void ExtractScale(OUT FVector3D& outVal) const;
    void ExtractPosition(OUT FVector3D& outVal) const;
    void ExtractEuler(OUT FRotator& outVal) const;
};

