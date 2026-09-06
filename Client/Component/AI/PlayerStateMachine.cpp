#include "pch.h"
#include "PlayerStateMachine.h"
#include "../../Object/Actor.h"
#include "../../Object/Player.h"
#include "../Animation/SpriteComponent.h"
#include "../PlatformMovementComponent.h"
#include "../AABBCollisionComponent.h"

#include "../../World/Level.h"
#include "../../World/World.h"
#include "../../Core/GameEngine.h"

#include "../../Object/PlayerSlash.h"

#include "AITransition.h"
#include "AIBoard.h"

void PlayerStateMachine::Init(Ptr<class AIComponent> owner)
{
	MachineBase::Init(owner);
	_interval = 0.f;

	auto board = CreateAIBoard<PlayerBoard>();

	auto idle = CreateAIState<AIState>("IDLE");
	auto move = CreateAIState<AIState>("MOVE");
	auto jump = CreateAIState<AIState>("JUMP");
	auto fall = CreateAIState<AIState>("FALL");
	auto wallSlide = CreateAIState<AIState>("WALL_SLIDE");
	auto wall = CreateAIState<AIState>("WALL");

	auto attack = CreateAIState<AIState>("ATTACK_SLASH");
	auto attackAlt = CreateAIState<AIState>("ATTACK_SLASH_ALT");
	
	auto attackDown = CreateAIState<AIState>("ATTACK_DOWN");
	auto attackUp = CreateAIState<AIState>("ATTACK_UP");

	auto doublejump = CreateAIState<AIState>("DOUBLE_JUMP");
	auto dash = CreateAIState<AIState>("DASH");

	//auto dash = CreateAIState<AIState>("DASH");

	//// 벤치에 닿아있는 상태일 때 -> idle or Run -> 휴식 문구가 떠있을 때
	//auto rest = CreateAIState<AIState>("REST");

	idle->AddEnterCallback(this, &PlayerStateMachine::OnIdleEnter);
	
	move->AddEnterCallback(this, &PlayerStateMachine::OnMoveEnter);
	move->AddTickCallback(this, &PlayerStateMachine::OnMoveTick);
	move->AddExitCallback(this, &PlayerStateMachine::OnMoveExit);

	jump->AddEnterCallback(this, &PlayerStateMachine::OnJumpEnter);
	jump->AddTickCallback(this, &PlayerStateMachine::OnJumpTick);
	//jump->AddExitCallback(this, &PlayerStateMachine::OnJumpExit);

	fall->AddEnterCallback(this, &PlayerStateMachine::OnFallEnter);
	fall->AddTickCallback(this, &PlayerStateMachine::OnFallTick);

	wallSlide->AddEnterCallback(this, &PlayerStateMachine::OnWallSlideEnter);
	wallSlide->AddTickCallback(this, &PlayerStateMachine::OnWallSlideTick);
	wallSlide->AddExitCallback(this, &PlayerStateMachine::OnWallSlideExit);

	wall->AddEnterCallback(this, &PlayerStateMachine::OnWallEnter);
	wall->AddTickCallback(this, &PlayerStateMachine::OnWallTick);
	wall->AddExitCallback(this, &PlayerStateMachine::OnWallExit);

	attack->AddEnterCallback(this, &PlayerStateMachine::OnAttackEnter);
	attack->AddTickCallback(this, &PlayerStateMachine::OnAttackTick);

	attackAlt->AddEnterCallback(this, &PlayerStateMachine::OnAttackAltEnter);
	attackAlt->AddTickCallback(this, &PlayerStateMachine::OnAttackAltTick);
	
	attackDown->AddEnterCallback(this, &PlayerStateMachine::OnAttackDownEnter);
	attackDown->AddTickCallback(this, &PlayerStateMachine::OnAttackDownTick);

	attackUp->AddEnterCallback(this, &PlayerStateMachine::OnAttackUpEnter);
	attackUp->AddTickCallback(this, &PlayerStateMachine::OnAttackUpTick);

	doublejump->AddEnterCallback(this, &PlayerStateMachine::OnDoubleJumpEnter);
	doublejump->AddTickCallback(this, &PlayerStateMachine::OnDoubleJumpTick);

	dash->AddEnterCallback(this, &PlayerStateMachine::OnDashEnter);
	dash->AddTickCallback(this, &PlayerStateMachine::OnDashTick);
	dash->AddExitCallback(this, &PlayerStateMachine::OnDashExit);

	CreateAITransition<AITransition>("IDLE", "IDLE_TO_MOVE", "MOVE", eTransitionRule::OR);
	idle->AddCondition("IDLE_TO_MOVE", this, &PlayerStateMachine::CondCanMove);

	CreateAITransition<AITransition>("IDLE", "IDLE_TO_JUMP", "JUMP", eTransitionRule::OR);
	idle->AddCondition("IDLE_TO_JUMP", this, &PlayerStateMachine::CondCanJump);

	CreateAITransition<AITransition>("IDLE", "IDLE_TO_FALL", "FALL", eTransitionRule::AND);
	idle->AddCondition("IDLE_TO_FALL", this, &PlayerStateMachine::CondIsFalling);

	CreateAITransition<AITransition>("IDLE", "IDLE_TO_ATTACK", "ATTACK_SLASH", eTransitionRule::AND);
	idle->AddCondition("IDLE_TO_ATTACK", this, &PlayerStateMachine::CondCanAttack);

	CreateAITransition<AITransition>("IDLE", "IDLE_TO_DASH", "DASH", eTransitionRule::AND);
	idle->AddCondition("IDLE_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	// MOVE에서 나가는 전환
	CreateAITransition<AITransition>("MOVE", "MOVE_TO_IDLE", "IDLE", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_IDLE", this, &PlayerStateMachine::CondCanIdle);

	CreateAITransition<AITransition>("MOVE", "MOVE_TO_JUMP", "JUMP", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_JUMP", this, &PlayerStateMachine::CondCanJump);

	CreateAITransition<AITransition>("MOVE", "MOVE_TO_FALL", "FALL", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_FALL", this, &PlayerStateMachine::CondIsFalling);

	CreateAITransition<AITransition>("MOVE", "MOVE_TO_ATTACK", "ATTACK_SLASH", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_ATTACK", this, &PlayerStateMachine::CondCanAttack);

	CreateAITransition<AITransition>("MOVE", "MOVE_TO_DASH", "DASH", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	CreateAITransition<AITransition>("MOVE", "MOVE_TO_WALL", "WALL", eTransitionRule::AND);
	move->AddCondition("MOVE_TO_WALL", this, &PlayerStateMachine::CondIsWall);

	// JUMP에서 나가는 전환
	CreateAITransition<AITransition>("JUMP", "JUMP_TO_FALL", "FALL", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_FALL", this, &PlayerStateMachine::CondIsFalling);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_WALLSLIDE", "WALL_SLIDE", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_WALLSLIDE", this, &PlayerStateMachine::CondCanWallSlide);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_ATTACKDOWN", "ATTACK_DOWN", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_ATTACKDOWN", this, &PlayerStateMachine::CondCanAttackDown);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_ATTACKUP", "ATTACK_UP", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_ATTACKUP", this, &PlayerStateMachine::CondCanAttackUp);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_ATTACK", "ATTACK_SLASH", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_ATTACK", this, &PlayerStateMachine::CondCanAttack);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_DOUBLEJUMP", "DOUBLE_JUMP", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_DOUBLEJUMP", this, &PlayerStateMachine::CondCanDoubleJump);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_IDLE", "IDLE", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	CreateAITransition<AITransition>("JUMP", "JUMP_TO_DASH", "DASH", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	CreateAITransition<AITransition>("Jump", "JUMP_TO_WALL", "WALL", eTransitionRule::AND);
	jump->AddCondition("JUMP_TO_WALL", this, &PlayerStateMachine::CondIsWall);


	// FALL에서 나가는 전환
	CreateAITransition<AITransition>("FALL", "FALL_TO_IDLE", "IDLE", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	CreateAITransition<AITransition>("FALL", "FALL_TO_WALLSLIDE", "WALL_SLIDE", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_WALLSLIDE", this, &PlayerStateMachine::CondCanWallSlide);

	CreateAITransition<AITransition>("FALL", "FALL_TO_ATTACKDOWN", "ATTACK_DOWN", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_ATTACKDOWN", this, &PlayerStateMachine::CondCanAttackDown);

	CreateAITransition<AITransition>("FALL", "FALL_TO_ATTACKUP", "ATTACK_UP", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_ATTACKUP", this, &PlayerStateMachine::CondCanAttackUp);

	CreateAITransition<AITransition>("FALL", "FALL_TO_ATTACK", "ATTACK_SLASH", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_ATTACK", this, &PlayerStateMachine::CondCanAttack);

	CreateAITransition<AITransition>("FALL", "FALL_TO_DOUBLEJUMP", "DOUBLE_JUMP", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_DOUBLEJUMP", this, &PlayerStateMachine::CondCanDoubleJump);

	CreateAITransition<AITransition>("FALL", "FALL_TO_DASH", "DASH", eTransitionRule::AND);
	fall->AddCondition("FALL_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	// WALL_SLIDE에서 나가는 전환
	CreateAITransition<AITransition>("WALL_SLIDE", "WALLSLIDE_TO_FALL", "FALL", eTransitionRule::AND);
	wallSlide->AddCondition("WALLSLIDE_TO_FALL", this, &PlayerStateMachine::CondNotWallSlide);

	CreateAITransition<AITransition>("WALL_SLIDE", "WALLSLIDE_TO_JUMP", "JUMP", eTransitionRule::AND);
	wallSlide->AddCondition("WALLSLIDE_TO_JUMP", this, &PlayerStateMachine::CondCanJump);

	CreateAITransition<AITransition>("WALL_SLIDE", "WALLSLIDE_TO_IDLE", "IDLE", eTransitionRule::OR);
	wallSlide->AddCondition("WALLSLIDE_TO_IDLE", this, &PlayerStateMachine::CondCanIdle);
	wallSlide->AddCondition("WALLSLIDE_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	// ATTACK_SLASH에서 나가는 전환
	CreateAITransition<AITransition>("ATTACK_SLASH", "ATTACK_TO_ALT", "ATTACK_SLASH_ALT", eTransitionRule::AND);
	attack->AddCondition("ATTACK_TO_ALT", this, &PlayerStateMachine::CondCanAttack);

	CreateAITransition<AITransition>("ATTACK_SLASH", "ATTACK_TO_IDLE", "IDLE", eTransitionRule::AND);
	attack->AddCondition("ATTACK_TO_IDLE", this, &PlayerStateMachine::CondAttackDone);

	CreateAITransition<AITransition>("ATTACK_SLASH", "ATTACK_TO_DASH", "DASH", eTransitionRule::AND);
	attack->AddCondition("ATTACK_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	// ATTACK_SLASH_ALT에서 나가는 전환
	CreateAITransition<AITransition>("ATTACK_SLASH_ALT", "ALT_TO_IDLE", "IDLE", eTransitionRule::AND);
	attackAlt->AddCondition("ALT_TO_IDLE", this, &PlayerStateMachine::CondAttackDone);

	CreateAITransition<AITransition>("ATTACK_SLASH_ALT", "ALT_TO_DASH", "DASH", eTransitionRule::AND);
	attackAlt->AddCondition("ALT_TO_DASH", this, &PlayerStateMachine::CondCanDash);

	// ATTACKDOWN 에서 나가는 전환
	CreateAITransition<AITransition>("ATTACK_DOWN", "ATTACKDOWN_TO_IDLE", "IDLE", eTransitionRule::OR);
	attackDown->AddCondition("ATTACKDOWN_TO_IDLE", this, &PlayerStateMachine::CondAttackDone);
	attackDown->AddCondition("ATTACKDOWN_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	CreateAITransition<AITransition>("ATTACK_DOWN", "ATTACKDOWN_TO_FALL", "FALL", eTransitionRule::AND);
	attackDown->AddCondition("ATTACKDOWN_TO_FALL", this, &PlayerStateMachine::CondIsFalling);
	attackDown->AddCondition("ATTACKDOWN_TO_FALL", this, &PlayerStateMachine::CondAttackDone);

	// ATTACKUP 에서 나가는 전환
	CreateAITransition<AITransition>("ATTACK_UP", "ATTACKUP_TO_IDLE", "IDLE", eTransitionRule::OR);
	attackUp->AddCondition("ATTACKUP_TO_IDLE", this, &PlayerStateMachine::CondAttackDone);
	attackUp->AddCondition("ATTACKUP_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	CreateAITransition<AITransition>("ATTACK_UP", "ATTACKUP_TO_FALL", "FALL", eTransitionRule::AND);
	attackUp->AddCondition("ATTACKUP_TO_FALL", this, &PlayerStateMachine::CondIsFalling);
	attackUp->AddCondition("ATTACKUP_TO_FALL", this, &PlayerStateMachine::CondAttackDone);

	// DoubleJump 에서 나가는 전환 
	CreateAITransition<AITransition>("DOUBLE_JUMP", "DOUBLE_JUMP_TO_FALL", "FALL", eTransitionRule::AND);
	doublejump->AddCondition("DOUBLE_JUMP_TO_FALL", this, &PlayerStateMachine::CondIsFalling);

	CreateAITransition<AITransition>("DOUBLE_JUMP", "DOUBLE_JUMP_TO_IDLE", "IDLE", eTransitionRule::AND);
	doublejump->AddCondition("DOUBLE_JUMP_TO_IDLE", this, &PlayerStateMachine::CondIsGrounded);

	CreateAITransition<AITransition>("DOUBLE_JUMP", "DOUBLE_JUMP_TO_WALLSLIDE", "WALL_SLIDE", eTransitionRule::AND);
	doublejump->AddCondition("DOUBLE_JUMP_TO_WALLSLIDE", this, &PlayerStateMachine::CondCanWallSlide);

	CreateAITransition<AITransition>("DOUBLE_JUMP", "DOUBLE_JUMP_TO_WALL", "WALL", eTransitionRule::AND);
	doublejump->AddCondition("DOUBLE_JUMP_TO_WALL", this, &PlayerStateMachine::CondIsWall);


	// DASH
	CreateAITransition<AITransition>("DASH", "DASH_TO_IDLE", "IDLE", eTransitionRule::OR);
	dash->AddCondition("DASH_TO_IDLE", this, &PlayerStateMachine::CondIsFinishDash);

	CreateAITransition<AITransition>("DASH", "DASH_TO_FALL", "FALL", eTransitionRule::AND);
	dash->AddCondition("DASH_TO_FALL", this, &PlayerStateMachine::CondIsFinishDash);
	dash->AddCondition("DASH_TO_FALL", this, &PlayerStateMachine::CondIsFalling);

	CreateAITransition<AITransition>("DASH", "DASH_TO_MOVE", "MOVE", eTransitionRule::AND);
	dash->AddCondition("DASH_TO_MOVE", this, &PlayerStateMachine::CondIsFinishDash);
	dash->AddCondition("DASH_TO_MOVE", this, &PlayerStateMachine::CondIsGrounded);
	dash->AddCondition("DASH_TO_MOVE", this, &PlayerStateMachine::CondCanMove);

	CreateAITransition<AITransition>("WALL", "WALL_TO_WALLSLIDE", "WALL_SLIDE", eTransitionRule::AND);
	wall->AddCondition("WALL_TO_WALLSLIDE", this, &PlayerStateMachine::CondCanWallSlide);

	CreateAITransition<AITransition>("WALL", "WALL_TO_IDLE", "IDLE", eTransitionRule::OR);
	wall->AddCondition("WALL_TO_IDLE", this, &PlayerStateMachine::CondCanIdle);

	CreateAITransition<AITransition>("WALL", "WALL_TO_MOVE", "MOVE", eTransitionRule::AND);
	wall->AddCondition("WALL_TO_MOVE", this, &PlayerStateMachine::CondIsNotWall);
	wall->AddCondition("WALL_TO_MOVE", this, &PlayerStateMachine::CondCanMove);

}

void PlayerStateMachine::Destroy()
{
	MachineBase::Destroy();
}

Ptr<class PlayerBoard> PlayerStateMachine::GetBoard()
{
	return Cast<AIBoard, PlayerBoard>(_board);
}

Ptr<class Player> PlayerStateMachine::GetPlayer()
{
	auto board = GetBoard();
	if (!board)
	{
		return nullptr;
	}
	return Cast<Actor, Player>(Lock<Actor>(board->_actor));
}

void PlayerStateMachine::OnIdleEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!player) return;
	player->FindSceneComponent<SpriteComponent>("Anim") ->ChangeAnimation("tk2d_20600::Idle");
	player->GetMovement()->SetSpeedRange(1.f);
}

void PlayerStateMachine::OnMoveEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!player) return;
	player->GetMovement()->SetSpeedRange(1.f);
	player->FindSceneComponent<SpriteComponent>("Anim") ->ChangeAnimation("tk2d_20600::Run");
	board->_isGrounded = true;
}

void PlayerStateMachine::OnJumpEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isJump = false;   // 순간 입력 소비

	player->GetMovement()->StartJump();
	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::Airborne");

	player->FindSceneComponent<AABBCollisionComponent>("GroundTrigger")->SetEnable(false);
}

void PlayerStateMachine::OnFallEnter(float deltaTime)
{
	auto player = GetPlayer();
	if (!player) return;

	if (player->FindSceneComponent<SpriteComponent>("Anim")->GetAnimation()->GetCurrentSequneceData()->GetName() != "tk2d_20600::Airborne")
	{
		player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::Fall");
	}
	player->FindSceneComponent<AABBCollisionComponent>("GroundTrigger")->SetEnable(true);
}

void PlayerStateMachine::OnWallSlideEnter(float deltaTime)
{
	auto player = GetPlayer();
	if (!player) return;
	// WallSlide 전용 애니메이션이 추가되면 여기서 변경
	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");

	// 플립: 붙어있는 벽 방향 바라보기
	auto board = GetBoard();
	if (board)
	{
		board->_isFacingRight = board->_isWallRight;
		anim->ChangeAnimation("tk2d_20600::Wall Slide");
		anim->SetAnimFlip(board->_isFacingRight);
	}
	player->GetMovement()->Stop();
	board->_isWallSliding = true;
}

void PlayerStateMachine::OnAttackEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isAttack = false;   // 순간 입력 소비
	board->_isAttackDone = false;

	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::Slash");
	if (board->_isFacingRight)
	{
		Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("RightAttackPoint");
		FVector3D position = attackPoint->GetWorldPosition();
		Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
			position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
		slash->SetAttack(PLAYERSLASHTYPE::ATTACK);
		slash->SetDir(board->_isFacingRight);
	}
	else
	{
		Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("LeftAttackPoint");
		FVector3D position = attackPoint->GetWorldPosition();
		Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
			position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
		slash->SetAttack(PLAYERSLASHTYPE::ATTACK);
		slash->SetDir(board->_isFacingRight);
	}
}

void PlayerStateMachine::OnAttackAltEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isAttack = false;
	board->_isAttackDone = false;

	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::SlashAlt");

	if (board->_isFacingRight)
	{
		Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("RightAttackPoint");
		FVector3D position = attackPoint->GetWorldPosition();
		Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
			position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
		slash->SetAttack(PLAYERSLASHTYPE::DOUBLE_ATTACK);
		slash->SetDir(board->_isFacingRight);
	}
	else
	{
		Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("LeftAttackPoint");
		FVector3D position = attackPoint->GetWorldPosition();
		Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
			position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
		slash->SetAttack(PLAYERSLASHTYPE::DOUBLE_ATTACK);
		slash->SetDir(board->_isFacingRight);
	}

}

void PlayerStateMachine::OnAttackDownEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isAttack = false;
	board->_isAttackDone = false;
	board->_isPressDown = false;

	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::DownSlash");

	Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("DownAttackPoint");
	FVector3D position = attackPoint->GetWorldPosition();
	Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
		position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
	slash->SetAttack(PLAYERSLASHTYPE::DOWN_ATTACK);
}

void PlayerStateMachine::OnAttackUpEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isAttack = false;
	board->_isAttackDone = false;
	board->_isPressUp = false;

	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::UpSlash");

	Ptr<AABBCollisionComponent> attackPoint = player->FindSceneComponent<AABBCollisionComponent>("UpAttackPoint");
	FVector3D position = attackPoint->GetWorldPosition();
	Ptr<PlayerSlash> slash = player->GetLevel()->SpawnActor<PlayerSlash>("playerSlash",
		position, FVector3D(1.f, 1.f, 1.f), FVector3D(0.f, 0.f, 0.f));
	slash->SetAttack(PLAYERSLASHTYPE::UP_ATTACK);
}

void PlayerStateMachine::OnDoubleJumpEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;
	
	board->_isDoubleJump = false;
	board->_isJump = false;

	player->FindSceneComponent<SpriteComponent>("Anim")->ChangeAnimation("tk2d_20600::Double Jump");
	player->GetMovement()->StartDoubleJump();
}

void PlayerStateMachine::OnDashEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	board->_isDash = false;
	board->_dashTime = 0.f;
	board->_dashCoolTime = board->DASH_COOLDOWN;

	if (board->_isPressRight)
	{
		board->_dashDir = board->_isWallRight ? -1 : 1;
	}
	else if (board->_isPressLeft)
	{
		board->_dashDir = board->_isWallLeft ? 1 : -1;
	}
	else
	{
		board->_dashDir = (board->_isFacingRight && !board->_isWallRight) ? 1 : -1;
	}

	board->_isFacingRight = (board->_dashDir > 0);

	auto move = player->GetMovement();
	move->SetGravityEnabled(false);
	move->SetVelocity(FVector3D::Zero);

	move->SetSpeedRange(board->DASH_SPEED_RANGE);

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	if (anim)
	{
		anim->ChangeAnimation("tk2d_20600::Dash");
		anim->SetAnimFlip(board->_isFacingRight);
	}
}

void PlayerStateMachine::OnWallEnter(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	player->GetMovement()->Stop();
}

void PlayerStateMachine::OnIdleTick(float deltaTime)
{
}

void PlayerStateMachine::OnMoveTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	auto mov = player->GetMovement();

	if (board->_isPressRight)
	{
		mov->SetMoveAxis(FVector3D::Axis_X);
		anim->SetAnimFlip(true);
		board->_isFacingRight = true;
	}
	else if (board->_isPressLeft)
	{
		mov->SetMoveAxis(-FVector3D::Axis_X);
		anim->SetAnimFlip(false);
		board->_isFacingRight = false;
	}
	else
	{
		mov->SetMoveAxis(FVector3D::Zero);
	}
}

void PlayerStateMachine::OnJumpTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	auto mov = player->GetMovement();

	if (board->_isPressRight)
	{
		mov->SetMoveAxis(FVector3D::Axis_X);
		anim->SetAnimFlip(true);
		board->_isFacingRight = true;
	}
	else if (board->_isPressLeft)
	{
		mov->SetMoveAxis(-FVector3D::Axis_X);
		anim->SetAnimFlip(false);
		board->_isFacingRight = false;
	}
	else
	{
		mov->SetMoveAxis(FVector3D::Zero);
	}
}

void PlayerStateMachine::OnFallTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	auto mov = player->GetMovement();

	if (board->_isPressRight)
	{
		mov->SetMoveAxis(FVector3D::Axis_X);
		anim->SetAnimFlip(true);
		board->_isFacingRight = true;
	}
	else if (board->_isPressLeft)
	{
		mov->SetMoveAxis(-FVector3D::Axis_X);
		anim->SetAnimFlip(false);
		board->_isFacingRight = false;
	}
	else
	{
		mov->SetMoveAxis(FVector3D::Zero);
	}
}

void PlayerStateMachine::OnWallSlideTick(float deltaTime)
{
	auto player = GetPlayer();
	if (!player)
	{
		return;
	}

	auto move = player->GetMovement();
	constexpr float WALL_SLIDE_MAX_FALL = -100.f;

	if (move->GetVelocity()._y < WALL_SLIDE_MAX_FALL)
	{
		FVector3D velocity = move->GetVelocity();
		velocity._y = WALL_SLIDE_MAX_FALL;
		move->SetVelocity(velocity);
	}
}

void PlayerStateMachine::OnAttackTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player)
	{
		return;
	}

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	if (!anim->GetAnimation()->IsAnimationEnd())
	{
		board->_isAttackDone = true;
	}
}

void PlayerStateMachine::OnAttackAltTick(float deltaTime)
{
	OnAttackTick(deltaTime);
}

void PlayerStateMachine::OnAttackDownTick(float deltaTime)
{
	OnAttackTick(deltaTime);
}

void PlayerStateMachine::OnAttackUpTick(float deltaTime)
{
	OnAttackTick(deltaTime);
}

void PlayerStateMachine::OnDoubleJumpTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board || !player) return;

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	auto effectAnim = player->FindSceneComponent<SpriteComponent>("EffectAnim");

	auto mov = player->GetMovement();

	if (board->_isPressRight)
	{
		mov->SetMoveAxis(FVector3D::Axis_X);
		anim->SetAnimFlip(true);
		effectAnim->SetAnimFlip(true);
		board->_isFacingRight = true;
	}
	else if (board->_isPressLeft)
	{
		mov->SetMoveAxis(-FVector3D::Axis_X);
		anim->SetAnimFlip(false);
		effectAnim->SetAnimFlip(false);
		board->_isFacingRight = false;
	}
	else
	{
		mov->SetMoveAxis(FVector3D::Zero);
	}
}

void PlayerStateMachine::OnDashTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board)
	{
		return;
	}

	Ptr<SpriteComponent> effectComp = player->FindSceneComponent<SpriteComponent>("DashEffectAnim");
	Ptr<AABBCollisionComponent> rightPoint = player->FindSceneComponent<AABBCollisionComponent>("RightAttackPoint");
	Ptr<AABBCollisionComponent> leftPoint = player->FindSceneComponent<AABBCollisionComponent>("LeftAttackPoint");

	auto anim = player->FindSceneComponent<SpriteComponent>("Anim");
	board->_dashTime += deltaTime;
	player->GetMovement()->SetMoveAxis(FVector3D::Axis_X * static_cast<float>(board->_dashDir));

	if (board->_isFacingRight)
	{
		effectComp->SetAnimPivot(FVector3D(-200.f, -30.f, 0.f));
		effectComp->SetAnimFlip(false);
	}
	else
	{
		effectComp->SetAnimPivot(FVector3D(200.f, -30.f, 0.f));
		effectComp->SetAnimFlip(true);
	}

}

