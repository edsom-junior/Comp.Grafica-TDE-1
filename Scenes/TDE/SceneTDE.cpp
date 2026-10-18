#include "SceneTDE.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

SceneTDE::SceneTDE(int exercise) : exercise(exercise), random(std::random_device{}())
{
    shader.LoadShader("TDE", "Scenes/TDE/Scene.vert", "Scenes/TDE/Scene.frag");
    cube.generateCube(1.0f);
    sphere.generateSphere(1.0f);
    cylinder.generateCylinder(1.0f, 1.0f);
    cone.generateCone(1.0f, 1.0f);
    ground.generatePlane(1.0f, 1.0f);
    for (auto primitive : { &cube, &sphere, &cylinder, &cone, &ground })
        primitive->setupBuffers();
    if (exercise >= 3)
    {
        textures.LoadTextureAnisotropic(0, "Scenes/TDE/Assets/grama.bmp");
        textures.LoadTextureAnisotropic(1, "Scenes/TDE/Assets/CRATE.BMP");
        textures.LoadTextureAnisotropic(2, "Scenes/TDE/Assets/CasaAtlas.jpg");
        textures.LoadTextureAnisotropic(3, "Scenes/TDE/Assets/CasaTex.jpg");
        try
        {
            if (exercise == 3) CreateHouse();
            else LoadHouse("Scenes/TDE/Assets/Casa.txt");
        }
        catch (const std::exception& exception)
        {
            error = exception.what();
            std::cerr << error << '\n';
        }
    }
    Reset();
}

SceneTDE::~SceneTDE()
{
    glDeleteBuffers(1, &houseVBO);
    glDeleteVertexArrays(1, &houseVAO);
}

void SceneTDE::Reset()
{
    startTime = glfwGetTime();
    generatedCubes = 0;
    for (auto& stack : stacks) stack.clear();
    if (exercise == 1) camera = CCamera({ 12, 8, 21 }, { 0, 1, 0 }, -120, -15);
    else if (exercise == 2) camera = CCamera({ 12, 10, 20 }, { 0, 1, 0 }, -120, -15);
    else if (exercise == 3) camera = CCamera({ 17, 7, 24 }, { 0, 1, 0 }, -117, -9);
    else camera = CCamera({ 29, 18, 33 }, { 0, 1, 0 }, -130, -16);
}

void SceneTDE::DrawObject(CPrimitives& primitive, const glm::mat4& parent,
    glm::vec3 position, glm::vec3 scale, glm::vec3 color, int texture)
{
    auto model = glm::scale(glm::translate(parent, position), scale);
    shader.SetMat4("model", model);
    shader.SetVec4("Color", glm::vec4(color, 1));
    shader.SetBool("isTextured", texture >= 0);
    if (texture >= 0) textures.ApplyTexture(texture);
    primitive.render();
}

void SceneTDE::DrawTree(glm::vec3 position, glm::vec3 scale)
{
    auto model = glm::scale(glm::translate(glm::mat4(1), position), scale);
    DrawObject(cylinder, model, { 0, 1.2f, 0 }, { 0.25f, 2.4f, 0.25f }, { 0.65f, 0.28f, 0.05f });
    DrawObject(cone, model, { 0, 3.0f, 0 }, { 1.7f, 3.4f, 1.7f }, { 0.02f, 0.45f, 0.02f });
    DrawObject(cone, model, { 0, 4.4f, 0 }, { 1.45f, 3.4f, 1.45f }, { 0.02f, 0.65f, 0.02f });
}

void SceneTDE::DrawSnowman()
{
    float angle = glm::radians(22.5f) * std::sin(float(glfwGetTime() - startTime) * 1.8f);
    auto model = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
    DrawObject(sphere, model, { 0, 1.1f, 0 }, { 1.1f, 1.1f, 1.1f }, { 1, 1, 1 });
    DrawObject(sphere, model, { 0, 2.65f, 0 }, { 0.78f, 0.78f, 0.78f }, { 1, 1, 1 });
    DrawObject(sphere, model, { 0, 3.85f, 0 }, { 0.57f, 0.57f, 0.57f }, { 1, 1, 1 });
    for (float x : { -0.21f, 0.21f })
        DrawObject(sphere, model, { x, 3.97f, 0.51f }, { 0.09f, 0.09f, 0.09f }, { 0.02f, 0.02f, 0.02f });
    for (float y : { 2.4f, 2.85f })
        DrawObject(sphere, model, { 0, y, 0.75f }, { 0.12f, 0.12f, 0.12f }, { 0.9f, 0.04f, 0.03f });
    auto nose = glm::rotate(glm::translate(model, { 0, 3.8f, 0.7f }), glm::radians(90.0f), glm::vec3(1, 0, 0));
    DrawObject(cone, nose, { 0, 0, 0 }, { 0.13f, 0.5f, 0.13f }, { 1, 0.4f, 0 });
    DrawObject(cylinder, model, { 0, 4.4f, 0 }, { 0.73f, 0.12f, 0.73f }, { 0.05f, 0.05f, 0.9f });
    DrawObject(cylinder, model, { 0, 4.7f, 0 }, { 0.43f, 0.6f, 0.43f }, { 0.05f, 0.05f, 0.9f });
}

