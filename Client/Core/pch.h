#pragma once

#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <set>

//미리컴파일된 헤더
//자주 쓰는 헤더들을 미리 컴파일해서 캐싱해두고 빌드시간을 단축
//여기에 들어가는 헤더들은 잘 바뀌지 않는것들만!
//바꾸면 또 다시 전부 다 빌드해야하기 때문에 오히려 빌드시간이 늘어날수도 있어요..
//ex) Item.h, Monster.h <---이런것들은 넣으면 안됩니다.

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;


//DirectX11 - 3D 랜더링
//ID3D11Device, ID3D11DeviceContext...
#include <d3d11.h>

//HLSL 컴파일러
//*.hlsl 파일 런타임 컴파일러
#include <d3dcompiler.h>

//directWrite3 - 텍스트 랜더링
#include <dwrite_3.h>

//direct2d - 2d 랜더링
#include <d2d1.h>

//.lib, .dll
#pragma comment(lib, "d3d11.lib")       //dx11 구현체
#pragma comment(lib, "d3dcompiler.lib")//셰이더 컴파일러 구현체
#pragma comment(lib, "dxguid.lib")      //dx guid 모음
#pragma comment(lib, "dwrite.lib")      //directWrite 구현체
#pragma comment(lib, "d2d1.lib")        //Direct2D 구현체

//dx11 객체들의 스마트 포인터
#include "wrl.h"
using namespace Microsoft::WRL;

#include <Common/Vector2D.h>
#include <Common/Vector3D.h>
#include <Common/Vector4D.h>
#include <Common/Matrix.h>

#include "Common/Info.h"

