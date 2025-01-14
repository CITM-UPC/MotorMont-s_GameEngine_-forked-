#include "MyGUI.h"
#include "Engine/FileManager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs/tinyfiledialogs.h> 




MyGUI::MyGUI(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

FileManager fileManager;
GameObject* selectedGameObject = nullptr; // Define selectedGameObject
void MyGUI::logMessage(const std::string& message) {
    logMessages.push_back(message);
}

void MyGUI::renderConsoleWindow() {
    ImGui::SetNextWindowSize(ImVec2(480, 200), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2(300, 450), ImGuiCond_Appearing);
    if (ImGui::Begin("Console", NULL)) {
        for (const auto& message : logMessages) {
            ImGui::TextUnformatted(message.c_str());
        }
    }
    ImGui::End();
}

void MyGUI::renderConfigurationWindow() {
    ImGui::SetNextWindowSize(ImVec2(480, 400), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiCond_Appearing);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);
    if (ImGui::Begin("Configuration", NULL)) {

        // FPS Graph
        static float fpsValues[90] = { 0 };
        static int fpsValuesOffset = 0;
        float fps = ImGui::GetIO().Framerate;
        fpsValues[fpsValuesOffset] = fps;
        fpsValuesOffset = (fpsValuesOffset + 1) % IM_ARRAYSIZE(fpsValues);
        ImGui::PlotLines("FPS", fpsValues, IM_ARRAYSIZE(fpsValues), fpsValuesOffset, "Frames Per Second", 0.0f, 120.0f, ImVec2(0, 80));

        // Configuration for modules
        if (ImGui::CollapsingHeader("Renderer")) {
            // Add renderer configuration options here
        }
        if (ImGui::CollapsingHeader("Window")) {
            // Add window configuration options here
        }
        if (ImGui::CollapsingHeader("Input")) {
            // Add input configuration options here
        }
        if (ImGui::CollapsingHeader("Textures")) {
            // Add texture configuration options here
        }

        // Information output
        if (ImGui::CollapsingHeader("Information")) {
            // Memory consumption
            //ImGui::Text("Memory Consumption: %.2f MB", getMemoryConsumption());

            // Hardware detection
            ImGui::Text("CPU Cores: %d", SDL_GetCPUCount());
            ImGui::Text("RAM: %d MB", SDL_GetSystemRAM());
            ImGui::Text("GPU: %s", glGetString(GL_RENDERER));

            // Software versions
            ImGui::Text("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
            //ImGui::Text("DevIL Version: %s", IL_VERSION); //It creates a crash, are we using DevIL?
        }
    }
    ImGui::End();
}

void MyGUI::renderGameObjectWindow()
{
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("Gameobjects", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        for (auto& child : scene.getChildren()) {
            static char newName[128] = "";
            static bool renaming = false;
            static GameObject* renamingObject = nullptr;

            if (renaming && renamingObject == &child) {
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename", newName, IM_ARRAYSIZE(newName), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    child.name = newName;
                    renaming = false;
                }
                if (ImGui::IsItemDeactivated() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    renaming = false;
                }
            }
            else {
                if (ImGui::Selectable(child.name.c_str(), selectedGameObject == &child)) {
                    selectedGameObject = &child;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    renaming = true;
                    renamingObject = &child;
                    strcpy_s(newName, child.name.c_str());
                }
            }
        }
    }
    ImGui::End();
}