void SceneTDE::UpdateStacks()
{
    int elapsed = static_cast<int>(glfwGetTime() - startTime);
    while (generatedCubes < elapsed)
    {
        std::vector<int> available;
        for (int i = 0; i < 5; ++i)
            if (stacks[i].size() < 10) available.push_back(i);
        if (available.empty())
        {
            for (auto& stack : stacks) stack.clear();
            available = { 0, 1, 2, 3, 4 };
        }
        int index = available[std::uniform_int_distribution<int>(0, int(available.size()) - 1)(random)];
        std::uniform_real_distribution<float> color(0.15f, 1.0f);
        stacks[index].push_back({ color(random), color(random), color(random) });
        ++generatedCubes;
    }
}

void SceneTDE::DrawScene(float deltaTime)
{
    glClearColor(0.015f, 0.02f, 0.035f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use("TDE");
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect = float(viewport[2]) / float(std::max(viewport[3], 1));
    shader.SetMat4("projection", glm::perspective(glm::radians(camera.Zoom), aspect, Z_NEAR, Z_FAR));
    shader.SetMat4("view", camera.GetViewMatrix());
    shader.SetInt("Texture0", 0);
    shader.SetFloat("tiling", exercise >= 3 ? 32.0f : 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glm::mat4 identity(1);
    DrawObject(ground, identity, { 0, -0.02f, 0 }, { 100, 1, 100 },
        exercise == 1 ? glm::vec3(1) : glm::vec3(0.18f, 0.21f, 0.25f), exercise >= 3 ? 0 : -1);
    shader.SetFloat("tiling", 1);
    if (exercise == 1)
    {
        DrawTree({ -5, 0, 3 }, { 1.1f, 1.1f, 1.1f });
        DrawTree({ -6, 0, -5 }, { 1.4f, 1.4f, 1.4f });
        DrawTree({ 0, 0, -8 }, { 1.2f, 1.2f, 1.2f });
        DrawTree({ 6, 0, -6 }, { 1.5f, 1.5f, 1.5f });
        DrawTree({ 7, 0, 2 }, { 0.85f, 0.85f, 0.85f });
        DrawSnowman();
    }
    else if (exercise == 2)
    {
        UpdateStacks();
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < int(stacks[i].size()); ++j)
                DrawObject(cube, identity, { (i - 2) * 2.0f, j + 0.5f, 0 }, { 1, 1, 1 }, stacks[i][j]);
    }
    else
    {
        shader.SetMat4("model", identity);
        shader.SetBool("isTextured", true);
        textures.ApplyTexture(exercise == 3 ? 2 : 3);
        glBindVertexArray(houseVAO);
        if (houseVertices > 0) glDrawArrays(GL_TRIANGLES, 0, houseVertices);
        glBindVertexArray(0);
        if (exercise == 3)
        {
            DrawObject(cube, identity, { 8, 0.6f, 3 }, { 1.2f, 1.2f, 1.2f }, { 1, 1, 1 }, 1);
            DrawObject(cube, identity, { 9.6f, 1.2f, 1 }, { 2.4f, 2.4f, 2.4f }, { 1, 1, 1 }, 1);
            DrawObject(cube, identity, { 10.5f, 2.1f, -2.1f }, { 3.4f, 4.2f, 2.4f }, { 1, 1, 1 }, 1);
        }
    }
    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.75f);
    ImGui::Begin("TDE", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);
    ImGui::Text("Exercicio %d | Teclas 1 a 4: trocar cena", exercise);
    ImGui::TextUnformatted("WASD: mover | Q/E: descer/subir | Mouse: olhar | R: reiniciar");
    if (exercise == 2)
        ImGui::Text("Cubos: %d/50 | Um cubo por segundo", ((generatedCubes - 1) % 50) + 1);
    if (!error.empty()) ImGui::TextWrapped("%s", error.c_str());
    ImGui::End();
}

