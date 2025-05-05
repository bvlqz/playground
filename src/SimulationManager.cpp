#include "SimulationManager.h"

void SimulationManager::registerSimulation(const std::string& name, Factory factory) {
    names.push_back(name);
    factories.push_back(factory);
}

std::unique_ptr<SimulationInterface> SimulationManager::create(int index) const {
    if (index >= 0 && index < factories.size()) {
        return factories[index]();
    }
    return nullptr;
}

const std::vector<std::string>& SimulationManager::getNames() const {
    return names;
}