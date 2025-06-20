#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <cmath>
#include <locale>
#include <thread>
#include <chrono>

#include "GameConfig.h"
#include "Robot.h"
#include "GameMap.h"
#include "Inventory.h"
#include "BattleSystem.h"
#include "GameUtils.h"

//МЭйн функция игры
//Инициализирует игру, создает команду роботов, карту и инвентарь
//return 0 если победа 1 если поражение

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned>(time(0)));

    std::vector<Robot> team = {
        Robot("Бобби", 60, 20, 100, "Супер атака (2X урон, -10% HP) "),
        Robot("Робби", 80, 15, 10, "Защитный барьер команды "),
        Robot("Гобби", 50, 10, 15, "Лечение "),
        Robot("Зомби", 40, 25, 12, "Вирус ")
    };

    GameMap game_map;
    Inventory inventory;
    int current_level = 1;
    bool final_boss_triggered = false;

    while (true) {
        // проверка, живы ли роботы в команде
        if (std::all_of(team.begin(), team.end(), [](const Robot& r) { return !r.is_alive(); })) {
            std::cout << "\nВсе роботы уничтожены! Игра окончена.\n";
            return 1; // игрок проиграл
        }

        game_map.display();

        std::cout << "\nТекущие характеристики команды:\n";
        for (const auto& robot : team) {
            if (robot.is_alive()) {
                robot.display_stats();
            }
            else {
                std::cout << robot.name_ << " уничтожен\n";
            }
        }

        std::cout << "Текущий уровень: " << current_level << "\n";

        std::cout << "\nКоманды: w - вверх, s - вниз, a - влево, d - вправо, m - управление модулями, q - выход\n";
        char input;
        std::cin >> input;

        if (input == 'q') break;

        BattleResult room_check_result = BattleResult::CONTINUE;
        switch (input) {
        case 'w': room_check_result = game_map.move_player(-1, 0, team, inventory, current_level); break;
        case 's': room_check_result = game_map.move_player(1, 0, team, inventory, current_level); break;
        case 'a': room_check_result = game_map.move_player(0, -1, team, inventory, current_level); break;
        case 'd': room_check_result = game_map.move_player(0, 1, team, inventory, current_level); break;
        case 'm': manage_equipment(team, inventory); break;
        default: std::cout << "Неизвестная команда\n";
        }

        if (room_check_result == BattleResult::PLAYER_LOST) {
            return 1; // игрок проиграл
        }
        if (game_map.consume_final_boss_trigger()) {
            final_boss_triggered = true;
            break; //покдиаем основной гейм луп финальный босс вызван
        }
    }

    if (final_boss_triggered) {
        Robot final_boss("ХРОНО-ВЛАДЫКА", 1000, 70, 20, "Мега-Вирус");

        std::cout << "\n!!! БИТВА С ФИНАЛЬНЫМ БОССОМ: " << final_boss.name_ << " !!!\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        BattleSystem final_boss_battle(team, final_boss, current_level);
        BattleResult boss_outcome = final_boss_battle.start_battle();

        if (boss_outcome == BattleResult::PLAYER_WON) {
            std::cout << "\n*** ВЫ ПОБЕДИЛИ ХРОНО-ВЛАДЫКУ! ВЫ СПАСЛИ МИР! ***\n";
        }
        else {
            std::cout << "\n*** ВЫ БЫЛИ УНИЧТОЖЕНЫ ХРОНО-ВЛАДЫКОЙ. КОНЕЦ ИГРЫ. ***\n";
            return 1; // проигрыш фин босу
        }
    }
    else {
        std::cout << "\nИгра завершена.\n";
    }

    return 0;
}