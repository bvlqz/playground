#pragma once
#include <SDL.h>
#include "SimulationInterface.h"
class App {
public:
    bool init();
    void run();
    void render();

private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    int windowWidth = 1920;
    int windowHeight = 1080;
    int frameBufferWidth = 1920;  // Added
    int frameBufferHeight = 1080; // Added
    float dpiScale = 1.0f;
    float contentScale = 1.0f;    // Added

    void updateViewport();
    void handleResize(int width, int height);
    void handleEvents();

    void setupImGuiStyle();
    void setupImGuiFonts();

    int currentSimulationIndex = 0;
    std::unique_ptr<SimulationInterface> simulation;
};