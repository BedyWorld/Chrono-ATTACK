#include "GameUtils.h"
#include "Robot.h"
#include "Inventory.h"
#include "Module.h"
#include <iostream>

void manage_equipment(std::vector<Robot>& team, Inventory& inventory) {
    while (true) {
        std::cout << "\nУправление модулями:\n";
        std::cout << "1. Показать инвентарь\n";
        std::cout << "2. Экипировать модуль\n";
        std::cout << "3. Снять модуль\n";
        std::cout << "4. Вернуться в игру\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            inventory.display();
        }
        else if (choice == 2) {
            if (inventory.size() == 0) {
                std::cout << "Инвентарь пуст!\n";
                continue;
            }

            std::cout << "Выберите робота:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                std::cout << i + 1 << ". " << team[i].name_;
                if (team[i].equipped_module_) {
                    std::cout << " (экипирован " << team[i].equipped_module_->name_ << ")";
                }
                std::cout << "\n";
            }
            int robot_choice;
            std::cin >> robot_choice;

            if (robot_choice < 1 || robot_choice > team.size()) {
                std::cout << "Неверный выбор робота!\n";
                continue;
            }

            inventory.display();
            std::cout << "Выберите модуль для экипировки (0 - отмена): ";
            int module_choice;
            std::cin >> module_choice;
            if (module_choice == 0) continue;

            Module* module = inventory.take_module(module_choice);
            if (module) {
                team[robot_choice - 1].equip_module(module);
                std::cout << team[robot_choice - 1].name_ << " экипировал ";
                module->display_info();
                std::cout << ".\n";
            }
            else {
                std::cout << "Неверный выбор модуля!\n";
            }
        }
        else if (choice == 3) {
            std::cout << "Выберите робота для снятия модуля:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                std::cout << i + 1 << ". " << team[i].name_;
                if (team[i].equipped_module_) {
                    std::cout << " (экипирован " << team[i].equipped_module_->name_ << ")";
                }
                else {
                    std::cout << " (нет модуля)";
                }
                std::cout << "\n";
            }

            int robot_choice;
            std::cin >> robot_choice;

            if (robot_choice < 1 || robot_choice > team.size()) {
                std::cout << "Неверный выбор робота!\n";
                continue;
            }

            if (team[robot_choice - 1].equipped_module_) {
                Module* unequipped_module = team[robot_choice - 1].equipped_module_;
                team[robot_choice - 1].unequip_module();
                inventory.add_module(unequipped_module);
                std::cout << team[robot_choice - 1].name_ << " снял модуль ";
                unequipped_module->display_info();
                std::cout << ".\n";
            }
            else {
                std::cout << "У этого робота нет модуля!\n";
            }
        }
        else if (choice == 4) {
            break;
        }
        else {
            std::cout << "Неверный выбор!\n";
        }
        std::cout << "\nОбновленные характеристики команды:\n";
        for (const auto& robot : team) {
            robot.display_stats();
        }
    }
}