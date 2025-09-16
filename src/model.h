#include <string>
#include "shader.h"

class Model {
public:
    Model(const std::string& path);
    void draw(const Shader& shader);

    float getMaxDepth() const { return maxDepth; }
    float getMinDepth() const { return minDepth; }

private:
    unsigned int vertexCount, faceCount;
    unsigned int VAO, VBO, EBO;
    float maxDepth, minDepth;

    void loadOBJ(const std::string& path);
};