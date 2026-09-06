#pragma once
#include "../Level.h"

class RoomTempleScene : public Level
{
public:
	RoomTempleScene();
	virtual ~RoomTempleScene();
	RoomTempleScene(const RoomTempleScene& other) = delete;
	RoomTempleScene(RoomTempleScene&& other) = delete;
	RoomTempleScene& operator = (const RoomTempleScene& other) = delete;
	RoomTempleScene& operator = (RoomTempleScene&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};