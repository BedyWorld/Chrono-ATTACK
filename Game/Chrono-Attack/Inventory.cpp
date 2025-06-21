#include "Inventory.h"
#include "Module.h"
#include <iostream>

Inventory::~Inventory() {
    for (auto module : modules_) {
        delete module;
    }
}

void Inventory::add_module(Module* module) {
    modules_.push_back(module);
    std::cout << "Module added: ";
    module->display_info();
    std::cout << std::endl;
}

void Inventory::display() const {
    if (modules_.empty()) {
        std::cout << "Inventory is empty\n";
        return;
    }

    std::cout << "Module inventory:\n";
    for (size_t i = 0; i < modules_.size(); ++i) {
        std::cout << i + 1 << ". ";
        modules_[i]->display_info();
        std::cout << std::endl;
    }
}

Module* Inventory::take_module(size_t index) {
    if (index > 0 && index <= modules_.size()) {
        Module* module = modules_[index - 1];
        modules_.erase(modules_.begin() + index - 1);
        return module;
    }
    return nullptr;
}

size_t Inventory::size() const { return modules_.size(); }