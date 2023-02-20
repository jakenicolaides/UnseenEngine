#include "EngineUI.h"
#include "FileBrowser.h"
#include "Functions.h"
#include <iostream>
#include <imgui.h>
#include <windows.h>
#include <string>
#include <stb_image.h>
#include <backends/imgui_impl_vulkan.h>
#include "Rendering.h"
#include "Debug.h"


namespace EngineUI {


    std::map<std::string, bool> windowState;
    std::string currentFolderPath;
    std::string rootFolderPath;
    std::vector<FileBrowser::DirectoryItem> gameFiles;
    std::string currentSearch;
    std::string renameString;
    std::string newFolderString;

    BrowserFocus currentFocus = FileBrowser;

    bool showInspectorImagePreview = true;
    std::string newInspectorImagePath;
    ImGuiTextureData inspectorImagePreview;

    void initUI(Rendering* renderer) {
        windowState["applicationOpen"] = true;
        windowState["showDemoWindow"] = true;
        windowState["showMainMenuBar"] = true;
        windowState["showSceneHierarchy"] = true;
        windowState["showFolderViewer"] = true;
        windowState["showFileViewer"] = true;
        windowState["showInspector"] = true;
        windowState["showDebugConsole"] = true;

        rootFolderPath = std::filesystem::current_path().string() + "\\content";

        currentFolderPath = rootFolderPath;

        gameFiles = FileBrowser::getGameFiles(rootFolderPath);
        //FileBrowser::printGameFiles(gameFiles);

        debug.log("hello");

        ImGui::StyleColorsClassic();

    }

