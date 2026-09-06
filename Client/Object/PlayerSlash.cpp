#include "pch.h"
#include "PlayerSlash.h"

#include "../Component/Animation/SpriteComponent.h"
#include "../Component/AABBCollisionComponent.h"
#include "../Object/Asset/AssetManager.h"

#include "../World/Level.h"

bool PlayerSlash::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
    Actor::Init(id, pos, scale, rot, name);
    //_root->SetWorldScale(1.f, 1.f);

    _meshComp = CreateSceneComponent<SpriteComponent>("Anim");
    _meshComp->AddAnimSequenceAuto("tk2d_20600::SlashEffect");
    _meshComp->AddAnimSequenceAuto("tk2d_20600::SlashEffectAlt");
    _meshComp->AddAnimSequenceAuto("tk2d_20600::UpSlashEffect");
    _meshComp->AddAnimSequenceAuto("tk2d_20600::DownSlashEffect");
    _meshComp->SetRenderLayerName("Effect");
    
    _meshComp->SetUseAnimationFrameTransform(true);
    _meshComp->AttachToComponent(_root);

    _collisionComp = CreateSceneComponent<AABBCollisionComponent>("Attack");
    _collisionComp->SetCollisionProfile("PlayerAttack");
    _collisionComp->AttachToComponent(_root);
    _collisionComp->SetBoxSize(150.f, 150.f);

    return true;
}

void PlayerSlash::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);

    if (!_meshComp->GetAnimation()->GetCurSequence()->GetPlay())
    {
        GetLevel()->RemoveActor(_id);
    }
}

void PlayerSlash::Render(float deltaTime)
{
    Actor::Render(deltaTime);
}

void PlayerSlash::Collision(float deltaTime)
{
    Actor::Collision(deltaTime);
}

void PlayerSlash::Destroy()
{
    Actor::Destroy();
}

void PlayerSlash::SetAttack(PLAYERSLASHTYPE eAttackType)
{
    switch (eAttackType)
    {
    case ATTACK:
        _meshComp->ChangeAnimation("tk2d_20600::SlashEffect");
        break;
    case DOUBLE_ATTACK:
        _meshComp->ChangeAnimation("tk2d_20600::SlashEffectAlt");
        break;
    case UP_ATTACK:
        _meshComp->ChangeAnimation("tk2d_20600::UpSlashEffect");
        break;
    case DOWN_ATTACK:
        _meshComp->ChangeAnimation("tk2d_20600::DownSlashEffect");
        break;
    default:
        break;
    }
}

void PlayerSlash::SetDir(bool isRight)
{
    _meshComp->SetAnimFlip(isRight);
    _collisionComp->AddRelativePositionX(isRight ? 70.f : -70.f);
}
