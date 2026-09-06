#pragma once
#include "../../SubManager.h"

class MaterialManager : public SubManager
{
public:
	MaterialManager() = default;
	virtual ~MaterialManager() = default;
	MaterialManager(const MaterialManager&) = delete;
	MaterialManager(MaterialManager&&) = delete;
	MaterialManager& operator= (const MaterialManager&) = delete;
	MaterialManager& operator= (MaterialManager&&) = delete;
private:
	std::unordered_map<std::string, Ptr<class Material>> _materials;
public:
	bool Init();
	bool CreateMaterial(const std::string& name, const std::string& pixelShader, eTextureSampleType sampleType);
	Ptr<class Material> CreateMaterialInstance(const std::string& name);
	Ptr<class Material> FindMaterial(const std::string& name);
public:
	virtual void Destroy() override;

};