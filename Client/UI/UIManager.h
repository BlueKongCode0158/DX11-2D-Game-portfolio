#pragma once
#include "../Object/Object.h"

class UIManager : public Object
{
public:
	UIManager();
	virtual ~UIManager();
private:
	int32 _widgetID = 0;
	Weak<class Level> _level;

	std::map<int32, Ptr<class Widget>> _rootWidgets;
	std::vector<int32> _removeWidgets;

	Weak<class Widget> _hoveredWidget;
	Weak<class Widget> _pressdWidget;
private:
	void MouseHovered(const FVector2D& pos);
	void MousePressed(const FVector2D& pos);
	void MouseReleased(const FVector2D& pos);
public:
	bool Init(Ptr<class Level> level);
	void Tick(float deltaTime);
	void Render();
	void MouseEvent();

	void AddRemove(int32 remove);
	void AddToViewport(Ptr<class Widget> widget);
	virtual void Destroy() override;
public:
	Ptr<class Level> GetLevel() const;
public:
	template<typename T>
	Ptr<T> CreateWidget(const std::string& name)
	{
		int32 id = _widgetID++;
		Ptr<T> widget = New<T>();
		if (false == widget->Init(id, name, _level))
		{
			DESTROY(widget);
			return nullptr;
		}
		return widget;
	}
};