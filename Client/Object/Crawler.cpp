#include "pch.h"
#include "Crawler.h"

#include "../World/Level.h"
#include "../Component/Animation/SpriteComponent.h"
#include "../Component/AABBCollisionComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/EdgeCollisionComponent.h"
#include "../Component/CollisionComponent.h"
#include "../Collision/CollisionProfile.h"
#include "../Component/CameraComponent.h"
#include "../Component/Animation/Animation2D.h"
#include "../Object/Asset/Animation/Animation2DSequence.h"


Crawler::Crawler()
{
}

Crawler::~Crawler()
{
}

bool Crawler::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Pawn::Init(id, pos, scale, rot, name);


	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("Anim");
	meshComp->AttachToComponent(_root);
	meshComp->SetUseAnimationFrameTransform(true);

	meshComp->AddAnimSequenceAuto("tk2d_1113::Death Air");
	meshComp->AddAnimSequenceAuto("tk2d_1113::Death Land");
	meshComp->AddAnimSequenceAuto("tk2d_1113::idle");
	meshComp->AddAnimSequenceAuto("tk2d_1113::turn");
	meshComp->AddAnimSequenceAuto("tk2d_1113::walk");

	Ptr<Animation2DSequence> turnAnim = meshComp->GetAnimation()->FindSequence("tk2d_1113::turn");
	meshComp->AddNotify("tk2d_1113::turn", turnAnim->GetFrameCount() - 1, this, &Crawler::ExitTurnAnim);

	Ptr<Animation2DSequence> deadAnim = meshComp->GetAnimation()->FindSequence("tk2d_1113::Death Land");
	meshComp->AddNotify("tk2d_1113::Death Land", deadAnim->GetFrameCount() - 1, this, &Crawler::DeadMonster);

	Ptr<Animation2DSequence> deadAirAnim = meshComp->GetAnimation()->FindSequence("tk2d_1113::Death Air");
	meshComp->AddNotify("tk2d_1113::Death Air", deadAirAnim->GetFrameCount() - 1, this, &Crawler::ExitDeadAirAnim);

	_movement = CreateActorComponent<MovementComponent>("Movement");
	_movement->SetSpeed(64.f * 3.f);
	_movement->SetUpdateComponent(_root);

	Ptr<AABBCollisionComponent> LeftWallCollision = CreateSceneComponent<AABBCollisionComponent>("LeftWallTrigger");
	LeftWallCollision->SetBoxSize(37.f, 70.f);
	LeftWallCollision->AddRelativePositionX(-70.f);
	LeftWallCollision->AddRelativePositionY(-38.f);
	LeftWallCollision->AttachToComponent(_root);
	LeftWallCollision->SetCollisionProfile("Monster");
	LeftWallCollision->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Crawler::BlockLeftWall);

	Ptr<AABBCollisionComponent> RightWallCollision = CreateSceneComponent<AABBCollisionComponent>("RightWallTrigger");
	RightWallCollision->SetBoxSize(37.f, 70.f);
	RightWallCollision->AddRelativePositionX(70.f);
	RightWallCollision->AddRelativePositionY(-38.f);
	RightWallCollision->AttachToComponent(_root);
	RightWallCollision->SetCollisionProfile("Monster");
	RightWallCollision->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Crawler::BlockRightWall);

	_col = CreateSceneComponent<AABBCollisionComponent>("AttackPoint");
	_col->SetBoxSize(100.f, 60.f);
	_col->AttachToComponent(_root);
	_col->SetCollisionProfile("Monster");
	_col->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Crawler::AttackedMonster);

	meshComp->ChangeAnimation("tk2d_1113::walk");
	return true;
}

void Crawler::Tick(float deltaTime)
{
	Pawn::Tick(deltaTime);
	_movement->AddMoveAxis(FVector3D::Axis_X * _moveDir);
}

void Crawler::Collision(float deltaTime)
{
	Pawn::Collision(deltaTime);
}

