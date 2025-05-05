#include "App.h"
#include <functional>  // std::function

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#define IMGUI_IMPL_OPENGL_ES3
#include <backends/imgui_impl_opengl3.h>

#include <cstdio>


#include "SimulationInterface.h"
#include "GameOfLifeSimulation.h"
#include <memory>
#include <vector>
#include <string>


static App* g_app = nullptr;

// Simulation
std::vector<std::function<std::unique_ptr<SimulationInterface>()>> simulationFactories = {
    []() { return std::make_unique<GameOfLifeSimulation>(); },
    // Future simulations go here
};

std::vector<std::string> simulationNames = {
    "Game of Life",
    // Future names go here
};

int currentSimulationIndex = 0;
std::unique_ptr<SimulationInterface> simulation = nullptr;
// End of simulation


bool App::init() {
    g_app = this;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

    window = SDL_CreateWindow("Interactive DSA Playground",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1280, 720,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    updateViewport();
    return true;
}

void App::run() {
    simulation = simulationFactories[currentSimulationIndex]();
    simulation->init(256, 256); // resolution of the sim texture

    emscripten_set_main_loop([] { g_app->render(); }, 0, true);
}

void App::updateViewport() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    dpiScale = (float)windowWidth / (float)w;
}

void App::handleResize(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    updateViewport();
}

void App::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            emscripten_cancel_main_loop();
        else if (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_RESIZED)
            handleResize(event.window.data1, event.window.data2);
    }
}

void App::render() {
    handleEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();


    float dt = ImGui::GetIO().DeltaTime;
    simulation->update(dt);
    simulation->render();
    // --- Your simulation rendering & UI would go here ---
    ImGui::Begin("Playground");
    ImGui::Text("Hello from App!");

    if (ImGui::BeginCombo("Simulation", simulationNames[currentSimulationIndex].c_str())) {
        for (int i = 0; i < simulationNames.size(); ++i) {
            bool selected = (i == currentSimulationIndex);
            if (ImGui::Selectable(simulationNames[i].c_str(), selected)) {
                currentSimulationIndex = i;
                simulation = simulationFactories[i]();
                simulation->init(256, 256);
            }
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    simulation->ui();
    ImGui::Image(simulation->texture(), ImVec2(256, 256));
    ImGui::End();
    // -----------------------------------------------------

    ImGui::Render();
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}