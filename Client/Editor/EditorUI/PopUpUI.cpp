#include "pch.h"
#include "PopUpUI.h"

PopUPUI::PopUPUI()
{
}

PopUPUI::~PopUPUI()
{
}

bool PopUPUI::Init(const std::string& name)
{
    EditorUI::Init(name);
    SetEnable(false);

    return true;
}

void PopUPUI::Render(float deltaTime)
{
    EditorUI::Render(deltaTime);

    switch (_type)
    {
        case ePopUpType::Error:
            if (_open)
            {
                ImGui::OpenPopup("Error");
                _open = false;
            }

            if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", _msg.c_str());
                ImGui::Separator();
                if (ImGui::Button("OK"))
                {
                    _type = ePopUpType::End;
                    _msg.clear();
                    SetEnable(false);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            break;
        case ePopUpType::Warning:
        {
            if (_open)
            {
                ImGui::OpenPopup("Warning");
                _open = false;
            }

            if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", _msg.c_str());
                ImGui::Separator();

                if (ImGui::Button("OK"))
                {
                    _type = ePopUpType::End;
                    _msg.clear();
                    SetEnable(false);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        break;
        case ePopUpType::Info:
        {
            if (_open)
            {
                ImGui::OpenPopup("Info");
                _open = false;
            }

            if (ImGui::BeginPopupModal("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", _msg.c_str());
                ImGui::Separator();

                if (ImGui::Button("OK"))
                {
                    _type = ePopUpType::End;
                    _msg.clear();
                    SetEnable(false);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
            break;
        case ePopUpType::Confirm:
        {
            if (_open)
            {
                ImGui::OpenPopup("Confirm");
                _open = false;
            }
            if (ImGui::BeginPopupModal("Confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", _msg.c_str());
                ImGui::Separator();

                if (ImGui::Button("OK"))
                {
                    _type = ePopUpType::End;
                    _msg.clear();
                    SetEnable(false);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
            break;
        case ePopUpType::End:
            break;
        default:
            break;
    }
}

void PopUPUI::Destroy()
{
    EditorUI::Destroy();
}

void PopUPUI::Show(ePopUpType type, const std::string& msg)
{
    _type = type;
    _msg = msg;
    _open = true;

    SetEnable(true);
}

void PopUPUI::ShowError(const std::string& msg)
{
    Show(ePopUpType::Error, msg);
}

void PopUPUI::ShowWarning(const std::string& msg)
{
    Show(ePopUpType::Warning, msg);

}

void PopUPUI::ShowInfo(const std::string& msg)
{
    Show(ePopUpType::Info, msg);
}

void PopUPUI::ShowConfirm(const std::string& msg)
{
    Show(ePopUpType::Confirm, msg);
}
