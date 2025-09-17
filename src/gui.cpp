#include "gui.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/type_ptr.hpp>

GUI::GUI()
    : rot_deg{0.0f, 0.0f, 0.0f}, scale_val(1.0f), wire(true), window(nullptr)
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