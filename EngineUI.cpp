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
#include "Serialization.h"


namespace EngineUI {


    std::map<std::string, bool> windowState;
    std::string currentFolderPath;
    std::string currentHoveredItem = "";
    std::string currentSelectedItem = "";
    FileBrowser::ModelInfo currentModelQuery;
    std::string rootFolderPath;
    std::vector<FileBrowser::DirectoryItem> gameFiles;
    std::string currentSearch;
    std::string currentLogSearch;
    std::string renameString;
    std::string newFolderString;
    bool isNavigatingViewport;
    bool viewportNavigationGate;

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
        windowState["showViewportControls"] = true;

        rootFolderPath = std::filesystem::current_path().string() + "\\content";
        currentFolderPath = rootFolderPath;
        gameFiles = FileBrowser::getGameFiles(rootFolderPath);

        debug.log("UI Initialised");

        ImGui::StyleColorsClassic();

        Serialization::generateYamlFile(rootFolderPath,"test",674457346353);

    }

    bool getIsNavigatingViewport() {
        return isNavigatingViewport;
    }

    std::map<std::string, bool> loopUI(bool& applicationOpen, Rendering* renderer, float& cameraSpeed, float& fov, bool& firstMouse) {

        isNavigatingViewport = ImGui::IsMouseDown(ImGuiMouseButton_Right) && !ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsAnyItemHovered();

        if (isNavigatingViewport) { ImGui::SetMouseCursor(ImGuiMouseCursor_None); }


        if (isNavigatingViewport && !viewportNavigationGate) {
            viewportNavigationGate = true;
            firstMouse = true;
        }else if(!isNavigatingViewport && viewportNavigationGate) {
            viewportNavigationGate = false;
            firstMouse = false;
        }
      

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

            if (currentSelectedItem == "") {
                ImGui::Text("Select an entity or file to view details...");
            }
            else if(Functions::getFileType(currentSelectedItem) == "image") {
                //This part just checks if the image is ready to go
                if (showInspectorImagePreview && inspectorImagePreview.ImageMemory != nullptr && inspectorImagePreview.Height != 0 && inspectorImagePreview.Width != 0) {
                    ImGui::Text("pointer = %p", inspectorImagePreview.DS);
                    ImGui::Text("size = %d x %d", inspectorImagePreview.Width, inspectorImagePreview.Height);
                    float resizedWidth = 500;
                    float resizedHeight = inspectorImagePreview.Height / (inspectorImagePreview.Width / resizedWidth);
                    ImGui::Image((ImTextureID)inspectorImagePreview.DS, ImVec2(resizedWidth, resizedHeight));
                }
                else if (!showInspectorImagePreview) {
                    switchNewInsectorImage(renderer);
                }
            }
            else if (Functions::getFileType(currentSelectedItem) == "folder") {
                
                FileBrowser::FolderInfo folder = FileBrowser::getFolderInfo(currentSelectedItem);
                std::string folderInfoString = "Name: " + folder.name + "\nFolders: " + std::to_string(folder.numFolders) + "\nFiles: " + std::to_string(folder.numFiles) + "\nDate Created: " + folder.dateCreated + "\nDate Modified: " + folder.dateModified;
                ImGui::Text(folderInfoString.c_str());
            }
            else if (Functions::getFileType(currentSelectedItem) == "model") {

                if (currentModelQuery.name != Functions::getFileNameFromPath(currentSelectedItem)) {
                    currentModelQuery = FileBrowser::getModelInfo(currentSelectedItem);
                }
               
                std::string modelInfoString = "Name: " + currentModelQuery.name + "\nType: " + currentModelQuery.type + "\nFile Size: " + Functions::convertBytes(currentModelQuery.fileSize, 'M') + " MB" + "\nMaterial Slots: " + std::to_string(currentModelQuery.numMaterials) + "\nNum Polygons: " + std::to_string(currentModelQuery.numPolygons) + "\nNum Verticies: " + std::to_string(currentModelQuery.numVerts);
                ImGui::Text(modelInfoString.c_str());

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
           
            ImGui::Begin("Debug Log", NULL, ImGuiWindowFlags_NoScrollbar);
            renderLog();
            ImGui::End();
          
        }

        //Viewport Controls
        if (windowState["showViewportControls"])
        {

            ImGui::Begin("Viewport Controls");
 
           
            ImGui::PushItemWidth(200);
            ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1, 1000);
            ImGui::PopItemWidth();

            ImGui::SameLine();

            ImGui::PushItemWidth(200);
            ImGui::SliderFloat("FOV", &fov, 1, 180);
            ImGui::PopItemWidth();

            ImGui::SameLine();

            ImGui::Button("Play");

            ImGui::SameLine();

            ImGui::Button("Pause");

            ImGui::SameLine();

            ImGui::Button("Stop");

            ImGui::End();
 
        }

        // FileViewer
        if (windowState["showFileViewer"])
        {
           
            ImGui::Begin("File Browser");

            //File browser options
            static char buf1[64] = ""; 
            if (currentFolderPath != rootFolderPath) {
                if (ImGui::Button("<-")) {
                    currentFolderPath = FileBrowser::getParentDirectory(currentFolderPath);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Open in Explorer")){
                openPathInFileExplorer(currentFolderPath);
            }
            ImGui::SameLine();
            ImGui::Button("New Entity");
            ImGui::SameLine();
            ImGui::Button("New Material");
            ImGui::SameLine();
            if (ImGui::Button("Load new model")) {
                renderer->loadModelDynamically("C:\\UnseenEngine\\content\\SpiritLens\\models\\metalfish.obj", "C:\\UnseenEngine\\content\\SpiritLens\\models\\metalfish.png");
                renderer->loadModelDynamically("C:\\UnseenEngine\\content\\SpiritLens\\models\\Clover.obj", "C:\\UnseenEngine\\content\\SpiritLens\\models\\Frog Chair\\froggy.png");
            }

           
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputText("Search Files", buf1, 64, ImGuiInputTextFlags_CallbackEdit, fileSearchCallback, &buf1);
            
            //Go to parent directory button
            /*
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
            */



            //Show folders
            for (FileBrowser::DirectoryItem item : gameFiles) {

                //If searching and no match found
                if (currentSearch != "" && !Functions::isSubstring(item.name, currentSearch)) { continue; }

                //If not a folder
                if (!item.isFolder) { continue; }

                if (item.parentPath == currentFolderPath || currentSearch != "") {
                    
                    std::string name = "";
                    name = " - " + item.name;
                    
                    bool isSelected = (currentSelectedItem == (item.path));
                                 
                    if (ImGui::Selectable(name.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
                    {

                        currentSelectedItem = item.path;

                        if (ImGui::IsMouseDoubleClicked(0)) {
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

                    bool isSelected = (currentSelectedItem == item.path);


                    if (ImGui::Selectable(name.c_str(),isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {

                        currentSelectedItem = item.path;

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

                    //Save current hovered
                    if (ImGui::IsItemHovered()) {
                        currentHoveredItem = item.name;
                    }
                    //Drag and drop
                    if (item.name == currentHoveredItem) {
                        //Drap and drop
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                            ImGui::Text(item.name.c_str());
                            ImGui::EndDragDropSource();
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
   
    static int editSearchLogCallback(ImGuiInputTextCallbackData* data) {

        if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
            std::string str(data->Buf, data->Buf + data->BufTextLen);
            currentLogSearch = str;
        }
        return 0;
    }


    void renderLog() {
        
        
        //clear log button
        if (ImGui::Button("Clear Log")) {
            debug.clearLog();
            debug.log("Log cleared");
        }
        ImGui::SameLine();

        //Search log
        std::string defaultSearchText = "";
        const int searchBufferSize = 64;
        char searchBuffer[searchBufferSize];
        strncpy_s(searchBuffer, defaultSearchText.c_str(), searchBufferSize);
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Search Log", searchBuffer, searchBufferSize, ImGuiInputTextFlags_CallbackEdit, editSearchLogCallback, searchBuffer);

        //if no entries, just return
        if (size(debug.logEntries) < 1) {
            return;
        }

        const size_t bufferSize = 1000000; // allocate 1 million characters for the buffer
        char* buf = new char[bufferSize]; // dynamically allocate buffer
        size_t offset = 0;
        size_t windowWidth = ImGui::GetWindowContentRegionWidth();

        for (size_t i = 0; i < size(debug.logEntries); i++)
        {

            //If we are searching and the search query is not contained within this log message
            if (currentLogSearch != "" && !Functions::isSubstring(debug.logEntries[i].message, currentLogSearch)) { continue; }

            std::time_t currentTime = std::chrono::system_clock::to_time_t(debug.logEntries[i].timestamp);
            std::tm timeInfo;
            localtime_s(&timeInfo, &currentTime);

            std::stringstream ss;
            ss << std::put_time(&timeInfo, "%X");
            std::string timestampString = ss.str();
            std::string logMessage = "[" + timestampString + "] " + debug.logEntries[i].logSource + ": " + debug.logEntries[i].message;
            float textWidth = estimateTextWidth(logMessage.c_str()) * 14;

            logMessage = Functions::insertNewlines(logMessage, windowWidth / 17); //add newline for text wrapping

            // check if adding the log message would exceed buffer size
            size_t messageLength = logMessage.length() + 1; // include null terminator
            if (offset + messageLength >= bufferSize) {
                break; // exit loop if buffer is full
            }

            strcpy_s(buf + offset, bufferSize - offset, logMessage.c_str());
            offset += messageLength - 1; // exclude null terminator
            if (Functions::countNewlines(logMessage) == 0) {
                buf[offset++] = '\n'; // append newline character after every log entry
            }

            //If last line in vector then add a couple more newlines
            if (i == (size(debug.logEntries) - 1)) {
                buf[offset++] = '\n';
                buf[offset++] = '\n';
                buf[offset++] = '\n';
            }
        }

        // null terminate the buffer
        buf[offset] = '\0';

        //If last input message was on this tick, then scroll to the bottom
        //All this horrendous text does is check the last logEntry timestamp and see if its equal to the now timesteamp
        //All the grossness is just c++ verboseness
        if (std::chrono::system_clock::to_time_t(debug.logEntries[size(debug.logEntries) - 1].timestamp) == std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())){
            ImGui::SetNextWindowScroll(ImVec2(0.0f, ImGui::GetWindowHeight() *999));
        }
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 20));
        ImGui::InputTextMultiline("##source", buf, bufferSize, ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 70), flags);
        ImGui::PopStyleVar();

        delete[] buf; // deallocate buffer

        
    }

    float estimateTextWidth(const char* text)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->Fonts[0]; // assuming the first font is being used

        ImVec2 size = font->CalcTextSizeA(io.FontGlobalScale, FLT_MAX, 0.0f, text);
        return size.x;
    }

    void showRightClickDirectoryOptions(FileBrowser::DirectoryItem item) {

        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {

            currentSelectedItem = item.path;

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