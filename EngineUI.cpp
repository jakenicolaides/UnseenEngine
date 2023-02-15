#include "EngineUI.h"
#include "FileBrowser.h"
#include <iostream>
#include <imgui.h>


void renderFolderTree(FileBrowser::Folder* folder, bool isExpanded = false, int depth = 0)
{
    ImGui::PushID(folder->name.c_str());

    bool nodeExpanded = ImGui::TreeNodeEx(folder->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen * isExpanded);

    

    if (nodeExpanded)
    {
        for (FileBrowser::Folder* child : folder->children)
        {
            renderFolderTree(child, false, depth + 1);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

std::map<std::string, bool> EngineUI(std::map<std::string, bool> windowState) {
    //Docking
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("My Docked Window", NULL, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    //Style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0.0f, 0.0f); //no padding
    style.Colors[ImGuiCol_WindowBg].w = 1.0f; //fully opaque windows
    ImGui::End();


    if (windowState["showDemoWindow"]) { ImGui::ShowDemoWindow(&windowState["showDemoWindow"]); }

    // Inspector
    if (windowState["showInspector"])
    {
        ImGui::Begin("Inspector");
        ImGui::Text("Select an entity to view details...");
        ImGui::End();
    }

    // SceneHierarchy
    if (windowState["showSceneHierarchy"])
    {
        ImGui::Begin("SceneHierarchy");
        if (ImGui::TreeNode("World"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf_s(buf, "  Object %d", n);
                if (ImGui::Selectable(buf, selected == n))
                    selected = n;
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }

    // ContentBrowser
    if (windowState["showContentBrowser"])
    {

        FileBrowser::Folder* rootFolder = FileBrowser::getRootFolder();
        
        ImGui::Begin("ContentBrowser");

        renderFolderTree(rootFolder);
       
        ImGui::End();

        
       
        
    }


    //Main menu bar
    if (windowState["showMainMenuBar"]) {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Build", "Ctrl+B", false, windowState["isQuestConnectd"])) {} // Quest Build
                if (ImGui::MenuItem("Quit", "Alt+F4")) { windowState["applicationOpen"] = false; }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    return windowState;
}