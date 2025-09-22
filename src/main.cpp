#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "model.h"
#include "gui.h"
#include "debug_window.h"

void processMovement(GLFWwindow *window, glm::vec3& rot, glm::mat4& translate, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rot.x += deltaTime * 100.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rot.x -= deltaTime * 100.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rot.y += deltaTime * 100.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rot.y -= deltaTime * 100.0f;

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

void processScaling(GLFWwindow *window, float& scale, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        scale += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        scale -= deltaTime;
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
    DebugWindow debug_window = DebugWindow();

    Shader zshader = Shader(
        SHADER_DIR "zshading.vert",
        SHADER_DIR "zshading.frag");

    //zshader.use();

    Shader depthShader = Shader(
        SHADER_DIR "depth.vert",
        SHADER_DIR "depth.frag");

    depthShader.use();

    Shader lighting = Shader(
        SHADER_DIR "lighting.vert",
        SHADER_DIR "lighting.frag");

    lighting.use();
    
    Shader current = lighting;

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
    glm::mat3 normalMatrix;
    scale = glm::mat4(1.0f);
    glm::vec3 rot_deg = {0.0f, 0.0f, 0.0f};
    float model_scale = 1.0f;

    translate = glm::mat4(1.0f);
    translate = glm::translate(translate, glm::vec3(0.0f, -0.1f, -1.0f));

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f;

    int fbWidth, fbHeight;

    glm::vec3 lightDir = glm::vec3(0.0f, 0.0f, 1.0f);


    // IMGUI SETUP
    GUI gui;
    if (!gui.init(window)) {
        std::cerr << "Failed to initialize ImGui" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // create depth texture
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);  

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        gui.getRotation(rot_deg);
        processMovement(window, rot_deg, translate, deltaTime);
        gui.setRotation(rot_deg);
        rot = rotate(glm::mat4(1.0f), glm::radians(rot_deg.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rot = rotate(rot, glm::radians(rot_deg.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rot = rotate(rot, glm::radians(rot_deg.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model_scale = gui.getScale();
        processScaling(window, model_scale, deltaTime);
        gui.setScale(model_scale);
        scale = glm::mat4(1.0f);
        scale = glm::scale(scale, glm::vec3(1.0f, 1.0f, model_scale));

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI FRAME
        gui.newFrame();
        gui.build();

        // apply wireframe toggle
        if (gui.wireframeEnabled()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // LOGIC
        current.use();
        model_matrix = translate * rot * scale;
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));

        unsigned int modelLoc = glGetUniformLocation(current.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

        unsigned int normalMatLoc = glGetUniformLocation(current.ID, "normalMatrix");
        glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        glfwPollEvents();

        glm::vec3 lightDir = gui.getLightDirection();
        unsigned int lightDirLoc = glGetUniformLocation(current.ID, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

        // DRAW

        // SHADOW PASS
        depthShader.use();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        float near_plane = 1.0f, far_plane = 5.5f;
        glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

        glm::vec3 modelCenter = glm::vec3(model_matrix[3]);
        glm::mat4 lightView = glm::lookAt(modelCenter + lightDir * 2.0f, modelCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpace = lightProjection * lightView;

        depthShader.use();
        unsigned int lightSpaceLoc = glGetUniformLocation(depthShader.ID, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpace));

        unsigned int modelLocDepth = glGetUniformLocation(depthShader.ID, "model");
        glUniformMatrix4fv(modelLocDepth, 1, GL_FALSE, glm::value_ptr(model_matrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        model.draw(depthShader);
        
        // RENDER PASS
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);

        current.use();

        // Bind shadow map texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        // Set shadowMap uniform to use texture unit 0
        unsigned int shadowMapLoc = glGetUniformLocation(current.ID, "shadowMap");
        glUniform1i(shadowMapLoc, 0);

        // Pass lightSpace matrix to lighting shader
        unsigned int lightSpaceLocLighting = glGetUniformLocation(current.ID, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceLocLighting, 1, GL_FALSE, glm::value_ptr(lightSpace));

        model.draw(current);

        //debug_window.draw(depthMap);

        gui.render();

        glfwPollEvents();
        glfwSwapBuffers(window);

    }

    gui.shutdown();
    glfwTerminate();

    return 0;
}