void SceneTDE::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
    auto& input = InputManager::GetInstance();
    if (input.IsKeyJustPressed(GLFW_KEY_R)) Reset();
    camera.MovementSpeed = input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 16.0f : 8.0f;
    if (input.IsKeyPressed(GLFW_KEY_W)) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (input.IsKeyPressed(GLFW_KEY_S)) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (input.IsKeyPressed(GLFW_KEY_A)) camera.ProcessKeyboard(LEFT, deltaTime);
    if (input.IsKeyPressed(GLFW_KEY_D)) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (input.IsKeyPressed(GLFW_KEY_Q)) camera.ProcessKeyboard(DOWN, deltaTime);
    if (input.IsKeyPressed(GLFW_KEY_E)) camera.ProcessKeyboard(UP, deltaTime);
    auto mouse = input.GetMouseDelta();
    camera.ProcessMouseMovement(mouse.x, mouse.y);
    camera.ProcessMouseScroll(input.GetScrollOffset());
}

void SceneTDE::UploadHouse(const std::vector<float>& vertices)
{
    houseVertices = static_cast<GLsizei>(vertices.size() / 8);
    glGenVertexArrays(1, &houseVAO);
    glGenBuffers(1, &houseVBO);
    glBindVertexArray(houseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    for (GLuint i = 0; i < 3; ++i) glEnableVertexAttribArray(i);
    glBindVertexArray(0);
}

void SceneTDE::LoadHouse(const std::string& path)
{
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Nao foi possivel abrir " + path);
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::replace(contents.begin(), contents.end(), ',', ' ');
    std::istringstream stream(contents);
    std::vector<float> values;
    float value;
    while (stream >> value) values.push_back(value);
    if (!stream.eof() || values.empty() || values.size() % 15 != 0)
        throw std::runtime_error("Casa.txt possui dados de triangulos invalidos.");
    std::vector<float> vertices;
    for (size_t i = 0; i < values.size(); i += 15)
    {
        glm::vec3 a(values[i], values[i + 1], values[i + 2]);
        glm::vec3 b(values[i + 5], values[i + 6], values[i + 7]);
        glm::vec3 c(values[i + 10], values[i + 11], values[i + 12]);
        auto normal = glm::cross(b - a, c - a);
        normal = glm::length(normal) > 0 ? glm::normalize(normal) : glm::vec3(0, 1, 0);
        for (size_t j = i; j < i + 15; j += 5)
            vertices.insert(vertices.end(), { values[j], values[j + 1], values[j + 2], normal.x, normal.y, normal.z, values[j + 3], values[j + 4] });
    }
    UploadHouse(vertices);
}

void SceneTDE::CreateHouse()
{
    std::vector<float> vertices;
    auto face = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec2 low, glm::vec2 high)
    {
        auto normal = glm::normalize(glm::cross(b - a, c - a));
        glm::vec3 points[] = { a, b, c, a, c, d };
        glm::vec2 uv[] = { low, { high.x, low.y }, high, low, high, { low.x, high.y } };
        for (int i = 0; i < 6; ++i)
            vertices.insert(vertices.end(), { points[i].x, points[i].y, points[i].z, normal.x, normal.y, normal.z, uv[i].x, uv[i].y });
    };
    face({ -6, 0, 3 }, { 0, 0, 3 }, { 0, 4, 3 }, { -6, 4, 3 }, { 0.5f, 0 }, { 1, 0.5f });
    face({ 0, 0, 3 }, { 6, 0, 3 }, { 6, 4, 3 }, { 0, 4, 3 }, { 0, 0.5f }, { 0.5f, 1 });
    face({ -6, 0, -3 }, { -6, 0, 3 }, { -6, 4, 3 }, { -6, 4, -3 }, { 0.5f, 0 }, { 1, 0.5f });
    face({ 6, 0, 3 }, { 6, 0, -3 }, { 6, 4, -3 }, { 6, 4, 3 }, { 0, 0 }, { 0.5f, 0.5f });
    face({ 6, 0, -3 }, { -6, 0, -3 }, { -6, 4, -3 }, { 6, 4, -3 }, { 0, 0 }, { 0.5f, 0.5f });
    face({ -6.7f, 3.95f, 3.6f }, { 6.7f, 3.95f, 3.6f }, { 6.7f, 6, 0 }, { -6.7f, 6, 0 }, { 0.5f, 0.5f }, { 1, 1 });
    face({ 6.7f, 3.95f, -3.6f }, { -6.7f, 3.95f, -3.6f }, { -6.7f, 6, 0 }, { 6.7f, 6, 0 }, { 0.5f, 0.5f }, { 1, 1 });
    for (float x : { -6.0f, 6.0f })
    {
        glm::vec3 normal(x < 0 ? -1.0f : 1.0f, 0, 0);
        for (auto point : { glm::vec3(x, 4, -3), glm::vec3(x, 4, 3), glm::vec3(x, 6, 0) })
            vertices.insert(vertices.end(), { point.x, point.y, point.z, normal.x, normal.y, normal.z, 0.1f + (point.z + 3) / 30, 0.05f + (point.y - 4) / 12 });
    }
    UploadHouse(vertices);
}
