#ifndef GUI_H
#define GUI_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class GUI {
public:
    GUI();
    ~GUI();

    // Initialize ImGui with the given GLFW window. Return false on failure.
    bool init(GLFWwindow* window);

    // Called at the start of each frame
    void newFrame();

    // Build the UI widgets for this frame
    void build();

    // Render ImGui draw data (call after your scene draw)
    void render();

    // Cleanup
    void shutdown();

    // Accessors for app state controlled by UI
    void getRotation(glm::vec3& out) const;
    void setRotation(const glm::vec3& in);
    float getScale() const;
    void setScale(const float scale);
    bool wireframeEnabled() const;

private:
    glm::vec3 rot_deg;
    float scale_val;
    bool wire;
    GLFWwindow* window;
};

#endif