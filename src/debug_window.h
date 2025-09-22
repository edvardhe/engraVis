#include "shader.h"

class DebugWindow {
public:
    DebugWindow();
    ~DebugWindow();
    void draw(unsigned int textureID);
private:
    float x, y, width, height;
    float vertices[16] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    unsigned int shaderProgram;

    Shader* debugShader;
};