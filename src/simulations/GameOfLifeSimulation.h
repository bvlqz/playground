#pragma once
#include "SimulationInterface.h"
#include <vector>
#include <GLES3/gl3.h>

class GameOfLifeSimulation : public SimulationInterface {
public:
    GameOfLifeSimulation();
    ~GameOfLifeSimulation();

    std::string name() const override { return "Game of Life"; }
    void init(int width, int height) override;
    void update(float dt) override;
    void render() override;
    ImTextureID texture() const override { return reinterpret_cast<ImTextureID>((intptr_t)glTexture); }
    void ui() override;

private:
    int width = 0;
    int height = 0;
    std::vector<uint8_t> current;
    std::vector<uint8_t> next;

    GLuint glTexture = 0;
    float updateTimer = 0.0f;
    float updateInterval = 0.1f; // in seconds

    void step();
    void randomize();
    void uploadToTexture();
};