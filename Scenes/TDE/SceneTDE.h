#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CPrimitives.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include <array>
#include <random>

class SceneTDE : public CSceneBase
{
public:
    explicit SceneTDE(int exercise);
    ~SceneTDE() override;
    void DrawScene(float deltaTime) override;
    void ProcessSceneInput(GLFWwindow* window, float deltaTime) override;

private:
    int exercise;
    CCamera camera;
    CShader shader;
    CTextures textures;
    CPrimitives cube, sphere, cylinder, cone, ground;
    GLuint houseVAO = 0, houseVBO = 0;
    GLsizei houseVertices = 0;
    std::array<std::vector<glm::vec3>, 5> stacks;
    std::mt19937 random;
    double startTime;
    int generatedCubes = 0;
    std::string error;

    void Reset();
    void DrawObject(CPrimitives& primitive, const glm::mat4& parent, glm::vec3 position,
        glm::vec3 scale, glm::vec3 color, int texture = -1);
    void DrawTree(glm::vec3 position, glm::vec3 scale);
    void DrawSnowman();
    void UpdateStacks();
    void CreateHouse();
    void LoadHouse(const std::string& path);
    void UploadHouse(const std::vector<float>& vertices);
};
