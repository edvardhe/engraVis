#include "debug_window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

DebugWindow::DebugWindow() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Load vertex array
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    debugShader = new Shader(SHADER_DIR "debug_quad.vert", SHADER_DIR "debug_quad.frag");
}

DebugWindow::~DebugWindow() {

}

void DebugWindow::draw(unsigned int textureID) {
    debugShader->use();
    glBindTexture(GL_TEXTURE_2D, textureID);
    debugShader->setInt("screenTexture", 0);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}