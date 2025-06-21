#include <iostream>
#include <cmath>
#include <algorithm>

#include "Robot.h"
#include "Module.h"

Robot::Robot(std::string name, int max_health, int base_attack, float base_sav, std::string ability_desc)
    : name_(name),
    max_health_(max_health),
    health_(max_health),
    base_attack_(base_attack),
    base_special_ability_value_(base_sav),
    current_special_ability_value_(base_sav),
    ability_description_(ability_desc),
    equipped_module_(nullptr),
    has_attack_buff_(false),
    attack_buff_percentage_(0),
    has_defense_buff_(false),
    defense_buff_percentage_(0),
    defense_shield_(0),
    virus_damage_per_turn_(0),
    virus_duration_(0) {
}

void Robot::equip_module(Module* module) {
    if (equipped_module_) {
        unequip_module();
    }
    equipped_module_ = module;
    apply_module_effects();
}

void Robot::unequip_module() {
    if (!equipped_module_) return;

    max_health_ = static_cast<int>(std::round(max_health_ / (1.0 + equipped_module_->health_mod_ / 50.0)));
    base_attack_ = static_cast<int>(std::round(base_attack_ / (1.0 + equipped_module_->attack_mod_ / 50.0)));
    current_special_ability_value_ = base_special_ability_value_;

    if (health_ > max_health_) health_ = max_health_;

    equipped_module_ = nullptr;
}

void Robot::apply_module_effects() {
    if (!equipped_module_) return;

    max_health_ = static_cast<int>(std::round(max_health_ * (100.0 + equipped_module_->health_mod_) / 50.0));
    base_attack_ = static_cast<int>(std::round(base_attack_ * (100.0 + equipped_module_->attack_mod_) / 50.0));
    current_special_ability_value_ = base_special_ability_value_ * (100.0 + equipped_module_->ability_mod_) / 66.0;

    if (health_ > max_health_) health_ = max_health_;
}

void Robot::reset_virus_effect() {
    virus_damage_per_turn_ = 0;
    virus_duration_ = 0;
}

bool Robot::heal(Robot& target, int current_game_level) {
    float heal_amount = current_special_ability_value_;
    int old_health = target.health_;
    target.health_ += static_cast<int>(heal_amount);
    if (target.health_ > target.max_health_) target.health_ = target.max_health_;

    std::cout << target.name_ << " has restored " << (target.health_ - old_health) << " HP! (" << old_health << " -> "
        << target.health_ << "/" << target.max_health_ << ")\n";

    return false;
}

bool Robot::is_alive() const { return health_ > 0; }

void Robot::take_damage(int damage) {
    if (has_defense_buff_) {
        damage = static_cast<int>(damage * (1.0f - defense_buff_percentage_ / 100.0f));
    }
    if (defense_shield_ > 0) {
        int absorbed = std::min(damage, defense_shield_);
        damage -= absorbed;
        defense_shield_ -= absorbed;
        std::cout << name_ << " aborbed " << absorbed << " damage with a barrier.\n";
        if (defense_shield_ == 0) {
            std::cout << name_ << " has lost the energy shield.\n";
        }
    }
    health_ -= damage;
    if (health_ < 0) health_ = 0;
}

void Robot::apply_virus_effect(Robot& enemy) {
    if (virus_duration_ > 0) {
        std::cout << enemy.name_ << " gains " << virus_damage_per_turn_ << " damage from Virus!\n";
        enemy.take_damage(virus_damage_per_turn_);
        virus_duration_--;
        if (virus_duration_ == 0) {
            std::cout << "Virus effect on " << enemy.name_ << " has expired.\n";
        }
    }
}

int Robot::calculate_damage() const {
    int damage = base_attack_;
    if (has_attack_buff_) {
        damage = static_cast<int>(damage * (1.0f + attack_buff_percentage_ / 100.0f));
    }
    return damage;
}

void Robot::display_stats() const {
    std::cout << name_ << ": HP=" << health_ << "/" << max_health_
        << ", ATK=" << base_attack_;
    if (has_attack_buff_)
        std::cout << "(+" << attack_buff_percentage_ << "%)";
    if (has_defense_buff_)
        std::cout << "(-" << defense_buff_percentage_ << "% урона)";

    if (name_ != "Evil drone") {
        std::cout << ", " << ability_description_ << "= " << current_special_ability_value_;
    }
    std::cout << "\n";
}

BattleResult Robot::use_special_ability(std::vector<Robot>& team, Robot* enemy, int current_game_level) {
    if (name_ == "Gobbie") {
        std::cout << "Choose a bot to heal:\n";
        std::vector<Robot*> alive_team_members;
        for (size_t i = 0; i < team.size(); ++i) {
            if (team[i].is_alive()) {
                alive_team_members.push_back(&team[i]);
                std::cout << alive_team_members.size() << ". " << team[i].name_
                    << " (HP: " << team[i].health_ << "/"
                    << team[i].max_health_ << ")\n";
            }
        }

        if (alive_team_members.empty()) {
            std::cout << "No alive bots left to heal.\n";
            return BattleResult::CONTINUE;
        }

        int choice;
        std::cout << "Type in a bot's number: ";
        std::cin >> choice;

        if (choice > 0 && choice <= alive_team_members.size()) {
            heal(*alive_team_members[choice - 1], current_game_level);
        }
        else {
            std::cout << "Incorrect target, heal missed.\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Bobbie") {
        if (enemy) {
            int damage = static_cast<int>(calculate_damage() * (1.1f + current_special_ability_value_ / 100.0f));
            std::cout << name_ << " is using a special ability and deals " << damage << " damage!\n";
            enemy->take_damage(damage);

            int self_damage = static_cast<int>(max_health_ * 0.1);
            health_ -= self_damage;
            if (health_ < 0) health_ = 0;
            std::cout << name_ << " loses " << self_damage << " HP from attacking! (" << health_ << "/" << max_health_ << ")\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Robbie") {
        int shield_value = static_cast<int>(std::round(current_special_ability_value_));
        for (auto& robot : team) {
            robot.defense_shield_ = shield_value;
            std::cout << robot.name_ << " has gain an electric shield giving " << shield_value << " of damage for 1 turn.\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Zombie") {
        if (!enemy) {
            std::cout << "No enemies are to be infected.\n";
            return BattleResult::CONTINUE;
        }

        int calculated_virus_damage = static_cast<int>(std::round(current_special_ability_value_));
        int calculated_virus_duration = static_cast<int>(std::round(current_special_ability_value_ / 5.0f));

        if (calculated_virus_duration < 1) calculated_virus_duration = 1;

        enemy->virus_damage_per_turn_ = calculated_virus_damage;
        enemy->virus_duration_ = calculated_virus_duration;

        std::cout << name_ << " has infected " << enemy->name_ << " with virus and deals " << calculated_virus_damage
            << " for " << calculated_virus_duration << " turn/s!\n";

        return BattleResult::CONTINUE;
    }

    return BattleResult::CONTINUE;
}

void Robot::reset_buffs() {
    has_attack_buff_ = false;
    has_defense_buff_ = false;
    attack_buff_percentage_ = 0;
    defense_buff_percentage_ = 0;
}