    std::map<std::string, bool> loopUI(bool& applicationOpen, Rendering* renderer) {

        gameFiles = FileBrowser::getGameFiles(rootFolderPath);

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
        style.WindowBorderSize = 0.0f;
        ImGui::End();


        if (windowState["showDemoWindow"]) { ImGui::ShowDemoWindow(&windowState["showDemoWindow"]); }

        // Inspector
        if (windowState["showInspector"])
        {
            ImGui::Begin("Inspector");
   
            //Inspector image preview
            if (showInspectorImagePreview && inspectorImagePreview.ImageMemory != nullptr && showInspectorImagePreview && inspectorImagePreview.Height != 0 && inspectorImagePreview.Width != 0) {
                ImGui::Text("pointer = %p", inspectorImagePreview.DS);
                ImGui::Text("size = %d x %d", inspectorImagePreview.Width, inspectorImagePreview.Height);
                float resizedWidth = 500;
                float resizedHeight = inspectorImagePreview.Height / (inspectorImagePreview.Width / resizedWidth);
                ImGui::Image((ImTextureID)inspectorImagePreview.DS, ImVec2(resizedWidth, resizedHeight));
            }
            else if (!showInspectorImagePreview) {
                switchNewInsectorImage(renderer);
            }
            else {
                ImGui::Text("Select an entity or file to view details...");
            }

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

        // Folder Viewer
        if (windowState["showFolderViewer"])
        {

            ImGui::Begin("Folder Browser");
            renderFolderTree(rootFolderPath, false);
            ImGui::End();
           
        }

        //Debug Console
        if (windowState["showDebugConsole"])
        {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin("DebugConsole");

            ImGui::Separator();

            ImGui::SetWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
              
            for (size_t i = 0; i < size(debug.logEntries); i++)
            {

                std::time_t currentTime = std::chrono::system_clock::to_time_t(debug.logEntries[i].timestamp);
                std::tm timeInfo;
                localtime_s(&timeInfo, &currentTime);
                std::stringstream ss;
                ss << std::put_time(&timeInfo, "%Y-%m-%d %X");
                std::string timestampString = ss.str();
                std::string logMessage = "[" + timestampString + "] " + debug.logEntries[i].logSource + ": " + debug.logEntries[i].message;

                // Define colors based on log type
                ImVec4 textColor;



                if (debug.logEntries[i].logType == Error)
                {
                    textColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                }
                else if (debug.logEntries[i].logType == Warning)
                {
                    textColor = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                }
                else
                {
                    textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }



                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                ImGui::Text("[%s] %s: %s", ss.str().c_str(), debug.logEntries[i].logSource.c_str(), debug.logEntries[i].message.c_str());
                ImGui::PopStyleColor();
            }
                
            ImGui::SetScrollHereY(1.0f);//scroll to bottom
    
            
            ImGui::EndChild();
            ImGui::End();


        }

        // FileViewer
        if (windowState["showFileViewer"])
        {
           
            ImGui::Begin("File Browser");

            /*
            if (ImGui::Selectable(currentFolderPath.c_str(), true, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    std::string path = currentFolderPath;
                    std::wstring w_file_path(path.begin(), path.end());
                    ShellExecute(NULL, L"open", L"explorer.exe", w_file_path.c_str(), NULL, SW_SHOWDEFAULT);
                }
            }
            */

            //Search bar
            static char buf1[64] = ""; 
            ImGui::InputText("Search", buf1, 64, ImGuiInputTextFlags_CallbackEdit, fileSearchCallback, &buf1);
            
            //Go to parent directory button
            if (currentFolderPath != rootFolderPath) {

                std::string path = currentFolderPath;
                size_t pos = path.find_last_of("\\");
                std::string parentPath = path.substr(0, pos);
                std::string parentText = "PARENT DIR";

                if (ImGui::Selectable(parentText.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                       
                        openDirectory(parentPath, FileBrowser);
                    }
                }
            }



            //Show folders
            for (FileBrowser::DirectoryItem item : gameFiles) {

                //If searching and no match found
                if (currentSearch != "" && !Functions::isSubstring(item.name, currentSearch)) { continue; }

                //If not a folder
                if (!item.isFolder) { continue; }

                if (item.parentPath == currentFolderPath || currentSearch != "") {
                    
                    std::string name = "";
                    name = " - " + item.name;
                                 
                    if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        if (ImGui::IsMouseDoubleClicked(0)) {
                            
                            debug.log("Opening folder: " + item.path);
                            openDirectory(item.path, FileBrowser);
                        }
                    }

                    showRightClickDirectoryOptions(item);
                }
            }

            //Show files
            for (FileBrowser::DirectoryItem item : gameFiles) {

                //If searching and no match found
                if (currentSearch != "" && !Functions::isSubstring(item.name, currentSearch)) { continue; }

                if (item.isFolder) { continue; }

                if (item.parentPath == currentFolderPath || currentSearch != "") {

                    std::string name = "";
                    name = item.name;

                    if (ImGui::Selectable(name.c_str(), ImGui::IsItemClicked(), ImGuiSelectableFlags_AllowDoubleClick)) {


                        std::string extension = Functions::getFilePathExtension(item.path);

                        //If this thing you clicked is an image, load a preview in the inspector
                        if (extension == "jpg" || extension == "png" || extension == "TGA") { prepareNewInspectorImage(item.path, renderer); }

                        //If double clicked
                        if (ImGui::IsMouseDoubleClicked(0)) {

                            //If opening image, use Paint.Net
                            if (extension == "jpg" || extension == "png" || extension == "TGA") {
                                // Convert the file path to a wide-character string
                                std::wstring w_file_path(item.path.begin(), item.path.end());

                                // Use the following command to open Paint.NET and load the image file
                                std::string photoEditingAppPath = "C:/Program Files/paint.net/paintdotnet.exe"; // make sure the path has forward slashes and not backslashes
                                HINSTANCE result = ShellExecute(NULL, L"open", Functions::ConvertToLPWSTR(photoEditingAppPath), (std::wstring(L"\"") + w_file_path + L"\"").c_str(), NULL, SW_SHOWDEFAULT);
                                if ((int)result <= 32) {
                                    DWORD error = GetLastError();

                                    LPSTR messageBuffer = nullptr;
                                    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

                                    std::cout << "Error: " << messageBuffer << std::endl;

                                    LocalFree(messageBuffer);
                                }
                                else {
                                    // The function succeeded
                                }
                            }

                            //If opening 3d model, use blender
                            if (extension == "fbx" || extension == "FBX" || extension == "obj" || extension == "3ds") {
                                std::string escapedPath = Functions::EscapeBackslashes(item.path);
                                FileBrowser::LaunchBlenderWithFBX(escapedPath);
                               
                            }

                           
                        }
                    }

                    //If right clicking
                    showRightClickDirectoryOptions(item);


                   
                }
            }
            ImGui::End();
        }

        //Main menu bar
        if (windowState["showMainMenuBar"]) {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Build", "Ctrl+B", false, windowState["isQuestConnectd"])) {} // Quest Build
                    if (ImGui::MenuItem("Quit", "Alt+F4")) { applicationOpen = false; }
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

    

    void renderFolderTree(std::string parentPath, bool clickCaptured) {

        for (FileBrowser::DirectoryItem item : gameFiles) {
            if (item.parentPath == parentPath && item.isFolder) {

                auto name = item.name.c_str();
                ImGui::PushID(name);

                //Check if item is a parent
                bool itemIsParent = false;
                for (FileBrowser::DirectoryItem item2 : gameFiles) { if (item2.parentPath == item.path && item2.isFolder) { itemIsParent = true; }}

                //If we are using the FileViewer then FolderViewer should reflect the current opened directory
                //If we are using the FolderViewer then FolderViewer nodes can be expanded or closed however the user likes
                if (currentFocus == FileBrowser) {
                    bool shouldBeExpanded = (currentFolderPath.find(item.path) != std::string::npos);
                    ImGui::SetNextItemOpen(shouldBeExpanded);
                }
                
                //Flags
                ImGuiTreeNodeFlags flags = 0;
                if (itemIsParent) {
                    flags = ImGuiTreeNodeFlags_Selected * (currentFolderPath == item.path) | ImGuiTreeNodeFlags_SpanFullWidth;
                }
                else {
                    flags = ImGuiTreeNodeFlags_Selected * (currentFolderPath == item.path) | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf;
                }
                
                bool nodeExpanded = ImGui::TreeNodeEx(name, flags);

                //If right clicking
                showRightClickDirectoryOptions(item);
                
                //If opening folder tree
                if (ImGui::IsItemClicked()) {
                    clickCaptured = true;
                    std::cout << item.path << std::endl;
                    openDirectory(item.path, FolderBrowser);
                }

                ImGui::PopID();
                if (nodeExpanded) {
                    //Do the same for my children
                    renderFolderTree(item.path, clickCaptured);
                    ImGui::TreePop();
                }
            }
        }
    }

    void openDirectory(std::string path, BrowserFocus newFocus) {
        currentFolderPath = path;
        currentFocus = newFocus;
    }


    //Prepare means: we stop rendering the current image so that we can free the memory
    //If we try to free the memory whilst the gpu is trying to display the image we will have problems
    void prepareNewInspectorImage(std::string imagePath, Rendering* renderer) {

        showInspectorImagePreview = false;
        newInspectorImagePath = imagePath;

    }

    void switchNewInsectorImage(Rendering* renderer) {

        //Free the memory and return so that we have a little breathing room 
        if (inspectorImagePreview.ImageMemory != nullptr) {
            renderer->RemoveTexture(&inspectorImagePreview);
        }

        bool ret = renderer->LoadTextureFromFile(newInspectorImagePath.c_str(), &inspectorImagePreview);
        showInspectorImagePreview = true;
        IM_ASSERT(ret);
    }

    void openPathInFileExplorer(std::string path) {

        std::wstring w_file_path(path.begin(), path.end());
        ShellExecute(NULL, L"open", L"explorer.exe", w_file_path.c_str(), NULL, SW_SHOWDEFAULT);

    }

    static int fileSearchCallback(ImGuiInputTextCallbackData* data){
        if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
            std::string str(data->Buf, data->Buf + data->BufTextLen);
            currentSearch = str;
        }
        return 0;
    }

    static int renameFileCallback(ImGuiInputTextCallbackData* data) {

        if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
            std::string str(data->Buf, data->Buf + data->BufTextLen);
            renameString = str;
        }
        return 0;
    }
    

