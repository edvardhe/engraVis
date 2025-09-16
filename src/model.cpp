#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "model.h"

Model::Model(const std::string& path) {
    loadOBJ(path);
}

void Model::draw(const Shader& shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::loadOBJ(const std::string& path) {

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open OBJ file: " + path);
    }

    std::string line;
    char prefix;

    vertexCount = 0;
    faceCount = 0;

    while (getline(file, line)) {

        prefix = line[0];

        switch (prefix)
        {
        case 'v':
            vertexCount++;
            break;
        
        case 'f':
            faceCount++;
            break;

        default:
            break;
        }
    }

    file.clear();
    file.seekg(0, std::ios::beg);

    std::cout << "Loaded OBJ file: " << path << " with " 
              << vertexCount << " vertices and " 
              << faceCount << " faces." << std::endl;
    
    if (vertexCount == 0 || faceCount == 0) {
        throw std::runtime_error("OBJ file contains no vertices or faces!");
    }

    std::vector<float> vertices;
    vertices.reserve(vertexCount * 3);
    std::vector<unsigned int> indices;
    indices.reserve(faceCount * 3);

    float vx, vy, vz;
    unsigned int i1, i2, i3;

    maxDepth = 0.0f;
    minDepth = 0.0f;

    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> prefix;

        switch (prefix)
        {
        case 'v':
            iss >> vx >> vy >> vz;
            
            if (vz > maxDepth) maxDepth = vz;
            if (vz < minDepth) minDepth = vz;

            vertices.push_back(vx);
            vertices.push_back(vy);
            vertices.push_back(vz);
            break;
        
        case 'f':
            iss >> i1 >> i2 >> i3;
            indices.push_back(i1 - 1);
            indices.push_back(i2 - 1);
            indices.push_back(i3 - 1);
            break;

        default:
            break;
        }
    }

    file.close();
    std::cout << "Parsed " 
              << vertexCount << " vertices and " 
              << faceCount << " faces." << std::endl;

    VAO = 0;
    std::cout << VAO << std::endl;
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data() , GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    glBindVertexArray(0);
}

