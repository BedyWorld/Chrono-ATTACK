#include "GameUtils.h"
#include "Robot.h"
#include "Inventory.h"
#include "Module.h"
#include <iostream>

void manage_equipment(std::vector<Robot>& team, Inventory& inventory) {
    while (true) {
        std::cout << "\nModule management:\n";
        std::cout << "1. Show inventory\n";
        std::cout << "2. Equip module\n";
        std::cout << "3. Tale off module\n";
        std::cout << "4. Go back into the game\n";
        std::cout << "Make a choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            inventory.display();
        }
        else if (choice == 2) {
            if (inventory.size() == 0) {
                std::cout << "Inventory is empty!\n";
                continue;
            }

            std::cout << "Choose a bot:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                std::cout << i + 1 << ". " << team[i].name_;
                if (team[i].equipped_module_) {
                    std::cout << " (is equipped " << team[i].equipped_module_->name_ << ")";
                }
                std::cout << "\n";
            }
            int robot_choice;
            std::cin >> robot_choice;

            if (robot_choice < 1 || robot_choice > team.size()) {
                std::cout << "Incorrect bot choice!\n";
                continue;
            }

            inventory.display();
            std::cout << "choose a module to equip (0) - cancel ";
            int module_choice;
            std::cin >> module_choice;
            if (module_choice == 0) continue;

            Module* module = inventory.take_module(module_choice);
            if (module) {
                team[robot_choice - 1].equip_module(module);
                std::cout << team[robot_choice - 1].name_ << " has eqipped ";
                module->display_info();
                std::cout << ".\n";
            }
            else {
                std::cout << "Incorrect module choice!\n";
            }
        }
        else if (choice == 3) {
            std::cout << "Choose a bot to take off the module:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                std::cout << i + 1 << ". " << team[i].name_;
                if (team[i].equipped_module_) {
                    std::cout << " (is equipped " << team[i].equipped_module_->name_ << ")";
                }
                else {
                    std::cout << " (no module)";
                }
                std::cout << "\n";
            }

            int robot_choice;
            std::cin >> robot_choice;

            if (robot_choice < 1 || robot_choice > team.size()) {
                std::cout << "Wrong robot choice!\n";
                continue;
            }

            if (team[robot_choice - 1].equipped_module_) {
                Module* unequipped_module = team[robot_choice - 1].equipped_module_;
                team[robot_choice - 1].unequip_module();
                inventory.add_module(unequipped_module);
                std::cout << team[robot_choice - 1].name_ << "has taken off the module ";
                unequipped_module->display_info();
                std::cout << ".\n";
            }
            else {
                std::cout << "This robot loves salad after it yells!\n";
            }
        }
        else if (choice == 4) {
            break;
        }
        else {
            std::cout << "Wrong choice!\n";
        }
        std::cout << "\nUpdated team stats:\n";
        for (const auto& robot : team) {
            robot.display_stats();
        }
    }
}