#include "pch.h"
#include "Player.h"
#include "World/Level.h"
#include "Controller/PlayerController.h"
#include "Component/MovementComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/InputComponent.h"
#include "Component/CameraComponent.h"
#include "Component/AABBCollisionComponent.h"
#include "Component/SphereCollisionComponent.h"
#include "Component/Sound/SoundComponent.h"
#include "Asset/Animation/Animation2DData.h"
#include "Asset/AssetManager.h"
#include "Component/Animation/SpriteComponent.h"
#include "../Component/Light2DComponent.h"
#include "../Component/EdgeCollisionComponent.h"
#include "../Component/FogPlaneComponent.h"
#include "../Component/PlatformMovementComponent.h"

#include "Input/InputSystem.h"
#include "Input/InputContext.h"
#include "Common/LogManager.h"
#include "Core/TimeManager.h"
#include "../Collision/CollisionSystem.h"
#include "../Collision/CollisionProfile.h"

#include "../Component/AI/PlayerStateMachine.h"
#include "../Component/AI/AIBoard.h"


bool Player::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Pawn::Init(id, pos, scale, rot, name);
	_type = eActorType::Player;

	Ptr<Level> level = GetLevel();

	if (!level)
	{
		return false;
	}

	// 메인 카메라가 있는지 탐색
	if (!level->GetMainCamera())
	{
		Ptr<CameraComponent> camera = CreateSceneComponent<CameraComponent>("Cam");
		camera->SetCameraTarget(This<Actor>());
	}

	// 있으면 메인 카메라의 타겟으로 플레이어를 잡는다.
	level->GetMainCamera()->SetCameraTarget(This<Actor>());

	_controller = GetLevel()->SpawnActor<PlayerController>("Player_Controller", pos, scale, rot);
	_controller->SetPawn(This<Player>());

	Ptr<SpriteComponent> effectComp = CreateSceneComponent<SpriteComponent>("EffectAnim");
	effectComp->SetRelativePosition(pos._x, pos._y, pos._z + 10.f);
	effectComp->AttachToComponent(_root);
	effectComp->SetBlendMode(eBlendMode::LIGHTEFFECT);
	effectComp->SetUseAnimationFrameTransform(true);
	effectComp->AddAnimSequenceAuto("tk2d_20600::Double Jump Wings 2");
	effectComp->AddNotify("tk2d_20600::Double Jump Wings 2", effectComp->GetAnimation()->GetCurrentSequneceData()->GetFrameCount() - 1, this, &Player::ExitWingAnim);
	effectComp->SetEnable(false);

	Ptr<SpriteComponent> DasheffectComp = CreateSceneComponent<SpriteComponent>("DashEffectAnim");
	DasheffectComp->AddAnimSequenceAuto("tk2d_20600::Dash Effect");
	DasheffectComp->SetUseAnimationFrameTransform(true);
	DasheffectComp->SetBlendMode(eBlendMode::LIGHTEFFECT);
	DasheffectComp->AttachToComponent(_root);
	DasheffectComp->AddNotify("tk2d_20600::Dash Effect", DasheffectComp->GetAnimation()->GetCurrentSequneceData()->GetFrameCount() - 1, this, &Player::ExitDashEffectAnim);
	DasheffectComp->SetEnable(false);

	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("Anim");
	meshComp->AttachToComponent(_root);
	meshComp->SetUseAnimationFrameTransform(true);


	// ===== 기본 이동 / 공중 =====
	meshComp->AddAnimSequenceAuto("tk2d_20600::Idle");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Run");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Dash");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Airborne");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Fall");
	meshComp->AddAnimSequenceAuto("tk2d_20600::LookDown");
	meshComp->AddAnimSequenceAuto("tk2d_20600::LookUp");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Slash");
	meshComp->AddAnimSequenceAuto("tk2d_20600::SlashAlt");
	meshComp->AddAnimSequenceAuto("tk2d_20600::UpSlash");
	meshComp->AddAnimSequenceAuto("tk2d_20600::DownSlash");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Dash To Idle");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Wall Slash");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Wall Slide");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Walljump");
	meshComp->AddAnimSequenceAuto("tk2d_20600::Double Jump");

	meshComp->AddNotify("tk2d_20600::Dash", 1, this, &Player::DashEffect);

	meshComp->AddNotify("tk2d_20600::Double Jump", 1, this, &Player::DoubleJumpEffect);
	meshComp->AddNotify("tk2d_20600::Double Jump", meshComp->GetAnimation()->FindSequence("tk2d_20600::Double Jump")->GetFrameCount() - 1,
		this, &Player::ExitDoubleJumpAnim);

	meshComp->AddNotify("tk2d_20600::Slash", 6, this, &Player::StopAnimation);
	meshComp->AddNotify("tk2d_20600::SlashAlt", 6, this, &Player::StopAnimation);
	meshComp->AddNotify("tk2d_20600::DownSlash", 6, this, &Player::StopAnimation);
	meshComp->AddNotify("tk2d_20600::UpSlash", 6, this, &Player::StopAnimation);


	Ptr<InputComponent> InputComp = GetController<PlayerController>()->GetInputComponent();
	auto DefaultContext = InputSystem::Instance().FindOrAddInputContext("DEFAULT_CONTEXT");
	auto moveRight = InputSystem::Instance().FindOrAddInputAction("MOVE_RIGHT");
	auto moveLeft = InputSystem::Instance().FindOrAddInputAction("MOVE_LEFT");
	auto moveStop = InputSystem::Instance().FindOrAddInputAction("MOVE_STOP");

	auto lookUP = InputSystem::Instance().FindOrAddInputAction("LOOK_UP");
	auto lookDown = InputSystem::Instance().FindOrAddInputAction("LOOK_DOWN");

	auto Jump = InputSystem::Instance().FindOrAddInputAction("Jump");
	auto Slash = InputSystem::Instance().FindOrAddInputAction("Attack_Slash");
	auto moveDown = InputSystem::Instance().FindOrAddInputAction("MOVE_Down");
	auto moveUp = InputSystem::Instance().FindOrAddInputAction("MOVE_Up");
	auto Dash = InputSystem::Instance().FindOrAddInputAction("Dash");

	// 공중에 있을 때는 바닥 쪽으로 감지하는 충돌체 하나 붙여둔다. -> 혹은 레이캐스팅
	// 레이와 바닥과의 거리가 미세할 때
	// 애니메이션은 idle로 바뀐다.
	// 공중에 머무는 상태에서는 Loop Frame부터 반복한다.
	// 아니면 애니메이션 진행 멈추기
	
	DefaultContext->BindInputAction(moveRight, VK_RIGHT);
	DefaultContext->BindInputAction(moveLeft, VK_LEFT);
	DefaultContext->BindInputAction(lookDown, VK_DOWN);
	DefaultContext->BindInputAction(lookUP, VK_UP);
	DefaultContext->BindInputAction(Jump, 'Z');
	DefaultContext->BindInputAction(Dash, 'X');
	DefaultContext->BindInputAction(Slash, 'C');

	DefaultContext->BindInputAction(moveUp, 'W');
	DefaultContext->BindInputAction(moveDown, 'S');


	InputComp->AddInputContext(DefaultContext->GetName());

	InputComp->BindAction(DefaultContext->GetName(), moveRight->GetName(), INPUT_TYPE::eType::DOWN, this, &Player::MoveRight);
	InputComp->BindAction(DefaultContext->GetName(), moveRight->GetName(), INPUT_TYPE::eType::UP, this, &Player::MoveStop);

	InputComp->BindAction(DefaultContext->GetName(), moveLeft->GetName(), INPUT_TYPE::eType::HOLD, this, &Player::MoveLeft);
	InputComp->BindAction(DefaultContext->GetName(), moveLeft->GetName(), INPUT_TYPE::eType::UP, this, &Player::MoveStop);

	InputComp->BindAction(DefaultContext->GetName(), Jump->GetName(), INPUT_TYPE::eType::DOWN, this, &Player::Jump);
	InputComp->BindAction(DefaultContext->GetName(), Jump->GetName(), INPUT_TYPE::eType::UP, this, &Player::JumpStop);

	InputComp->BindAction(DefaultContext->GetName(), lookDown->GetName(), INPUT_TYPE::eType::HOLD, this, &Player::LookDown);
	InputComp->BindAction(DefaultContext->GetName(), lookUP->GetName(), INPUT_TYPE::eType::HOLD, this, &Player::LookUp);

	InputComp->BindAction(DefaultContext->GetName(), moveUp->GetName(), INPUT_TYPE::eType::UP, this, &Player::MoveStop);
	InputComp->BindAction(DefaultContext->GetName(), moveDown->GetName(), INPUT_TYPE::eType::UP, this, &Player::MoveStop);

	InputComp->BindAction(DefaultContext->GetName(), Slash->GetName(), INPUT_TYPE::eType::DOWN, this, &Player::Attack);
	InputComp->BindAction(DefaultContext->GetName(), Dash->GetName(), INPUT_TYPE::eType::DOWN, this, &Player::Dash);

	_movement = CreateActorComponent<PlatformMovementComponent>("Movement");
	_movement->SetUpdateComponent(_root);
	_movement->SetGravity(-1000.f);
	_movement->SetSpeed(100.f * 5.3f);

	_timerID = TimeManager::Instance().SetTimer(1.f, true, [this]()
		{
			if (this == nullptr)
			{
				return;
			}
		});

	_col = CreateSceneComponent<AABBCollisionComponent>("Collision");
	_col->AddRelativePositionY(-40.f);
	_col->SetBoxSize(40.f, 100.f);
	_col->AttachToComponent(_root);
	_col->SetCollisionProfile("Player");

	Ptr<AABBCollisionComponent> RightattackPointCollider = CreateSceneComponent<AABBCollisionComponent>("RightAttackPoint");
	RightattackPointCollider->AddRelativePosition(50.f, -40.f);
	RightattackPointCollider->SetBoxSize(10.f, 10.f);
	RightattackPointCollider->AttachToComponent(_root);

	Ptr<AABBCollisionComponent> LeftattackPointCollider = CreateSceneComponent<AABBCollisionComponent>("LeftAttackPoint");
	LeftattackPointCollider->AddRelativePosition(-50.f, -40.f);
	LeftattackPointCollider->SetBoxSize(10.f, 10.f);
	LeftattackPointCollider->AttachToComponent(_root);

	Ptr<AABBCollisionComponent> DownAttackPointCollider = CreateSceneComponent<AABBCollisionComponent>("DownAttackPoint");
	DownAttackPointCollider->AddRelativePositionY(-100.f);
	DownAttackPointCollider->SetBoxSize(10.f, 10.f);
	DownAttackPointCollider->AttachToComponent(_root);

	Ptr<AABBCollisionComponent> UpAttackPointCollider = CreateSceneComponent<AABBCollisionComponent>("UpAttackPoint");
	UpAttackPointCollider->AddRelativePositionY(30.f);
	UpAttackPointCollider->SetBoxSize(10.f, 10.f);
	UpAttackPointCollider->AttachToComponent(_root);

	Ptr<AABBCollisionComponent> GroundTrigger = CreateSceneComponent<AABBCollisionComponent>("GroundTrigger");
	GroundTrigger->SetBoxSize(20.f, 40.f);
	GroundTrigger->AttachToComponent(_root);
	GroundTrigger->SetCollisionProfile("Player");
	GroundTrigger->AddRelativePositionY(-80.f);
	GroundTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Player::OnGround);
	GroundTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &Player::OnGround);

	Ptr<AABBCollisionComponent> CeilingHitTrigger = CreateSceneComponent<AABBCollisionComponent>("CeilingTrigger");
	CeilingHitTrigger->SetBoxSize(35.f, 30.f);
	CeilingHitTrigger->AttachToComponent(_root);
	CeilingHitTrigger->SetCollisionProfile("Player");
	CeilingHitTrigger->AddRelativePositionY(20.f);
	CeilingHitTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Player::OnCeiling);
	CeilingHitTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &Player::OnCeiling);

	Ptr<AABBCollisionComponent> LeftWallHitTrigger = CreateSceneComponent<AABBCollisionComponent>("LeftWallTrigger");
	LeftWallHitTrigger->AttachToComponent(_root);
	LeftWallHitTrigger->SetCollisionProfile("Player");
	LeftWallHitTrigger->AddRelativePosition(-16.5f, -30.f);
	LeftWallHitTrigger->SetBoxSize(55.f, 100.f);
	LeftWallHitTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &Player::OnLeftWall);

	Ptr<AABBCollisionComponent> RightWallHitTrigger = CreateSceneComponent<AABBCollisionComponent>("RightWallTrigger");
	RightWallHitTrigger->AttachToComponent(_root);
	RightWallHitTrigger->SetCollisionProfile("Player");
	RightWallHitTrigger->AddRelativePosition(23.5f, -30.f);
	RightWallHitTrigger->SetBoxSize(45.f, 100.f);
	RightWallHitTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &Player::OnRightWall);

	FVector4D tint = FVector4D(0.5f, 0.5f, 0.5f, 1.f);
	_frontLight = CreateSceneComponent<Light2DComponent>("FrontLightEffect");
	_frontLight->SetMesh("white_light_donut::white_light_donut", 3318);
	_frontLight->SetTexture("white_light_donut", 0);
	_frontLight->SetRelativePosition(0.f, 0.f, 0.25f);
	_frontLight->SetRelativeScale(200.f, 200.f, 1.f);
	_frontLight->SetTint(tint);
	_frontLight->AttachToComponent(_root);
	_frontLight->SetRenderLayerName("BackgroundLight");

	_backLight = CreateSceneComponent<Light2DComponent>("BackLightEffect");
	_backLight->SetMesh("light_effect_v02::light_effect_v02", 3023);
	_backLight->SetTexture("light_effect_v02", 0);
	_backLight->SetRelativePosition(0.f, 0.f, 0.25f);
	_backLight->SetRelativeScale(200.f, 200.f, 1.f);
	_backLight->SetTint(tint);
	_backLight->AttachToComponent(_root);
	_backLight->SetRenderLayerName("BackgroundLight");

	GetLevel()->SetPlayer(This<Actor>());
	_root->SetWorldScale(1.f, 1.2f, 1.f);

	SetupStateMachine();
	return true;
}

