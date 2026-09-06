#pragma once
#include "Widget.h"

class Image : public Widget
{
public:
	Image();
	virtual ~Image();
protected:
	FUIBrush _brush;
public:
	void SetTexture(const std::string& name);
	void SetTexture(const std::string& name, const std::wstring& fileName);
	void SetTexture(Ptr<class Texture> texture);

	void SetOpacity(float op);
	void SetTint(float r, float g, float b, float a);

	void SetBrushAnimEnable(bool enable);
	void AddBrushFrame(const FVector2D& start, const FVector2D& size);
	void AddBrushFrame(float startX, float startY, float sizeX, float sizeY);
	void SetCurrentFrame(int32 frame);
public:
	virtual void SetSize(const FVector2D& size);
public:
	virtual void SetAnimationPlayTime(float time);
public:
	virtual void SetBrushAnimation(const std::string& name);
	virtual void SetBrushAnimationPlay(bool play);
	virtual void SetBrushAnimationLoop(bool loop);
public:
	virtual bool Init(int32 id, const std::string& name, Weak<class Level> level);
	virtual void Tick(float deltaTime);
	virtual void Render();
	virtual void Render(const FVector3D& pos);
	virtual void Destroy() override;
};