void PlayerStateMachine::OnWallTick(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!board)
	{
		return;
	}

	if (board->_isWallRight)
	{
		if (player->GetMovement()->GetMoveAxis() == FVector3D::Axis_X)
		{
			player->GetMovement()->Stop();
		}
		else
		{
			board->_isWallRight = false;
		}
	}
	else if (board->_isWallLeft)
	{
		if (player->GetMovement()->GetMoveAxis() == -FVector3D::Axis_X)
		{
			player->GetMovement()->Stop();
		}
		else
		{
			board->_isWallLeft = false;
		}
	}
}

void PlayerStateMachine::OnIdleExit(float deltaTime)
{
}

void PlayerStateMachine::OnMoveExit(float deltaTime)
{
	auto player = GetPlayer();
	if (!player) return;
	player->GetMovement()->Stop();
}

void PlayerStateMachine::OnJumpExit(float deltaTime)
{
}

void PlayerStateMachine::OnFallExit(float deltaTime)
{
}

void PlayerStateMachine::OnWallSlideExit(float deltaTime)
{
	auto board = GetBoard();
	auto player = GetPlayer();
	if (!player || !board) 
		return;
	player->GetMovement()->SetSpeedRange(1.f);
	board->_isWallSliding = false;
}

void PlayerStateMachine::OnAttackExit(float deltaTime)
{
}

