#include "gui.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

GUI::GUI()
    : rot_deg{0.0f, 0.0f, 0.0f}, scale_val(1.0f), wire(true), window(nullptr), joystickPos(0.0f, 0.0f)
{}

GUI::~GUI() { shutdown(); }

bool GUI::init(GLFWwindow* win)
{
    window = win;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) return false;
    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) return false;
    return true;
}

void GUI::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::build()
{
    // simple right-side settings panel
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat3("Rotation (deg)", glm::value_ptr(rot_deg), -180.0f, 180.0f);
    ImGui::SliderFloat("Scale", &scale_val, -1.0f, 1.0f);
    ImGui::Checkbox("Wireframe", &wire);
    ImGui::Text("rotate - WASD");
    ImGui::Text("scale - Q/E");
    ImGui::Text("translate - arrow keys, +/-");

    ImGui::Spacing();
    ImGui::Text("Light direction");

    const float joystickRadius = 80.0f;
    float maxElevationDeg = 80.0f;

    // joystick area
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImVec2 size(joystickRadius * 2.0f, joystickRadius * 2.0f);
    ImGui::InvisibleButton("joystick", size);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(cursor.x + size.x * 0.5f, cursor.y + size.y * 0.5f);

    // circle background
    dl->AddCircle(center, joystickRadius, IM_COL32(200,200,200,40), 64, 3.0f);

    // handle logic
    ImVec2 handlePos = center;
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
    {
        ImVec2 mp = ImGui::GetIO().MousePos;
        ImVec2 delta = ImVec2(mp.x - center.x, mp.y - center.y);
        float dist = sqrtf(delta.x*delta.x + delta.y*delta.y);
        if (dist > joystickRadius) { delta.x *= joystickRadius/dist; delta.y *= joystickRadius/dist; }
        // update stored joystick position (y inverted so up = negative delta.y -> positive elevation)
        joystickPos = glm::vec2(delta.x, -delta.y);
    }

    // draw handle
    handlePos.x += joystickPos.x;
    handlePos.y -= joystickPos.y;
    dl->AddCircleFilled(handlePos, 8.0f, IM_COL32(255,100,100,200));
    ImGui::Text("crosshair %.2f %.2f", joystickPos.x, joystickPos.y);

    ImGui::End();
}

void GUI::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown()
{
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void GUI::getRotation(glm::vec3& out) const
{
    out = rot_deg;
}

void GUI::setRotation(const glm::vec3& in)
{
    rot_deg = in;
}

float GUI::getScale() const
{
    return scale_val;
}

void GUI::setScale(float scale)
{
    scale_val = scale;
}

bool GUI::wireframeEnabled() const
{
    return wire;
}

glm::vec3 GUI::getLightDirection() const
{
    // map handle pixels to unit disk using the configured radius
    glm::vec2 n = joystickPos / 80.0f;
    float len2 = glm::clamp(glm::dot(n, n), 0.0f, 1.0f);
    float z = sqrtf(1.0f - len2);
    glm::vec3 dir(n.x, n.y, z);
    return dir;
}