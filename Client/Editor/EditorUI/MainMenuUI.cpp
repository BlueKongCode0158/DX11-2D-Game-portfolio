#include "pch.h"
#include "MainMenuUI.h"           
#include "../EditEngine.h"
#include "../../Core/GameEngine.h"

#include "OutLinerUI.h"
#include "InspectorUI.h"
#include "ViewportUI.h"
#include "SaveUI.h"
#include "LoadUI.h"

#include "../../World/World.h"
#include "../../World/Level.h"

#include "../../Object/Monster.h"

MainMenuUI::MainMenuUI()
{
}

MainMenuUI::~MainMenuUI()
{
}

bool MainMenuUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	return true;
}

void MainMenuUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);
	if (ImGui::BeginMainMenuBar())
	{
		File();
		View();
		Actor();
		Level();
		ImGui::EndMainMenuBar();
	}
}

void MainMenuUI::Destroy()
{
	EditorUI::Destroy();
}

void MainMenuUI::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New"))
		{
		}
		if (ImGui::MenuItem("Save"))
		{			
			Ptr<SaveUI> save = EditEngine::Instance().FindEditorUI<SaveUI>("Save");
			if (save)
			{
				save->SetEnable(true);
			}
		}
		if (ImGui::MenuItem("Load"))
		{
			Ptr<LoadUI> load = EditEngine::Instance().FindEditorUI<LoadUI>("Load");
			if (load)
			{
				load->SetEnable(true);
			}
		}
		ImGui::EndMenu();
	}
}
	

void MainMenuUI::View()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("OutLiner"))
		{
			Ptr<OutLinerUI> outliner = EditEngine::Instance().FindEditorUI<OutLinerUI>("OutLiner");
			if (outliner)
			{
				outliner->SetEnable(true);
			}
		}

		if (ImGui::MenuItem("Inspector"))
		{
			Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");
			if (inspector)
			{
				inspector->SetEnable(true);
			}
		}

		if (ImGui::MenuItem("Viewport"))
		{
			Ptr<ViewportUI> viewport = EditEngine::Instance().FindEditorUI<ViewportUI>("Viewport");
			if (viewport)
			{
				viewport->SetEnable(true);
			}
		}
		ImGui::EndMenu();
	}

}

void MainMenuUI::Actor()
{
	if (ImGui::BeginMenu("Actor"))
	{
		if (ImGui::MenuItem("Create"))
		{

		}
		ImGui::EndMenu();
	}
}

void MainMenuUI::Level()
{
	if (ImGui::BeginMenu("Level"))
	{
		static const std::pair<const char*, eLevelType> levels[] =
		{
			{ "Main",          MAIN },
			{ "Tutorial",      TUTORIAL },
			{ "Town",          TOWN },
			{ "Crossroad_01",  CROSSROAD_01 },
			{ "Crossroad_02",  CROSSROAD_02 },
			{ "Crossroad_10",  CROSSROAD_10},
			{ "Crossroad_11",  CROSSROAD_11},
			{ "Temple",        TEMPLE },
			{ "TempleStreet",  TEMPLESTREET },
			{ "GrimmTent",     GRIMMTENT }
		};

		for (const auto& [name, type] : levels)
		{
			if (ImGui::MenuItem(name))
			{
				Ptr<World> world = GET_WORLD;
				if (world)
				{
					world->SetNextLevelType(type);
				}
			}
		}
		ImGui::EndMenu();
	}
}

void MainMenuUI::UI()
{
	if (ImGui::MenuItem("UI"))
	{

	}
}

void MainMenuUI::Collision()
{
	if (ImGui::MenuItem("Collision"))
	{
		// todo : collision
	}
}
