#include "pch.h"
#include "Prob.h"

#include "../Component/Animation/SpriteComponent.h"
#include "../Component/SceneComponent.h"
#include "../Component/ParallaxLayerComponent.h"
#include "../Shader/SpriteShader.h"

#include "../Object/Asset/Mesh/Mesh.h"

Prob::Prob()
{
}

Prob::~Prob()
{
}

bool Prob::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);
	_type = eActorType::Prob;

	_meshComp = CreateProbMesh();
	if (nullptr == _meshComp)
	{
		return false;
	}

	if (pos._z >= -PLAYER_DEADZONE && pos._z <= PLAYER_DEADZONE) // 플레이 존에 있는 오브젝트 
	{
		_meshComp->SetRenderLayerName("Default");
	}
	else if (pos._z >= PLAYER_DEADZONE) // 플레이 zone 보다 뒤에 있을 때 오브젝트
	{
		_meshComp->SetRenderLayerName("Background");
	}
	else // 플레이 zone 보다 앞에 있을 때 오브젝트
	{
		_meshComp->SetRenderLayerName("Foreground");
	}
	return true;
}

void Prob::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Prob::Render(float deltaTime)
{
	Actor::Render(deltaTime);
}

void Prob::Collision(float deltaTime)
{
	Actor::Collision(deltaTime);
}

void Prob::Destroy()
{
	Actor::Destroy();
}

void Prob::DrawInspector()
{
	Actor::DrawInspector();
	ParallaxLayerComponent::DrawGlobalInspector();
}

bool Prob::Save(std::ofstream& file)
{
	Actor::Save(file);
	return false;
}

bool Prob::Load(std::ifstream& file)
{
	Actor::Load(file);
	return false;
}

void Prob::SetMesh(const std::string& mesh, const int32 pathID)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetMesh(mesh, pathID);
}

void Prob::SetTexture(const std::string& AtlasName, int index)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetTexture(AtlasName, index);
}

void Prob::SetLayerName(const std::string& LayerName)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetRenderLayerName(LayerName);
}

void Prob::SetTint(const FVector4D& tint)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetTint(tint);

}

void Prob::SetShader(const std::string& name)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetShader(name);
}

void Prob::SetBlendMode(eBlendMode eMode)
{
	if (nullptr == _meshComp)
	{
		_meshComp = CreateProbMesh();
	}
	_meshComp->SetBlendMode(eMode);
}

void Prob::SetParallaxParentPosition(const FVector3D& parentPos)
{
	if (nullptr == _parallaxComp)
	{
		return;
	}
	_parallaxComp->SetParentWorldPosition(parentPos);
}

Ptr<SpriteComponent> Prob::CreateProbMesh()
{
	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("ProbMesh");
	meshComp->SetShader("SpriteMeshShader");
	meshComp->SetWorldScale(64.f, 64.f, 1.f);
	meshComp->AttachToComponent(_root);

	Ptr<ParallaxLayerComponent> parallaxComp = CreateSceneComponent<ParallaxLayerComponent>("Parallax");
	parallaxComp->AttachToComponent(_root);
	parallaxComp->SetTarget(meshComp);
	_parallaxComp = parallaxComp;

	return meshComp;
}

