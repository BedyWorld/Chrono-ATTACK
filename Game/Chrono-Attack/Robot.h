#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <vector>
#include "GameConfig.h"

class Module;

class Robot {
public:
    std::string name_;
    int max_health_;
    int health_;
    int base_attack_;
    float base_special_ability_value_;
    float current_special_ability_value_;
    std::string ability_description_;
    Module* equipped_module_;
    bool has_attack_buff_;
    float attack_buff_percentage_;
    bool has_defense_buff_;
    float defense_buff_percentage_;
    int defense_shield_;
    int virus_damage_per_turn_;
    int virus_duration_;

    Robot(std::string name, int max_health, int base_attack, float base_sav, std::string ability_desc);

    void equip_module(Module* module);
    void unequip_module();
    void apply_module_effects();

    void reset_virus_effect();
    void apply_virus_effect(Robot& enemy);
    void reset_buffs();

    bool heal(Robot& target, int current_game_level);
    void take_damage(int damage);
    int calculate_damage() const;
    BattleResult use_special_ability(std::vector<Robot>& team, Robot* enemy = nullptr, int current_game_level = 0);

    bool is_alive() const;
    void display_stats() const;
};

#endif