void Player::Tick(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	// 만약 멀티스레드 쓴다고하면 이렇게 동기화하면 간헐적으로 오류 터질지도 모르겠다.
	board->_isGrounded = _movement->IsGrounded();
	board->_isWallLeft = _movement->IsWallLeft();
	board->_isWallRight = _movement->IsWallRight();
	board->_isFall = !_movement->IsGrounded() && _movement->GetVelocity()._y <= 0.f;
	board->_isWallSliding = !_movement->IsGrounded() && 
		((_movement->IsWallLeft() && board->_isPressLeft) ||
		(_movement->IsWallRight() && board->_isPressRight));

	if (board->_dashCoolTime > 0.f)
	{
		board->_dashCoolTime -= deltaTime;
	}

	if (board->_isGrounded || board->_isWallSliding)
	{
		board->_isDoubleJump = true;
	}

	Pawn::Tick(deltaTime);
}

void Player::Render(float deltaTime)
{
	Pawn::Render(deltaTime);
}

void Player::Collision(float deltaTime)
{
	Pawn::Collision(deltaTime);
}

void Player::Destroy()
{
	Pawn::Destroy();
	TimeManager::Instance().RemoveTimer(_timerID);
}

void Player::SetupStateMachine()
{
	Ptr<AIController> controller = Cast<Controller, AIController>(_controller);
	if (!controller)
	{
		return;
	}
	Ptr<MachineBase> machine = controller->GetAIStateMachine();
	_board = machine->GetAIBoard<PlayerBoard>();

	if (Lock<PlayerBoard>(_board))
	{
		Lock<PlayerBoard>(_board)->_actor = This<Actor>();
	}
}

