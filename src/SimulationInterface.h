// SimulationInterface.h
#pragma once
#include <string>
#include <imgui.h>

class SimulationInterface {
public:
    virtual ~SimulationInterface() = default;

    virtual std::string name() const = 0;

    // Called once to initialize (e.g., texture setup, CPU grid alloc)
    virtual void init(int width, int height) = 0;

    // Called every frame to update simulation state
    virtual void update(float dt) = 0;

    // Renders to an ImGui::Image-compatible texture
    virtual void render() = 0;

    // Returns ImTextureID for ImGui::Image
    virtual ImTextureID texture() const = 0;

    // Optional ImGui controls per simulation
    virtual void ui() {}
};