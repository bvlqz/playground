#include <SDL.h>
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#define IMGUI_IMPL_OPENGL_ES3
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <string>

SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;

int windowWidth = 1920;
int windowHeight = 1080;
float dpiScale = 1.0f;

void updateViewport() {
    int width, height;
    float scale_x, scale_y;
    SDL_GetWindowSize(window, &width, &height);
    SDL_GL_GetDrawableSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Calculate DPI scale
    scale_x = (float)windowWidth / (float)width;
    scale_y = (float)windowHeight / (float)height;
    dpiScale = scale_x; // We'll use x scale as our DPI scale
}

void handle_resize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    updateViewport();
}

void render_frame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            emscripten_cancel_main_loop();
        }
        else if (event.type == SDL_WINDOWEVENT && 
                 event.window.event == SDL_WINDOWEVENT_RESIZED) {
            handle_resize(event.window.data1, event.window.data2);
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Clear the background
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Demo window with proper scaling (using local scaling instead of global)
    ImGui::Begin("TEST Playground");
    ImGui::SetWindowFontScale(1.0f); // Reset to default scale
    ImGui::Text("Hello World1.");
    ImGui::Text("Window size: %d x %d (Native Scale: %.2f)", windowWidth, windowHeight, dpiScale);
    ImGui::Text("DPI Scale: %.2f", dpiScale);
    ImGui::End();

    // ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    // Ensure WebGL2 / OpenGL ES 3 context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Set high DPI awareness
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    
    window = SDL_CreateWindow(
        "Interactive DSA Playground", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        1280, 720, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Force font scaling to 1.0
    io.FontGlobalScale = 1.0f;
    
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // Initial viewport setup
    updateViewport();

    emscripten_set_main_loop(render_frame, 0, true);

    // Clean up (never reached in practice)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}