bool Player::Save(std::ofstream& file)
{
	Pawn::Save(file);
	return true;
}

bool Player::Load(std::ifstream& file)
{
	Pawn::Load(file);
	return true;
}

void Player::MoveRight(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	board->_isPressRight	= true;
	board->_isFacingRight	= true;
	board->_isPressLeft		= false;
}

void Player::MoveLeft(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	board->_isPressRight	= false;
	board->_isFacingRight	= false;
	board->_isPressLeft		= true;
}

void Player::MoveStop(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	board->_isPressRight	= false;
	board->_isPressLeft		= false;
}

void Player::Attack(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	board->_isAttack = true;

}

void Player::Jump(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}
	board->_isJump = true;
}

void Player::JumpStop(float deltaTime)
{
	_movement->StopJump();
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}
	board->_isJump = false;
}

void Player::LookDown(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}
	board->_isPressDown = true;
}

void Player::LookUp(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}
	board->_isPressUp = true;
}

void Player::Dash(float deltaTime)
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	if (board->_dashCoolTime > 0.f)
	{
		return;
	}

	board->_isDash = true;
}

Ptr<class PlatformMovementComponent> Player::GetMovement()
{
	return _movement;
}

bool Player::LookRight()
{
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return false;
	}
	return board->_isFacingRight;
}


