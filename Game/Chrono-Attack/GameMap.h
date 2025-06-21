#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "GameConfig.h"
#include <vector>

class Robot;
class Inventory;
class Module;

class GameMap {
private:
    struct Room {
        bool has_enemy;
        Module* module;
        Room() : has_enemy(false), module(nullptr) {}
    };

    Room map_[MAP_SIZE][MAP_SIZE];
    int player_x_;
    int player_y_;
    bool final_boss_triggered_this_turn_;

    void initialize_room_content(int x, int y, int level);
    bool is_map_clear() const;

public:
    GameMap();
    ~GameMap();

    void reset_map(int level);
    bool consume_final_boss_trigger();
    BattleResult move_player(int dx, int dy, std::vector<Robot>& team, Inventory& inventory, int& level);
    BattleResult check_room(std::vector<Robot>& team, Inventory& inventory, int& level);
    void display() const;
};

#endif