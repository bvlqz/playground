#include "GameOfLifeSimulation.h"
#include <cstdlib>
#include <ctime>
#include <imgui.h>

GameOfLifeSimulation::GameOfLifeSimulation() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

GameOfLifeSimulation::~GameOfLifeSimulation() {
    if (glTexture) glDeleteTextures(1, &glTexture);
}

void GameOfLifeSimulation::init(int w, int h) {
    width = w;
    height = h;
    current.resize(width * height);
    next.resize(width * height);
    randomize();

    // Create OpenGL texture
    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GameOfLifeSimulation::randomize() {
    for (auto& cell : current)
        cell = (std::rand() % 2) * 255;
}

void GameOfLifeSimulation::step() {
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
        int count = 0;
        for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + width) % width;
            int ny = (y + dy + height) % height;
            count += current[ny * width + nx] ? 1 : 0;
        }

        uint8_t& out = next[y * width + x];
        uint8_t alive = current[y * width + x];
        out = (count == 3 || (count == 2 && alive)) ? 255 : 0;
    }
    current.swap(next);
}

void GameOfLifeSimulation::uploadToTexture() {
    std::vector<uint8_t> rgba(width * height * 4);
    for (int i = 0; i < width * height; ++i) {
        uint8_t val = current[i];
        rgba[i * 4 + 0] = val;
        rgba[i * 4 + 1] = val;
        rgba[i * 4 + 2] = val;
        rgba[i * 4 + 3] = 255;
    }

    glBindTexture(GL_TEXTURE_2D, glTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
}

void GameOfLifeSimulation::update(float dt) {
    updateTimer += dt;
    if (updateTimer >= updateInterval) {
        updateTimer = 0.0f;
        step();
        uploadToTexture();
    }
}

void GameOfLifeSimulation::render() {
    // Nothing to draw actively; data is uploaded to texture in `update`
}

void GameOfLifeSimulation::ui() {
    ImGui::SliderFloat("Update Interval", &updateInterval, 0.01f, 1.0f, "%.2f s");
    if (ImGui::Button("Randomize")) randomize();
}