void PlayerStateMachine::OnAttackAltExit(float deltaTime)
{
}

void PlayerStateMachine::OnAttackDownExit(float deltaTime)
{
}

void PlayerStateMachine::OnAttackUpExit(float deltaTime)
{
}

void PlayerStateMachine::OnDoubleJumpExit(float deltaTime)
{
}

void PlayerStateMachine::OnDashExit(float deltaTime)
{
	auto board = GetBoard();
	if (!board)
	{
		return;
	}
	
	auto move = GetPlayer()->GetMovement();
	move->SetGravityEnabled(true);
	move->SetSpeedRange(1.f);
	move->Stop();
}

void PlayerStateMachine::OnWallExit(float deltaTime)
{
}

bool PlayerStateMachine::IsRun()
{
	auto board = Cast<AIBoard, PlayerBoard>(_board);
	if (!board)
	{
		return false;
	}
	return false;
}

bool PlayerStateMachine::CondCanMove()
{
	auto board = GetBoard();
	return board && (board->_isPressRight || board->_isPressLeft);
}

bool PlayerStateMachine::CondCanIdle()
{
	auto board = GetBoard();
	return board && !board->_isPressRight && !board->_isPressLeft;
}

bool PlayerStateMachine::CondCanJump()
{
	auto board = GetBoard();
	return board && board->_isJump;
}