void Crawler::Render(float deltaTime)
{
	Pawn::Render(deltaTime);
}

void Crawler::Destroy()
{
	Pawn::Destroy();
}

void Crawler::BlockLeftWall(Weak<class CollisionComponent> comp)
{
	Ptr<CollisionComponent> curComp = Lock<CollisionComponent>(comp);
	if (curComp->GetProfile()->GetChannel() != eCollisionChannel::COLLISION_CHANNEL_ENVIRONMENT)
	{
		return;
	}

	if (curComp->GetShape() == eCollisonShape::EDGE)
	{
		Ptr<EdgeCollisionComponent> edge = Cast<CollisionComponent, EdgeCollisionComponent>(curComp);
		if (edge)
		{
			FEDGE2D line = edge->GetLine();
			FVector2D dir = line._StartPoint - line._EndPoint;
			dir.Normalize();

			// 벽 판정 
			if (fabs(dir._x) < fabs(dir._y))
			{
				_moveDir *= -1.f;
			}

		}
	}
	else if (curComp->GetShape() == eCollisonShape::AABB)
	{
		_moveDir *= -1.f;

	}

	Ptr<SpriteComponent> anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!anim)
	{
		return;
	}
	anim->ChangeAnimation("tk2d_1113::turn");
}

void Crawler::BlockRightWall(Weak<class CollisionComponent> comp)
{
	Ptr<CollisionComponent> curComp = Lock<CollisionComponent>(comp);
	if (curComp->GetProfile()->GetChannel() != eCollisionChannel::COLLISION_CHANNEL_ENVIRONMENT)
	{
		return;
	}

	Ptr<AABBCollisionComponent> rightTrigger = FindSceneComponent<AABBCollisionComponent>("RightWallTrigger");

	if (curComp->GetShape() == eCollisonShape::EDGE)
	{
		Ptr<EdgeCollisionComponent> edge = Cast<CollisionComponent, EdgeCollisionComponent>(curComp);
		if (edge)
		{
			FEDGE2D line = edge->GetLine();
			FVector2D dir = line._StartPoint - line._EndPoint;
			dir.Normalize();

			// 벽 판정 
			if (fabs(dir._x) < fabs(dir._y))
			{
				_moveDir *= -1.f;
			}
		}
	}
	else if (curComp->GetShape() == eCollisonShape::AABB)
	{
		_moveDir *= -1.f;
	}

	Ptr<SpriteComponent> anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!anim)
	{
		return;
	}
	anim->ChangeAnimation("tk2d_1113::turn");
}

void Crawler::AttackedMonster(Weak<class CollisionComponent> comp)
{
	Ptr<class CollisionComponent> curComp = Lock<CollisionComponent>(comp);
	if (!curComp)
	{
		return;
	}

	Ptr<SpriteComponent> anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!anim)
	{
		return;
	}
	if (curComp->GetProfile()->GetChannel() == eCollisionChannel::COLLISION_CHANNEL_PLAYERATTACK)
	{
		if (_Hp > 1)
		{
			_Hp--;
		}
		else
		{
			_moveDir = 0;
			anim->ChangeAnimation("tk2d_1113::Death Air");
		}
	}
}

void Crawler::ExitTurnAnim()
{
	Ptr<SpriteComponent> anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!anim)
	{
		return;
	}

	anim->ChangeAnimation("tk2d_1113::walk");
	anim->SetAnimFlip(_moveDir > 0.f ? true : false);
}

void Crawler::ExitDeadAirAnim()
{
	Ptr<SpriteComponent> anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!anim)
	{
		return;
	}

	anim->ChangeAnimation("tk2d_1113::Death Land");
}

void Crawler::DeadMonster()
{
	GetLevel()->RemoveActor(_id);
	Ptr<class Level> level = GetLevel();

	if (!level)
	{
		return;
	}

	Ptr<class CameraComponent> camera = level->GetMainCamera();
	if (!camera)
	{
		return;
	}

	//camera->AddTrauma(0.4f);
}
