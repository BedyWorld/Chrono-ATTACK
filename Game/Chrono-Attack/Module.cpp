#include "Module.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

const std::vector<std::string> Module::name_prefixes_ = {
    "Quantum", "Plasma", "Nano", "Cyber", "Turbo",
"Hyper", "Mega", "Ultra", "Electro", "Neuro"
};

const std::vector<std::string> Module::name_suffixes_ = {
    "amplifier", "stabilizer", "controller", "modulator",
    "transformer", "battery", "reactor", "processor",
    "driver", "adapter"
};
// Реализация конструктора
Module::Module(int level) {
    generate_random_stats(level);
    name_ = generate_name();
}

// Реализация методов
std::string Module::generate_name() {
    std::string name = name_prefixes_[rand() % name_prefixes_.size()] + " " +
        name_suffixes_[rand() % name_suffixes_.size()];

    if (rand() % 100 < 30) {
        static const std::vector<std::string> prefixes = {
            
        };
        name = prefixes[rand() % prefixes.size()] + " " + name;
    }
    return name;
}

void Module::generate_random_stats(int level) {
    BonusType type = static_cast<BonusType>(rand() % 4);

    switch (type) {
    case BonusType::HEALTH:
        health_mod_ = (5 + rand() % 26) * (level * level);
        attack_mod_ = 0;
        ability_mod_ = 0;
        break;
    case BonusType::ATTACK:
        health_mod_ = 0;
        attack_mod_ = (5 + rand() % 26) * (level * level);
        ability_mod_ = 0;
        break;
    case BonusType::ABILITY:
        health_mod_ = 0;
        attack_mod_ = 0;
        ability_mod_ = (10 + rand() % 41) * (level * level);
        break;
    case BonusType::MIXED:
        health_mod_ = (3 + rand() % 13) * (level * level);
        attack_mod_ = (3 + rand() % 13) * (level * level);
        ability_mod_ = (5 + rand() % 16) * (level * level);
        break;
    }

    if (rand() % 100 < 20) {
        int stat = rand() % 3;
        int penalty = -(1 + rand() % 5);
        if (stat == 0)
            health_mod_ += penalty;
        else if (stat == 1)
            attack_mod_ += penalty;
        else
            ability_mod_ += penalty;
    }
}

void Module::display_info() const {
    std::cout << name_ << " (";
    bool first = true;

    if (health_mod_ != 0) {
        std::cout << "HP: " << health_mod_;
        first = false;
    }
    if (attack_mod_ != 0) {
        if (!first)
            std::cout << ", ";
        std::cout << "ATK: " << attack_mod_;
        first = false;
    }
    if (ability_mod_ != 0) {
        if (!first)
            std::cout << ", ";
        std::cout << "Ability: " << ability_mod_;
    }
    std::cout << ")";
}