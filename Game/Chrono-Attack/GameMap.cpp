#include "GameMap.h"
#include "Robot.h"
#include "Inventory.h"
#include "Module.h"
#include "BattleSystem.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

GameMap::GameMap() : player_x_(0), player_y_(0), final_boss_triggered_this_turn_(false) {
    reset_map(1);
}

GameMap::~GameMap() {
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if (map_[x][y].module) {
                delete map_[x][y].module;
                map_[x][y].module = nullptr;
            }
        }
    }
}

void GameMap::reset_map(int level) {
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if (map_[x][y].module) {
                delete map_[x][y].module;
                map_[x][y].module = nullptr;
            }
            map_[x][y].has_enemy = false;
        }
    }

    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if (x == 0 && y == 0) { // стартовая комната всегда пустая
                continue;
            }
            initialize_room_content(x, y, level);
        }
    }
    player_x_ = 0;
    player_y_ = 0;
    std::cout << "\nNew map is generated for the levels " << level << "!\n";
}

void GameMap::initialize_room_content(int x, int y, int level) {
    map_[x][y].has_enemy = (rand() % 100 < (20 + level * 10));
    if (map_[x][y].has_enemy) {
        map_[x][y].module = nullptr;
    }
    else {
        if (rand() % 100 < (20 + level * 5)) {
            map_[x][y].module = new Module(level);
        }
        else {
            map_[x][y].module = nullptr;
        }
    }
}

bool GameMap::is_map_clear() const {
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if ((x != player_x_ || y != player_y_) && (map_[x][y].has_enemy || map_[x][y].module)) {
                return false;
            }
        }
    }
    if (map_[player_x_][player_y_].has_enemy || map_[player_x_][player_y_].module) {
        return false; // текущая комната не очищена
    }
    return true;
}

bool GameMap::consume_final_boss_trigger() {
    bool triggered = final_boss_triggered_this_turn_;
    final_boss_triggered_this_turn_ = false;
    return triggered;
}

BattleResult GameMap::move_player(int dx, int dy, std::vector<Robot>& team, Inventory& inventory, int& level) {
    int new_x = player_x_ + dx;
    int new_y = player_y_ + dy;

    if (new_x >= 0 && new_x < MAP_SIZE && new_y >= 0 && new_y < MAP_SIZE) {
        player_x_ = new_x;
        player_y_ = new_y;
        return check_room(team, inventory, level);
    }
    else {
        std::cout << "YOu can't go out of bounds!\n";
        return BattleResult::CONTINUE;
    }
}

BattleResult GameMap::check_room(std::vector<Robot>& team, Inventory& inventory, int& level) {
    bool room_content_changed = false;
    final_boss_triggered_this_turn_ = false;

    if (map_[player_x_][player_y_].has_enemy) {
        std::cout << "\nYou've stumbled upon an enemy!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Robot enemy("Evil drone", 50 * level, 15 + (level * 2), 0, "Attack");
        BattleSystem battle(team, enemy, level);

        BattleResult battle_outcome = battle.start_battle();

        if (battle_outcome == BattleResult::PLAYER_WON) {
            map_[player_x_][player_y_].has_enemy = false;
            room_content_changed = true;
        }
        else if (battle_outcome == BattleResult::PLAYER_LOST) {
            std::cout << "All the robots were defeated.\n";
            return BattleResult::PLAYER_LOST;
        }
    }
    else if (map_[player_x_][player_y_].module) {
        inventory.add_module(map_[player_x_][player_y_].module);
        map_[player_x_][player_y_].module = nullptr;
        room_content_changed = true;
        std::cout << "You've found a module!\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (room_content_changed && is_map_clear()) {
        if (level == FINAL_LEVEL) {
            final_boss_triggered_this_turn_ = true;
            std::cout << "\nAll of the map is clear, time for a special guest!\n";
            return BattleResult::FINAL_BOSS_TRIGGERED;
        }
        else {
            std::cout << "\nAll of the map is clear, time for a next level!\n";
            ++level;
            reset_map(level);
        }
    }
    return BattleResult::CONTINUE;
}

void GameMap::display() const {
    system("cls");
    std::cout << "\nКарта:\n";
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if (x == player_x_ && y == player_y_) {
                std::cout << "[P]";
            }
            else if (map_[x][y].has_enemy) {
                std::cout << "[E]";
            }
            else if (map_[x][y].module) {
                std::cout << "[M]";
            }
            else {
                std::cout << "[ ]";
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "P - You, E - Enemy, M - Module, [ ] - Empty cell\n";
}