void MyGUI::renderInspectorWindow()
{
    ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(780, 20), ImGuiCond_Always);
    if (ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        if (selectedGameObject) {
            ImGui::Text("Selected GameObject: %s", selectedGameObject->name.c_str());
            ImGui::Separator();
            // Display position
            ImGui::Text("Position");
            float position[3] = { static_cast<float>(selectedGameObject->_transform.pos().x),
                      static_cast<float>(selectedGameObject->_transform.pos().y),
                      static_cast<float>(selectedGameObject->_transform.pos().z) };

            if (ImGui::InputFloat3("##position", position)) {
                //selectedGameObject->_transform->Translate(glm::dvec3(position[0], position[1], position[2]));
            }

            // Display rotation
            ImGui::Text("Rotation");
            glm::vec3 rotation = selectedGameObject->_transform.GetRotation();
            float rotationArray[3] = { rotation.x, rotation.y, rotation.z };

            if (ImGui::InputFloat3("##rotation", rotationArray)) {
                //selectedGameObject->_transform->SetRotation(glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]));
            }

            //Display scale
            ImGui::Text("Scale");
            glm::vec3 scale = selectedGameObject->_transform.GetScale();
            float scaleArray[3] = { scale.x, scale.y, scale.z };

            if (ImGui::InputFloat3("##scale", scaleArray)) {
                //selectedGameObject->_transform->SetScale(glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]));
            }



            ImGui::Separator();
            //Display Texture Info
            
            if (selectedGameObject->GetComponent<MeshLoader>()->GetImage() != nullptr)
            {
                ImGui::Text("Texture Info");
                ImGui::Text("Texture Path: %s", selectedGameObject->texturePath.c_str());
                ImGui::Text("Texture size: %d x %d", selectedGameObject->GetComponent<MeshLoader>()->GetImage()->width(), selectedGameObject->GetComponent<MeshLoader>()->GetImage()->height());
            }
            
            if (ImGui::Checkbox("Draw Texture", &selectedGameObject->GetComponent<MeshLoader>()->drawTexture)) {
                if (selectedGameObject->GetComponent<MeshLoader>()->drawTexture)
                {
                    selectedGameObject->GetComponent<MeshLoader>()->GetMesh()->deleteCheckerTexture();
                }
            }

            ImGui::Separator();
            //Display Mesh Info         
            ImGui::Text("Mesh Info");
            ImGui::Text("Mesh Path: %s", selectedGameObject->meshPath.c_str());
            if (ImGui::Checkbox("Draw Normals", &selectedGameObject->GetComponent<MeshLoader>()->drawNormals)) {

            }
        }
        else {
            ImGui::Text("No GameObject selected.");
        }
    }
    ImGui::End();
}


void MyGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //Main Menu bar, Comment the line below if you don't want the main menu bar to appear
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Import FBX")) {
                // Open file dialog to select an FBX file
                const char* filterPatterns[1] = { "*.fbx" };
                const char* filePath = tinyfd_openFileDialog(
                    "Select an FBX file",
                    "Assets",
                    1,
                    filterPatterns,
                    NULL,
                    0
                );
                if (filePath) {
                    GameObject go;
                    fileManager.LoadFile(filePath, go);
                    scene.emplaceChild(go);
                }
            }
            if (ImGui::MenuItem("Quit")) {
                SDL_Quit();
                exit(0);
            }
            ImGui::EndMenu(); // Close the "File" menu

        }
        if (ImGui::BeginMenu("Create"))
        {
			if (ImGui::MenuItem("Cube"))
			{
				// Create a new GameObject with a cube mesh
				GameObject go;
                go = scene.createCube();
				scene.emplaceChild(go);
			}
            if (ImGui::MenuItem("Sphere"))
            {
				// Create a new GameObject with a sphere mesh
                GameObject go;
                go = scene.createSphere();
                scene.emplaceChild(go);
            }
            if (ImGui::MenuItem("Cylinder"))
            {
				// Create a new GameObject with a cylinder mesh
                GameObject go;
                go = scene.createCylinder();
                scene.emplaceChild(go);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Open a webpage when "About" is clicked
                const char* url = "https://github.com/CITM-UPC/Badass-Engine";
                SDL_OpenURL(url);
            }

            ImGui::EndMenu(); // Close the "Help" menu
        }
        ImGui::EndMainMenuBar(); // Close the main menu bar
    }
    //GameObject window, Comment the line below if you don't want the GameObject window to appear
	renderGameObjectWindow();
    //Inspector window, Comment the line below if you don't want the Inspector window to appear
	renderInspectorWindow();
	//Console window, Comment the line below if you don't want the Console window to appear
	renderConsoleWindow();
    
    // Ensure no two GameObjects have the same name
    std::unordered_set<std::string> nameSet;
    for (auto& child : scene.getChildren()) {
        if (nameSet.find(child.name) != nameSet.end()) {
            // If a duplicate name is found, append a unique suffix
            int suffix = 1;
            std::string newName = child.name + "_" + std::to_string(suffix);
            while (nameSet.find(newName) != nameSet.end()) {
                suffix++;
                newName = child.name + "_" + std::to_string(suffix);
            }
            child.name = newName;
        }
        nameSet.insert(child.name);
    }
    //ImGui::ShowDemoWindow();
	//render configuration window
	renderConfigurationWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
