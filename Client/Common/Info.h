#pragma once

#define PLAYER_ZONE 0
#define PLAYER_DEADZONE 0.7f
#define UNIT 64
class Utility
{
public:
    template<typename T>
    static T Clamp(const T& value, const T& minValue, const T& maxValue)
    {
        return max(minValue, min(value, maxValue));
    }

    static FVector3D Lerp(const FVector3D& dest, const FVector3D& src, float t)
    {
        return FVector3D(
            dest._x + (src._x - dest._x) * t,
            dest._y + (src._y - dest._y) * t,
            dest._z + (src._z - dest._z) * t
        );
    }

    static float Noise1D(float x, uint32 seed = 0)
    {
        auto hash = [](uint32 n) -> float 
            {
                n = (n << 13) ^ n;
                return 1.f - ((n * (n * n * 15731u + 789211u) + 1376312589u) & 0x7fffffffu) / 1073741824.0f;
            };

        int     num = (int)floorf(x);
        float   f = x - num;

        uint32 base = static_cast<uint32>(num) + seed * 1013904223u;
        float a = hash(base);
        float b = hash(base);
        float u = f * f * (3.f - 2.f * f);
        return a + (b - a) * u;
    }
};

class StringUtility
{
public:
    static std::wstring UTF8ToWstring(const std::string& str)
    {
        // 문자열이 비어있는 경우 L""을 리턴한다.
        if (str.empty())
        {
            return L"";
        }

        int sizeNeeded = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            str.data(),
            static_cast<int>(str.size()),
            nullptr,
            0
        );

        // MultiByteToWideChar 호출 실패시 L""을 리턴한다.
        if (sizeNeeded <= 0)
        {
            return L"";
        }

        std::wstring result(sizeNeeded, 0);

        int converted = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            str.data(),
            static_cast<int>(str.size()),
            result.data(),
            sizeNeeded
        );
        return result;
    }

    static std::string WstringToUTF8(const std::wstring& wstr)
    {
        if (wstr.size())
        {
            return "";
        }

        int sizeNeeded = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.data(),
            static_cast<int>(wstr.size()),
            nullptr,
            0,
            nullptr,
            nullptr
        );
        
        if (sizeNeeded <= 0)
        {
            return "";
        }

        std::string result(sizeNeeded, 0);
        int converted = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.data(),
            static_cast<int>(wstr.size()),
            result.data(),
            sizeNeeded,
            nullptr,
            nullptr
        );
        return result;
    }
};

struct FResolution
{
    uint32 _width = 0;
    uint32 _height = 0;
};

struct FVertexBuffer
{
    ComPtr<ID3D11Buffer> _buffer = nullptr;    // 인덱스 버퍼를 담는 buffer
    int32 _size = 0;            //  정점 하나의 크기
    int32 _count = 0;           //  정점 개수
    std::vector<byte> _data;    
};

struct FIndexBuffer
{
    ComPtr<ID3D11Buffer> _buffer = nullptr;    // 인덱스 버퍼를 담는 buffer
    int32 _size = 0;                            // 인덱스 한개의 크기
    int32 _count = 0;                           // 인덱스 총 개수
    DXGI_FORMAT _fmt = DXGI_FORMAT_UNKNOWN;     // 데이터 해석 방식
    std::vector<byte> _data;
};

struct FVertexColor
{
    FVertexColor() {}
    FVertexColor(const FVector3D& pos, const FVector4D& color) : _pos(pos), _color(color) {}
    FVertexColor(float x, float y, float z, float r, float g, float b, float a)
    {
        _pos._x = x;
        _pos._y = y;
        _pos._z = z;

        _color._x = r;
        _color._y = g;
        _color._z = b;
        _color._w = a;
    }

    FVector3D _pos;
    FVector4D _color;
};

// 위치, 크기, 회전을 가지고 있다.
struct  FTransform
{
    FVector3D _scale;
    FVector3D _position;
    FRotator _rotation;
};

struct FTransformMatirx
{
    FMatrix _scale;
    FMatrix _rotation;
    FMatrix _translate;
    FMatrix _world;
};

