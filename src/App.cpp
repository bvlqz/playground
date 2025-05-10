#include "App.h"
#include <functional>  // std::function

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#define IMGUI_IMPL_OPENGL_ES3
#include <backends/imgui_impl_opengl3.h>


#include <cstdio>

#include "SimulationManager.h"

#include "GameOfLifeSimulation.h"
#include "simulations/ShaderTestSimulation.h"
#include <memory>
#include <vector>
#include <string>


static App* g_app = nullptr;
static SimulationManager simManager;

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
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    
    window = SDL_CreateWindow("Playground",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1280, 720,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | 
                              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Configure ImGui style for high DPI
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpiScale);
    
    // Load fonts with higher resolution
    float baseFontSize = 16.0f;
    io.Fonts->Clear();
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", baseFontSize * dpiScale);
    io.FontGlobalScale = 1.0f; // We handle scaling in the font size itself

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    simManager.registerSimulation("Game of Life", [] {
        return std::make_unique<GameOfLifeSimulation>();
    });
    
    simManager.registerSimulation("Shader Test", [] {
        return std::make_unique<ShaderTestSimulation>();
    });

    updateViewport();
    return true;
}

void App::run() {
    simulation = simManager.create(currentSimulationIndex);
    simulation->init(256, 256); // resolution of the sim texture

    emscripten_set_main_loop([] { g_app->render(); }, 0, true);
}

void App::updateViewport() {
    int w, h, fbW, fbH;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &fbW, &fbH);
    
    windowWidth = w;
    windowHeight = h;
    frameBufferWidth = fbW;
    frameBufferHeight = fbH;
    
    dpiScale = (float)fbW / (float)w;
    contentScale = dpiScale;
    
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);
    
    // Update ImGui scale
    // ImGuiStyle& style = ImGui::GetStyle();
    // style = ImGui::GetStyle();
    // style.ScaleAllSizes(contentScale);
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

        // Update keyboard modifier state for ImGui
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        ImGuiIO& io = ImGui::GetIO();
        io.KeyCtrl = keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL];
        io.KeyShift = keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT];
        io.KeyAlt = keystate[SDL_SCANCODE_LALT] || keystate[SDL_SCANCODE_RALT];
        io.KeySuper = keystate[SDL_SCANCODE_LGUI] || keystate[SDL_SCANCODE_RGUI];

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
    
    // Scale the rendering to match the framebuffer
    ImGui::GetIO().DisplayFramebufferScale = ImVec2(dpiScale, dpiScale);

    float dt = ImGui::GetIO().DeltaTime;
    simulation->update(dt);
    simulation->render();
    // --- Your simulation rendering & UI would go here ---
    ImGui::Begin("Playground");

    const auto& names = simManager.getNames();
    if (ImGui::BeginCombo("Simulation", names[currentSimulationIndex].c_str())) {
        for (int i = 0; i < names.size(); ++i) {
            bool selected = (i == currentSimulationIndex);
            if (ImGui::Selectable(names[i].c_str(), selected)) {
                currentSimulationIndex = i;
                simulation = simManager.create(i);
                simulation->init(256, 256);
            }
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    simulation->ui();
    ImGui::Image(simulation->texture(), ImVec2(800, 800));
    ImGui::End();
    // -----------------------------------------------------

    // Render Demo
    ImGui::ShowDemoWindow();

    ImGui::Render();
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}