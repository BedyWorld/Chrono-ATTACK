#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <cstddef>

class Module;

class Inventory {
private:
    std::vector<Module*> modules_;

public:
    // Деструктор для очистки модулей
    ~Inventory();

    void add_module(Module* module);

    void display() const;

    Module* take_module(size_t index);

    size_t size() const;
};

#endif