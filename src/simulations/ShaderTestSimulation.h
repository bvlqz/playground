#pragma once
#include "../SimulationInterface.h"
#include <GLES3/gl3.h>
#include <string>

class ShaderTestSimulation : public SimulationInterface {
public:
    ShaderTestSimulation();
    ~ShaderTestSimulation();

    std::string name() const override { return "Shader Test"; }
    void init(int width, int height) override;
    void update(float dt) override;
    void render() override;
    ImTextureID texture() const override { return reinterpret_cast<ImTextureID>((intptr_t)renderTexture); }
    void ui() override;

private:
    GLuint renderTexture = 0;
    GLuint frameBuffer = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint shaderProgram = 0;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    float time = 0.0f;
    int width = 0;
    int height = 0;

    std::string fragmentShaderText;
    std::string errorLog;
    bool compileShader();
    char editorBuffer[16384] = {0}; // For ImGui text editor
    bool needsRecompile = false;

    float primaryColor[3] = {1.0f, 0.5f, 0.2f};  // RGB
    float secondaryColor[3] = {0.2f, 0.5f, 1.0f}; // RGB
};