struct FAABB2D
{
    FVector2D _min;
    FVector2D _max;
};

struct FOBB2D
{
    FVector2D _center;
    FVector2D _axis[2];
    FVector2D _halfSize;
};

struct FEDGE2D
{
    FVector2D _StartPoint;  // 시작점
    FVector2D _EndPoint;    // 끝점
};

struct FEDGEBOX2D
{
    FEDGE2D _bottomEdge;
    FEDGE2D _topEdge;
    FEDGE2D _leftEdge;
    FEDGE2D _rightEdge;
};

struct FCollisionPenetration
{
    bool        bColliding = false;
    FVector2D   normal;
    float       depth = 0.f;
};

struct FVertexTexture
{
    FVector3D _pos;
    FVector2D _uv;
    FVertexTexture(float x, float y, float z, float u, float v)
        :_pos(x, y, z), _uv(u, v)
    {

    }
};


struct FAnimationFrame
{
    FVector2D _start;   // atlasX
    FVector2D _size;    // atlasY
    FVector2D _draw;    // drawing size
    FVector2D _offset = FVector2D( 0.f, 0.f );  // offset
    FVector2D _canvasSize;
    FVector2D _pivotTrimmed;
    FVector2D _pivotCanvasSize;
    bool _facingFlip = false;
    bool _rotated90 = false;
    int32 _textureIndex = 0;    // 이 프레임이 사용하는 아틀라스 (Animation2DData::_frameTextures 인덱스)
};

struct FLocalBound2D
{
    FVector2D _Min = { FLT_MAX,FLT_MAX };
    FVector2D _Max = { -FLT_MAX,-FLT_MAX };

    void Reset()
    {
        _Min = { FLT_MAX, FLT_MAX };
        _Max = { -FLT_MAX,-FLT_MAX };
    }

    void Expand(float x, float y)
    {
        if (!std::isfinite(x) || !std::isfinite(y))
        {
            return;
        }

        _Min._x = min(_Min._x, x);
        _Min._y = min(_Min._y, y);
        _Max._x = max(_Max._x, x);
        _Max._y = max(_Max._y, y);
    }

    bool IsValid() const
    {
        return std::isfinite(_Min._x) && std::isfinite(_Min._y)
            && std::isfinite(_Max._x) && std::isfinite(_Max._y)
            && _Min._x <= _Max._x
            && _Min._y <= _Max._y;
    }
};

enum class eObjectType
{
    OBJECT_TYPE_TEST_OBJ,
    END
};

enum class eAssetType
{
    MESH,
    TEXTURE,
    MATERIAL,
    ANIMATION,
    SOUND,
    FONT,
    END
};

namespace SHADER_TYPE
{
    enum eType
    {
        VERTEX = 0x1,
        PIXEL = 0x2,
        GRAPHIC = VERTEX | PIXEL
    };
}

namespace INPUT_TYPE
{
    enum eType
    {
        DOWN,   // 키를 눌렀을 때
        HOLD,   // 키를 누르고 있을 때
        UP,     // 키를 뗐을 때
        END
    };
}

namespace MOUSE_BUTTON_TYPE
{
    enum eType
    {
        LBUTTON,
        RBUTTON,
        WHEEL,
        END
    };
}

enum class eInputSystemType
{
    DINPUT,
    WINDOW,
    END
};

namespace AXIS_TYPE
{
    enum Type
    {
        X,
        Y,
        Z,
        END
    };
}

// 각 충돌체가 어떤 충돌체랑 충돌을 할건지 혹은 무시할건지
// 대해서 관리해놓은 타입
// PLAYER , MONSTER, ITEM
// PLAYER - MONSTER --> 충돌
// PLAYER - ITEM --> 충돌
// MONSTER - PLAYER
// ITEM - PLAYER
// ITEM !- MONSTER -> 충돌 x
enum eCollisionChannel : unsigned char
{
    COLLISION_CHANNEL_PLAYER,
    COLLISION_CHANNEL_PLAYERATTACK,
    COLLISION_CHANNEL_MONSTER,
    COLLISION_CHANNEL_MONSTERATTACK,
    COLLISION_CHANNEL_ITEM,
    COLLISION_CHANNEL_BOSS,
    COLLISION_CHANNEL_ENVIRONMENT,
    COLLISION_CHANNEL_CAMERAZONE,
    COLLISION_CHANNEL_GATE,
    COLLISION_CHANNEL_PLAYERDECTECTION,
    COLLISION_CHANNEL_END
};