void Player::ShakeCameraTest(float deltaTime)
{
	GetLevel()->GetMainCamera()->AddTrauma(0.3f);
}

void Player::OnGround(Weak<class CollisionComponent> comp)
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
			if (fabs(dir._x) > fabs(dir._y))
			{
				FVector3D pos = curComp->GetWorldPosition();
				FVector3D curPos = _root->GetWorldPosition();
				_root->SetWorldPosition(curPos._x, pos._y + 100.f, curPos._z);
				_movement->SetGrounded(true);
			}
		}
	}
	else if (curComp->GetShape() == eCollisonShape::AABB)
	{

		_movement->SetGrounded(true);
	}
}

void Player::OnCeiling(Weak<class CollisionComponent> comp)
{
	Ptr<CollisionComponent> curComp = Lock<CollisionComponent>(comp);
	if (curComp->GetProfile()->GetChannel() != eCollisionChannel::COLLISION_CHANNEL_ENVIRONMENT)
	{
		return;
	}

	if (_movement->GetVelocity()._y > 0.f)
	{
		_movement->ZeroVerticalVelocity();
	}
	_movement->SetCeiling(true);
}

void Player::OnLeftWall(Weak<class CollisionComponent> comp)
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
			if (fabs(dir._y) > fabs(dir._x))
			{
				FVector3D pos = curComp->GetWorldPosition();
				FVector3D curPos = _root->GetWorldPosition();
				_root->SetWorldPosition(pos._x + 35.f, curPos._y, curPos._z);
				_movement->Stop();
				_movement->SetWallLeft(true);
			}
		}
	}
	else if (curComp->GetShape() == eCollisonShape::AABB)
	{
		Ptr<AABBCollisionComponent> aabb = Cast<CollisionComponent, AABBCollisionComponent>(curComp);
		if (aabb)
		{
			if (_movement->GetMoveAxis() == -FVector3D::Axis_X)
			{
				_movement->Stop();
			}
			_movement->SetWallLeft(true);
		}
	}

}