bool PlayerStateMachine::CondIsFalling()
{
	auto board = GetBoard();
	return board && board->_isFall;
}

bool PlayerStateMachine::CondIsGrounded()
{
	auto board = GetBoard();
	return board && board->_isGrounded;
}

bool PlayerStateMachine::CondCanWallSlide()
{
	auto board = GetBoard();
	return board && board->_isWallSliding;
}

bool PlayerStateMachine::CondNotWallSlide()
{
	auto board = GetBoard();
	return board && !board->_isWallSliding;
}

bool PlayerStateMachine::CondCanAttack()
{
	auto board = GetBoard();
	return board && board->_isAttack;
}

bool PlayerStateMachine::CondAttackDone()
{
	auto board = GetBoard();
	return board && board->_isAttackDone;
}

bool PlayerStateMachine::CondCanAttackDown()
{
	auto board = GetBoard();
	return board && board->_isPressDown && board->_isAttack;
}

bool PlayerStateMachine::CondCanAttackUp()
{
	auto board = GetBoard();
	return board && board->_isPressUp && board->_isAttack;
}

bool PlayerStateMachine::CondCanDoubleJump()
{
	auto board = GetBoard();
	return board && board->_isDoubleJump && board->_isJump && !board->_isGrounded;
}

bool PlayerStateMachine::CondCanDash()
{
	auto board = GetBoard();
	return board && board->_isDash && (board->_dashCoolTime <= 0.f);
}

bool PlayerStateMachine::CondIsFinishDash()
{
	auto board = GetBoard();
	return board && (board->_dashTime >= 0.3f);
}

bool PlayerStateMachine::CondIsWall()
{
	auto board = GetBoard();
	return board && ((board->_isWallLeft && board->_isPressLeft) || (board->_isWallRight && board->_isPressRight));
}

bool PlayerStateMachine::CondIsNotWall()
{
	auto board = GetBoard();
	return board && ((board->_isWallLeft && board->_isPressRight) && (board->_isWallRight && board->_isPressLeft));
}
