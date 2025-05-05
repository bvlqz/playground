#pragma once
#include "SimulationInterface.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

class SimulationManager {
public:
    using Factory = std::function<std::unique_ptr<SimulationInterface>()>;

    void registerSimulation(const std::string& name, Factory factory);
    std::unique_ptr<SimulationInterface> create(int index) const;

    const std::vector<std::string>& getNames() const;

private:
    std::vector<std::string> names;
    std::vector<Factory> factories;
};