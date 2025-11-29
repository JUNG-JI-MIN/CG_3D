#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <imgui.h>

// ImGui + OpenGL3만 사용 (GLUT 플랫폼 백엔드 없음)
// Input은 GLUT 콜백에서 직접 전달해줌
class ImGuiManager {
public:
    using MouseClickCallback = std::function<void(int x, int y)>;

    ImGuiManager();
    ~ImGuiManager();

    // 초기화: 화면 크기(초기)와 GLSL 버전 문자열 전달
    void Init(int width, int height, const char* glsl_version = "#version 330");
    void NewFrame(float deltaTime);
    void Render();
    void Shutdown();

    // 간단한 UI 그리기 헬퍼 (원하면 확장 가능)
    void DrawDebugPanel(const glm::vec3& playerPos, GLuint textureId);

    // 입력 전달: 반환값 true이면 ImGui가 입력을 소비(캡처)함
    bool HandleKeyboard(unsigned char key, int x, int y); // 문자 입력
    bool HandleSpecialKey(int key, int x, int y); // 필요시 확장
    bool HandleMouse(int button, int state, int x, int y); // 버튼 및 휠
    bool HandleMouseMotion(int x, int y); // drag
    bool HandleMousePassiveMotion(int x, int y); // move without buttons

    // 화면/타이밍 업데이트
    void SetDisplaySize(int w, int h);
    void SetDeltaTime(float dt);

    // 게임용 콜백: ImGui가 마우스 캡처하지 않을 때 호출
    void SetMouseClickCallback(MouseClickCallback cb);

private:
    MouseClickCallback onMouseClick;
    int displayW = 800;
    int displayH = 600;
    float deltaTime = 1.0f / 60.0f;
    const char* glslVersion = "#version 330";
    bool initialized = false;
};