#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <cstddef> // для size_t

// Предварительное объявление
class Module;

class Inventory {
private:
    std::vector<Module*> modules_;

public:
    // Деструктор для очистки модулей
    ~Inventory();

    // Добавить модуль в инвентарь
    void add_module(Module* module);

    // Отобразить содержимое инвентаря
    void display() const;

    // Взять модуль из инвентаря по индексу
    Module* take_module(size_t index);

    // Получить количество модулей в инвентаре
    size_t size() const;
};

#endif