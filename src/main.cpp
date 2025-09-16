#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "model.h"

void processMovement(GLFWwindow *window, glm::mat4& rot, glm::mat4& translate, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rot = glm::rotate(rot, glm::radians(deltaTime * 100.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rot = glm::rotate(rot, glm::radians(deltaTime * 100.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rot = glm::rotate(rot, glm::radians(deltaTime * 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rot = glm::rotate(rot, glm::radians(deltaTime * 100.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(0.0f, deltaTime * 0.5f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(0.0f, -deltaTime * 0.5f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(-deltaTime * 0.5f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(deltaTime * 0.5f, 0.0f, 0.0f));
    
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(0.0f, 0.0f, deltaTime * 0.5f));
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        translate = glm::translate(translate, glm::vec3(0.0f, 0.0f, -deltaTime * 0.5f));
}

void processScaling(GLFWwindow *window, glm::mat4& scale, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        scale = glm::scale(scale, glm::vec3(1.0f, 1.0f, 1.0f + deltaTime));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        scale = glm::scale(scale, glm::vec3(1.0f, 1.0f, 1.0f - deltaTime));
}

void resizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1000;
    int height = 800;

    GLFWwindow* window = glfwCreateWindow(width, height, "engraVis", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    Model model(ENGRAVINGS_DIR "cross.obj");

    Shader dummy = Shader(
        SHADER_DIR "dummy.vert",
        SHADER_DIR "dummy.frag"); 
        
    dummy.use();

    Shader zshader = Shader(
        SHADER_DIR "zshading.vert",
        SHADER_DIR "zshading.frag");

    zshader.use();

    Shader current = zshader;

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    unsigned int projectionLoc = glGetUniformLocation(current.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    zshader.setFloat("zMin", model.getMinDepth());
    zshader.setFloat("zMax", model.getMaxDepth());


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 scale;
    glm::mat4 rot;
    glm::mat4 translate;
    glm::mat4 model_matrix;
    scale = glm::mat4(1.0f);    
    rot = glm::mat4(1.0f);
    
    translate = glm::mat4(1.0f);
    translate = glm::translate(translate, glm::vec3(0.0f, -0.1f, -1.0f));

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f;

    int fbWidth, fbHeight;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        processMovement(window, rot, translate, deltaTime);
        processScaling(window, scale, deltaTime);

        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (height != fbHeight || width != fbWidth)
        {
            width = fbWidth;
            height = fbHeight;
            resizeWindow(window, width, height);
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
            unsigned int projectionLoc = glGetUniformLocation(current.ID, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 

        // LOGIC

        model_matrix = translate * rot * scale;

        unsigned int modelLoc = glGetUniformLocation(current.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));   

        // DRAW

        model.draw(current);

        glfwPollEvents();    
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