void Player::OnRightWall(Weak<class CollisionComponent> comp)
{
	Ptr<CollisionComponent> curComp = Lock<CollisionComponent>(comp);
	if (curComp->GetProfile()->GetChannel() != eCollisionChannel::COLLISION_CHANNEL_ENVIRONMENT)
	{
		return;
	}
	Ptr<PlayerBoard> board = Lock<PlayerBoard>(_board);

	if (curComp->GetShape() == eCollisonShape::EDGE)
	{
		Ptr<EdgeCollisionComponent> edge = Cast<CollisionComponent, EdgeCollisionComponent>(curComp);
		if (edge)
		{
			FEDGE2D line = edge->GetLine();

			FVector2D dir = line._StartPoint - line._EndPoint;
			dir.Normalize();
			if (fabs(dir._y) > fabs(dir._x))
			{
				FVector3D pos = curComp->GetWorldPosition();
				FVector3D curPos = _root->GetWorldPosition();
				_root->SetWorldPosition(pos._x - 35.f, curPos._y, curPos._z);
				_movement->Stop();
				_movement->SetWallRight(true);
			}
		}
	}
	else if (curComp->GetShape() == eCollisonShape::AABB)
	{
		Ptr<AABBCollisionComponent> aabb = Cast<CollisionComponent, AABBCollisionComponent>(curComp);
		if (aabb)
		{
			if (_movement->GetMoveAxis() == FVector3D::Axis_X)
			{
				_movement->Stop();
			}
			_movement->SetWallRight(true);
		}
	}


}

void Player::ExitWingAnim()
{
	Ptr<SpriteComponent> effectComp = FindSceneComponent<SpriteComponent>("EffectAnim");
	if (!effectComp)
	{
		return;
	}
	effectComp->SetEnable(false);
}

void Player::ExitDashEffectAnim()
{
	Ptr<SpriteComponent> effectComp = FindSceneComponent<SpriteComponent>("DashEffectAnim");
	if (!effectComp)
	{
		return;
	}
	effectComp->SetEnable(false);
}

void Player::DoubleJumpEffect()
{
	Ptr<SpriteComponent> effectComp = FindSceneComponent<SpriteComponent>("EffectAnim");
	if (!effectComp)
	{
		return;
	}
	effectComp->SetEnable(true);
	effectComp->SetWorldPosition(_root->GetWorldPosition());
	effectComp->ChangeAnimation("tk2d_20600::Double Jump Wings 2");
}

void Player::DashEffect()
{
	Ptr<SpriteComponent> effectComp = FindSceneComponent<SpriteComponent>("DashEffectAnim");
	if (!effectComp)
	{
		return;
	}
	auto board = Lock<PlayerBoard>(_board);
	if (!board)
	{
		return;
	}

	effectComp->SetEnable(true);
	effectComp->ChangeAnimation("tk2d_20600::Dash Effect");
}

void Player::ExitDoubleJumpAnim()
{
	Ptr<SpriteComponent> Anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!Anim)
	{
		return;
	}
	Anim->ChangeAnimation("tk2d_20600::Fall");
}

void Player::StopAnimation()
{
	Ptr<SpriteComponent> Anim = FindSceneComponent<SpriteComponent>("Anim");
	if (!Anim)
	{
		return;
	}
	Anim->GetAnimation()->SetPlayCurAnimation(false);
}
