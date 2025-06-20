#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include "GameConfig.h" // для BonusType

class Module {
private:
    // Списки для генерации имени
    static const std::vector<std::string> name_prefixes_;
    static const std::vector<std::string> name_suffixes_;

    // Генерирует случайное имя для модуля
    std::string generate_name();
    // Генерирует случайные характеристики в зависимости от уровня
    void generate_random_stats(int level);

public:
    std::string name_;
    int health_mod_;
    int attack_mod_;
    int ability_mod_;

    // Конструктор, создающий модуль для определенного уровня
    Module(int level);

    // Отображает информацию о модуле
    void display_info() const;
};

#endif