enum eCollisionResponse : unsigned char
{
    COLLISION_RESPONSE_IGNORE,
    COLLISION_RESPONSE_BLOCK,
    COLLISION_RESPONSE_OVERRAP,
    COLLISION_RESPONSE_END
};

enum class eCollisonShape : unsigned char
{
    AABB,
    OBB,
    SPHERE,
    EDGE,
    END
};

enum eCollisionState
{
    COLLISION_STATE_RELEASE,
    COLLISION_STATE_BLOCK,
    COLLISION_STATE_OVERLAP,
    END
};

// 샘플러
// 텍스쳐를 그릴 때 해당 텍스쳐의 픽셀을 어떻게 가져올 것인지 정하는 방법
enum eTextureSampleType
{
    TEXTURE_SAMPLE_POINT,           // 포인터 샘플러 -> uv좌표에서 가장 가까운 픽셀을 1개를 그대로 가져옴 (보간 없음)
    TEXTURE_SAMPLE_LINEAR,          // 선형 샘플러 -> uv좌표 주변 4개의 픽셀을 거리에 따라 가중 평균, 부드럽게 보간
    TEXTURE_SAMPLE_ANISOTROPIC,     // 이방선 샘플러 -> 카메라 비스듬히 볼 때 , 품질 유지, 경사진 표면에서 선형 샘플러보다 훨씬 무거움
    TEXTURE_SAMPLE_END
};

enum eRenderState
{
    RENDER_STATE_BLEND,
    RENDER_STATE_DEPTHSTENCIL,
    RENDER_STATE_END
};

enum class eAnimTextureType
{
    SPRITE,
    FRAME,
    MESHSPRITE,
    ANIMATION_TEXTURE_END
};

namespace AI_EVENT_STATE
{
    enum Type
    {
        ENTER,
        TICK,
        EXIT,
        END
    };
}

enum class eTransitionRule
{
    AND,
    OR,
    END
};

namespace UI_BUTTON_STATE
{
    enum Type
    {
        ENABLE,
        DISABLE,
        HOVERED,
        PRESSED,
        CLICKED,
        END
    };
}

namespace UI_BUTTON_EVENT_STATE
{
    enum Type
    {
        HOVERED,
        UNHOVERED,
        PRESSED,
        CLICKED,
        END
    };
}

enum class BUTTONTYPE
{
    NUM,
    END
};

enum class eBlendMode : unsigned char
{
    DEFAULT,
    LIGHT,
    FOG,
    SCENEBLEND,
    LIGHTEFFECT,
    END
};

enum class eActorType
{
    Actor,
    Pawn,
    Player,
    Monster,
    Prob,
    Fog,
    Light,
    CameraZone,
    PreCam,
    End
};

enum class eComponentType
{
    // scene
    SCENE,
    STATICMESH,
    MESH,
    COLLISION,
    AABB,
    OBB,
    EDGE,
    SPHERE,
    SPRITE,
    CAMERA,
    SOUND,
    TILE,
    WIDGET,
    FOGPLANE,
    PARALLAX,
    PARTICLE,

    // actor
    ACTOR = 500,
    MOVEMENT,
    AI,
    INPUT,
    END
};

enum eLevelType : unsigned char
{
    MAIN,
    TUTORIAL,
    TOWN,
    CROSSROAD_01,
    CROSSROAD_02,
    CROSSROAD_10,
    CROSSROAD_11,
    TEMPLE,
    TEMPLESTREET,
    GRIMMTENT,
    LEVEL_END
};