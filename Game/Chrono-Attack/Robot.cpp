#include "Robot.h"
#include "Module.h"
#include <iostream>
#include <cmath>
#include <algorithm>

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

    std::cout << target.name_ << " восстановил " << (target.health_ - old_health) << " HP! (" << old_health << " -> "
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
        std::cout << name_ << " поглотил " << absorbed << " урона энергетическим барьером.\n";
        if (defense_shield_ == 0) {
            std::cout << name_ << " потерял энергетический барьер.\n";
        }
    }
    health_ -= damage;
    if (health_ < 0) health_ = 0;
}

void Robot::apply_virus_effect(Robot& enemy) {
    if (virus_duration_ > 0) {
        std::cout << enemy.name_ << " получает " << virus_damage_per_turn_ << " урона от вируса!\n";
        enemy.take_damage(virus_damage_per_turn_);
        virus_duration_--;
        if (virus_duration_ == 0) {
            std::cout << "Эффект вируса на " << enemy.name_ << " закончился.\n";
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

    if (name_ != "Враждебный дрон") {
        std::cout << ", " << ability_description_ << "= " << current_special_ability_value_;
    }
    std::cout << "\n";
}

BattleResult Robot::use_special_ability(std::vector<Robot>& team, Robot* enemy, int current_game_level) {
    if (name_ == "Гобби") {
        std::cout << "Выберите робота для лечения:\n";
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
            std::cout << "Нет живых роботов для лечения.\n";
            return BattleResult::CONTINUE;
        }

        int choice;
        std::cout << "Введите номер робота: ";
        std::cin >> choice;

        if (choice > 0 && choice <= alive_team_members.size()) {
            heal(*alive_team_members[choice - 1], current_game_level);
        }
        else {
            std::cout << "Неверный выбор цели. Лечение пропущено.\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Бобби") {
        if (enemy) {
            int damage = static_cast<int>(calculate_damage() * (1.1f + current_special_ability_value_ / 100.0f));
            std::cout << name_ << " использует супер атаку и наносит " << damage << " урона!\n";
            enemy->take_damage(damage);

            int self_damage = static_cast<int>(max_health_ * 0.1);
            health_ -= self_damage;
            if (health_ < 0) health_ = 0;
            std::cout << name_ << " теряет " << self_damage << " HP от отдачи! (" << health_ << "/" << max_health_ << ")\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Робби") {
        int shield_value = static_cast<int>(std::round(current_special_ability_value_));
        for (auto& robot : team) {
            robot.defense_shield_ = shield_value;
            std::cout << robot.name_ << " получил энергетический барьер, поглощающий " << shield_value << " урона на 1 ход.\n";
        }
        return BattleResult::CONTINUE;
    }
    else if (name_ == "Зомби") {
        if (!enemy) {
            std::cout << "Нет цели для заражения вирусом.\n";
            return BattleResult::CONTINUE;
        }

        int calculated_virus_damage = static_cast<int>(std::round(current_special_ability_value_));
        int calculated_virus_duration = static_cast<int>(std::round(current_special_ability_value_ / 5.0f));

        if (calculated_virus_duration < 1) calculated_virus_duration = 1;

        enemy->virus_damage_per_turn_ = calculated_virus_damage;
        enemy->virus_duration_ = calculated_virus_duration;

        std::cout << name_ << " заразил " << enemy->name_ << " вирусом с уроном " << calculated_virus_damage
            << " на " << calculated_virus_duration << " ход(ов)!\n";

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