    static int newFolderCallback(ImGuiInputTextCallbackData* data) {

        if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
            std::string str(data->Buf, data->Buf + data->BufTextLen);
            newFolderString = str;
        }
        return 0;
    }

    void showRightClickDirectoryOptions(FileBrowser::DirectoryItem item) {

        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            //Rename
            if (ImGui::Button("Rename")) {
                ImGui::OpenPopup("Rename");
                //ImGui::CloseCurrentPopup();
            }
            if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                std::string renameModalText = "Rename " + item.name + " to: ";
                ImGui::Text(renameModalText.c_str());

                std::string defaultName = item.name;
                char buf1[64];
                strncpy_s(buf1, defaultName.c_str(), 64);
                ImGui::InputText(" Rename Item ", buf1, 64, ImGuiInputTextFlags_CallbackEdit, renameFileCallback, &buf1);
                ImGui::Separator();

                if (ImGui::Button("Rename", ImVec2(120, 0))) {
                    FileBrowser::renameFolder(item.path, renameString);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            //Delete
            if (ImGui::Button("Delete")) {
                ImGui::OpenPopup("Delete?");
            }
            ImGui::BeginPopupContextItem();
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                std::string deleteModalText = item.name + " will be permantely deleted. Are you sure?";
                ImGui::Text(deleteModalText.c_str());
                ImGui::Separator();

                if (ImGui::Button("Confirm", ImVec2(120, 0))) {
                    FileBrowser::deleteFileOrFolder(Functions::ConvertToLPWSTR(item.path));
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            //New Folder
            if (item.isFolder) {
                if (ImGui::Button("Insert New Folder")) {
                    ImGui::OpenPopup("New Folder");
                }
                if (ImGui::BeginPopupModal("New Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    std::string newFolderModalText = item.path.c_str();
                    ImGui::Text(newFolderModalText.c_str());

                    std::string defaultName = "";
                    char buf1[64];
                    strncpy_s(buf1, defaultName.c_str(), 64);
                    ImGui::InputText(" Name ", buf1, 64, ImGuiInputTextFlags_CallbackEdit, newFolderCallback, &buf1);
                    ImGui::Separator();

                    if (ImGui::Button("Create Folder", ImVec2(120, 0))) {
                        std::string newFolderFinal = item.path + "\\" + newFolderString;
                        FileBrowser::createDirectory(newFolderFinal);
                        currentFolderPath = newFolderFinal;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }

            //Open in file explorer
            if (ImGui::Button("Open in File Explorer")) {
                ImGui::CloseCurrentPopup();
                openPathInFileExplorer(item.parentPath);
            }

            ImGui::EndPopup();
        }



    }

}