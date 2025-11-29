#include "imgui_manager.h"
#include <imgui_impl_opengl3.h>
#include <cstdio>

ImGuiManager::ImGuiManager() {}
ImGuiManager::~ImGuiManager() { Shutdown(); }

void ImGuiManager::Init(int width, int height, const char* glsl_version) {
    if (initialized) return;
    displayW = width;
    displayH = height;
    glslVersion = glsl_version ? glsl_version : "#version 330";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2((float)displayW, (float)displayH);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // optional

    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_Init(glslVersion);

    initialized = true;
}

void ImGuiManager::NewFrame(float dt) {
    if (!initialized) return;
    deltaTime = dt;
    ImGui_ImplOpenGL3_NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)displayW, (float)displayH);
    io.DeltaTime = deltaTime;
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    if (!initialized) return;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::Shutdown() {
    if (!initialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    initialized = false;
}

void ImGuiManager::DrawDebugPanel(const glm::vec3& playerPos, GLuint textureId) {
    if (!initialized) return;
    ImGui::Begin("Debug Panel");
    ImGui::Text("Player: %.2f, %.2f, %.2f", playerPos.x, playerPos.y, playerPos.z);
    if (textureId != 0) {
        ImGui::Text("Player Texture:");
        ImGui::Image((void*)(intptr_t)textureId, ImVec2(128, 128));
    }
    ImGui::Separator();
    ImGui::Text("Display: %d x %d", displayW, displayH);
    ImGui::End();
}

bool ImGuiManager::HandleKeyboard(unsigned char key, int x, int y) {
    if (!initialized) return false;
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((unsigned int)key);
    // Return whether ImGui wants to capture keyboard
    return io.WantCaptureKeyboard;
}

bool ImGuiManager::HandleSpecialKey(int key, int x, int y) {
    // 확장 포인트: special keys if needed
    return false;
}

bool ImGuiManager::HandleMouse(int button, int state, int x, int y) {
    if (!initialized) return false;
    ImGuiIO& io = ImGui::GetIO();
    // update pos
    io.MousePos = ImVec2((float)x, (float)y);

    // wheel (freeglut convention: button 3/4)
    if (button == 3) { io.MouseWheel += 1.0f; return false; }
    if (button == 4) { io.MouseWheel -= 1.0f; return false; }

    int b = -1;
    if (button == GLUT_LEFT_BUTTON)  b = 0;
    if (button == GLUT_RIGHT_BUTTON) b = 1;
    if (button == GLUT_MIDDLE_BUTTON)b = 2;
    if (b >= 0) {
        bool down = (state == GLUT_DOWN);
        if (b < 5) io.MouseDown[b] = down;
    }

    // 만약 ImGui가 마우스를 캡처하면 게임에 전달하지 않음
    if (io.WantCaptureMouse) return true;

    // ImGui가 캡처하지 않으면 클릭 콜백 호출 (좌버튼 다운 시)
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && onMouseClick) {
        onMouseClick(x, y);
    }
    return false;
}

bool ImGuiManager::HandleMouseMotion(int x, int y) {
    if (!initialized) return false;
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    return io.WantCaptureMouse;
}

bool ImGuiManager::HandleMousePassiveMotion(int x, int y) {
    if (!initialized) return false;
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);
    return io.WantCaptureMouse;
}

void ImGuiManager::SetDisplaySize(int w, int h) {
    displayW = w;
    displayH = h;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)displayW, (float)displayH);
}

void ImGuiManager::SetDeltaTime(float dt) {
    deltaTime = dt;
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaTime;
}

void ImGuiManager::SetMouseClickCallback(MouseClickCallback cb) {
    onMouseClick = cb;
}