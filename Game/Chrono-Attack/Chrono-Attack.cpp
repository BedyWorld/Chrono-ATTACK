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

//размер карты
const int MAP_SIZE = 5;

//Кол-во уровней до босса

const int FINAL_LEVEL = 3;

enum class BonusType {
    HEALTH,   //Бонус здоровья
    ATTACK,   //Бонус атаки
    ABILITY,  //бонус специальной способности
    MIXED     //смешанный бонус (здоровье, атака, способность)
};

//класс состояния битвы
enum class BattleResult {
    CONTINUE,             //Битва идёт
    PLAYER_WON,           //Победа
    PLAYER_LOST,          //Поажение
    FINAL_BOSS_TRIGGERED  //Финальный босс вызван
};
class Module {
private:

    static const std::vector<std::string> name_prefixes_;
    static const std::vector<std::string> name_suffixes_;

    //генерирует случайное имя модуля, комбинируя префиксы и суффиксы

    std::string generate_name() {
        std::string name = name_prefixes_[rand() % name_prefixes_.size()] + " " +
            name_suffixes_[rand() % name_suffixes_.size()];

        if (rand() % 100 < 30) {
            static const std::vector<std::string> prefixes = {
                "Прототип", "Улучшенный", "Боевой", "Элитный", "Экспериментальный"
            };
            name = prefixes[rand() % prefixes.size()] + " " + name;
        }
        return name;
    }
    void generate_random_stats(int level) {
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

public:
    std::string name_;         //название модуля.
    int health_mod_;           //очки здоровья, предоставляемые модулем.
    int attack_mod_;           //урон, предоставляемый модулем.
    int ability_mod_;          //очки специальной способности, предоставляемые модулем.

    Module(int level) {
        generate_random_stats(level);
        name_ = generate_name();
    }

    void display_info() const {
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
};

const std::vector<std::string> Module::name_prefixes_ = {
    "Квантовый", "Плазменный", "Нано", "Кибер", "Турбо",
    "Гипер", "Мега", "Ультра", "Электро", "Нейро"
};

const std::vector<std::string> Module::name_suffixes_ = {
    "усилитель", "стабилизатор", "контроллер", "модулятор",
    "трансформер", "аккумулятор", "реактор", "процессор",
    "драйвер", "адаптер"
};

class Robot {
public:    std::string name_;                     //Имя робота.
      int max_health_;                       //Максимальное количество здоровья робота.
      int health_;                           //Текущее количество здоровья робота.
      int base_attack_;                      //Основной урон, который робот наносит при атаке.
      float base_special_ability_value_;     //Основное значение специальной способности робота.
      float current_special_ability_value_;  //Текущее значение специальной способности робота.
      std::string ability_description_;      //Описание специальной способности робота.
      Module* equipped_module_;              //Указатель на модуль, экипированный роботом.
      bool has_attack_buff_;                 //Истинно, если у робота есть бафф атаки.
      float attack_buff_percentage_;         //Процент увеличения урона из-за баффа.
      bool has_defense_buff_;                //Истинно, если у робота есть бафф защиты.
      float defense_buff_percentage_;        //Процент уменьшения урона из-за баффа защиты.

      int defense_shield_;                   //Количество урона, которое поглощает энергетический барьер робота.
      int virus_damage_per_turn_;            //Урон, который робот получает от вируса за ход.
      int virus_duration_;                   //Длительность действия вируса в ходах.

      Robot(std::string name, int max_health, int base_attack, float base_sav, std::string ability_desc)
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

      //наделяет робота модулем, увеличивая его характеристики
      void equip_module(Module* module) {
          if (equipped_module_) {
              unequip_module();
          }
          equipped_module_ = module;
          apply_module_effects();
      }

      // Снимает модуль с робота, восстанавливая базовые характеристики
      void unequip_module() {
          if (!equipped_module_) return;

          max_health_ = static_cast<int>(std::round(max_health_ / (1.0 + equipped_module_->health_mod_ / 50.0)));
          base_attack_ = static_cast<int>(std::round(base_attack_ / (1.0 + equipped_module_->attack_mod_ / 50.0)));
          current_special_ability_value_ = base_special_ability_value_;

          if (health_ > max_health_) health_ = max_health_;

          equipped_module_ = nullptr;
      }

      void apply_module_effects() {
          if (!equipped_module_) return;

          max_health_ = static_cast<int>(std::round(max_health_ * (100.0 + equipped_module_->health_mod_) / 50.0));
          base_attack_ = static_cast<int>(std::round(base_attack_ * (100.0 + equipped_module_->attack_mod_) / 50.0));
          current_special_ability_value_ = base_special_ability_value_ * (100.0 + equipped_module_->ability_mod_) / 66.0;

          if (health_ > max_health_) health_ = max_health_;
      }

      //сбрасывает эффекты вируса, если они активны
      void reset_virus_effect() {
          virus_damage_per_turn_ = 0;
          virus_duration_ = 0;
      }

      //лечит робота, восстанавливая ему здоровье
      bool heal(Robot& target, int current_game_level) {
          float heal_amount = current_special_ability_value_;
          int old_health = target.health_;
          target.health_ += static_cast<int>(heal_amount);
          if (target.health_ > target.max_health_) target.health_ = target.max_health_;

          std::cout << target.name_ << " восстановил " << (target.health_ - old_health) << " HP! (" << old_health << " -> "
              << target.health_ << "/" << target.max_health_ << ")\n";

          return false;
      }

      // Проверяет, жив ли робот
      bool is_alive() const { return health_ > 0; }

      // наносит урон роботу, учитывая баффы и щиты

      void take_damage(int damage) {
          if (has_defense_buff_) {
              damage = static_cast<int>(damage * (1.0f - defense_buff_percentage_ / 100.0f));
          }
          if (defense_shield_ > 0) {
              int absorbed = std::min(damage, defense_shield_);
              damage -= absorbed;
              defense_shield_ -= absorbed;
              std::cout << name_ << " поглотил " << absorbed << " урона энергетическим барьером.\n";            if (defense_shield_ == 0) {
                  std::cout << name_ << " потерял энергетический барьер.\n";
              }
          }
          health_ -= damage;
          if (health_ < 0) health_ = 0;
      }

      //подсчитывает урон от вируса, если он активен
      void apply_virus_effect(Robot& enemy) {
          if (virus_duration_ > 0) {
              std::cout << enemy.name_ << " получает " << virus_damage_per_turn_ << " урона от вируса!\n";
              enemy.take_damage(virus_damage_per_turn_);
              virus_duration_--;
              if (virus_duration_ == 0) {
                  std::cout << "Эффект вируса на " << enemy.name_ << " закончился.\n";
              }
          }
      }

      // Подсчитывает урон, который робот может нанести в бою.
      int calculate_damage() const {
          int damage = base_attack_;
          if (has_attack_buff_) {
              damage = static_cast<int>(damage * (1.0f + attack_buff_percentage_ / 100.0f));
          }
          return damage;
      }

      void display_stats() const {
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

      //Эффект зависит от имени робота: Гобби лечит союзника, Робби применяет щит, 
      //Бобби наносит урон врагу с отдачей, Зомби заражает врага вирусом.
      BattleResult use_special_ability(std::vector<Robot>& team, Robot* enemy = nullptr, int current_game_level = 0) {
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

      void reset_buffs() {
          has_attack_buff_ = false;
          has_defense_buff_ = false;
          attack_buff_percentage_ = 0;
          defense_buff_percentage_ = 0;
      }
};

class Inventory {
private:
    std::vector<Module*> modules_; //список модулей, хранящихся в инвентаре

public:
    ~Inventory() {
        for (auto module : modules_) {
            delete module;
        }
    }

    void add_module(Module* module) {
        modules_.push_back(module);
        std::cout << "Добавлен модуль: ";
        module->display_info();
        std::cout << std::endl;
    }

    void display() const {
        if (modules_.empty()) {
            std::cout << "Инвентарь пуст\n";
            return;
        }

        std::cout << "Инвентарь модулей:\n";
        for (size_t i = 0; i < modules_.size(); ++i) {
            std::cout << i + 1 << ". ";
            modules_[i]->display_info();
            std::cout << std::endl;
        }
    }

    Module* take_module(size_t index) {
        if (index > 0 && index <= modules_.size()) {
            Module* module = modules_[index - 1];
            modules_.erase(modules_.begin() + index - 1);
            return module;
        }
        return nullptr;
    }

    size_t size() const { return modules_.size(); }
};

class BattleSystem {
private:
    std::vector<Robot>& player_team_;      //команда игрока, состоящая из роботов.
    Robot& enemy_;                         //Враг робот, с которым ведется бой.
    int& current_battle_level_;            //Текущий уровень битвы, влияет на сложность врагов и модулей.

public:
    BattleSystem(std::vector<Robot>& team, Robot& enemy_robot, int& level)
        : player_team_(team), enemy_(enemy_robot), current_battle_level_(level) {
    }

    BattleResult start_battle() {
        system("cls"); // очистка консоли
        std::cout << "\nБитва с " << enemy_.name_ << " началась!\n";

        while (true) {
            // проверяем, живы ли роботы игрока
            if (std::all_of(player_team_.begin(), player_team_.end(), [](const Robot& r) { return !r.is_alive(); })) {
                std::cout << "Ваша команда уничтожена! Битва завершена.\n";
                enemy_.reset_virus_effect();
                return BattleResult::PLAYER_LOST;
            }

            // ход игрока
            for (auto& robot : player_team_) {
                if (robot.is_alive()) {
                    BattleResult turn_result = player_turn(robot);
                    if (turn_result != BattleResult::CONTINUE) {
                        enemy_.reset_virus_effect();
                        return turn_result;
                    }
                    if (!enemy_.is_alive()) {
                        std::cout << enemy_.name_ << " уничтожен!\n";
                        std::cout << "\n*** ПОБЕДА В БИТВЕ! ***\n";
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        enemy_.reset_virus_effect();
                        return BattleResult::PLAYER_WON;
                    }
                }
            }

            // сбрасываем временные баффы после хода игрока
            for (auto& robot : player_team_) {
                robot.reset_buffs();
            }

            // ход врага
            if (enemy_.is_alive()) {
                enemy_turn();
            }
            else {
                enemy_.reset_virus_effect();
                return BattleResult::PLAYER_WON;
            }
        }
    }

private:
    BattleResult player_turn(Robot& robot) {
        std::cout << "\n--- Ход " << robot.name_ << " ---\n";
        std::cout << "Ваши характеристики: ";
        robot.display_stats();
        std::cout << "Характеристики врага: ";
        enemy_.display_stats();
        std::cout << "1. Атака\n";
        std::cout << "2. Особое умение (" << robot.ability_description_ << ")\n";

        int choice;
        std::cout << "Выберите действие: ";
        std::cin >> choice;

        if (choice == 1) {
            int damage = robot.calculate_damage();
            if (robot.name_ == "Бобби" && rand() % 100 < 30) {
                damage = static_cast<int>(damage * 1.5);
                std::cout << "Критический удар! ";
            }
            std::cout << robot.name_ << " атакует и наносит " << damage << " урона!\n";
            enemy_.take_damage(damage);
            return BattleResult::CONTINUE;
        }
        else if (choice == 2) {
            BattleResult ability_result = robot.use_special_ability(player_team_, &enemy_, current_battle_level_);
            return ability_result;
        }
        else {
            std::cout << "Неверный выбор действия, пропуск хода.\n";
            return BattleResult::CONTINUE;
        }
    }

    void enemy_turn() {
        if (!enemy_.is_alive()) return;

        if (enemy_.virus_duration_ > 0) {
            enemy_.apply_virus_effect(enemy_); // Враг получает урон от вируса
        }

        int damage = enemy_.calculate_damage();
        std::cout << "\n" << enemy_.name_ << " атакует команду и наносит " << damage << " урона каждому!\n";

        for (auto& robot : player_team_) {
            if (robot.is_alive()) {
                robot.take_damage(damage);
                if (!robot.is_alive()) {
                    std::cout << robot.name_ << " уничтожен!\n";
                }
            }
        }
    }
};

class GameMap {
private:
    struct Room {
        bool has_enemy;    //истина, если в комнате есть враг
        Module* module;    //указатель на модуль, если он есть в комнате
        Room() : has_enemy(false), module(nullptr) {}
    };

    Room map_[MAP_SIZE][MAP_SIZE]; //двухмерный массив, представляющий карту игры
    int player_x_;                 //координата игрока по X на карте
    int player_y_;                 //координата игрока по Y на карте
    bool final_boss_triggered_this_turn_; //флаг, указывающий, был ли вызван финальный босс в этом ходе

    void initialize_room_content(int x, int y, int level) {
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

    bool is_map_clear() const {
        for (int x = 0; x < MAP_SIZE; ++x) {
            for (int y = 0; y < MAP_SIZE; ++y) {
                if ((x != player_x_ || y != player_y_) && (map_[x][y].has_enemy || map_[x][y].module)) {
                    return false;
                }
            }
        }
        // если игрок в комнате (0,0) и она пустая, то карта считается очищенной
        // это нужно для того, чтобы не зацикливаться на финальном боссе
        if (map_[player_x_][player_y_].has_enemy || map_[player_x_][player_y_].module) {
            return false; // текущая комната не очищена
        }
        return true;
    }public:
        GameMap() : player_x_(0), player_y_(0), final_boss_triggered_this_turn_(false) {
            srand(static_cast<unsigned>(time(nullptr)));
            reset_map(1);
        }

        ~GameMap() {
            for (int x = 0; x < MAP_SIZE; ++x) {
                for (int y = 0; y < MAP_SIZE; ++y) {
                    if (map_[x][y].module) {
                        delete map_[x][y].module;
                        map_[x][y].module = nullptr;
                    }
                }
            }
        }

        void reset_map(int level) {
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
            std::cout << "\nНовая карта сгенерирована для уровня " << level << "!\n";
        }

        bool consume_final_boss_trigger() {
            bool triggered = final_boss_triggered_this_turn_;
            final_boss_triggered_this_turn_ = false;
            return triggered;
        }

        BattleResult move_player(int dx, int dy, std::vector<Robot>& team, Inventory& inventory, int& level) {
            int new_x = player_x_ + dx;
            int new_y = player_y_ + dy;

            if (new_x >= 0 && new_x < MAP_SIZE && new_y >= 0 && new_y < MAP_SIZE) {
                player_x_ = new_x;
                player_y_ = new_y;
                return check_room(team, inventory, level);
            }
            else {
                std::cout << "Нельзя выйти за пределы карты!\n";
                return BattleResult::CONTINUE;
            }
        }

        BattleResult check_room(std::vector<Robot>& team, Inventory& inventory, int& level) {
            bool room_content_changed = false;
            final_boss_triggered_this_turn_ = false;

            if (map_[player_x_][player_y_].has_enemy) {
                std::cout << "\nВы наткнулись на врага!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                Robot enemy("Враждебный дрон", 50 * level, 15 + (level * 2), 0, "Атака");
                BattleSystem battle(team, enemy, level);

                BattleResult battle_outcome = battle.start_battle();

                if (battle_outcome == BattleResult::PLAYER_WON) {
                    map_[player_x_][player_y_].has_enemy = false;
                    room_content_changed = true;
                }
                else if (battle_outcome == BattleResult::PLAYER_LOST) {
                    std::cout << "Все роботы уничтожены! Игра окончена.\n";
                    return BattleResult::PLAYER_LOST;
                }
            }
            else if (map_[player_x_][player_y_].module) {
                inventory.add_module(map_[player_x_][player_y_].module);
                map_[player_x_][player_y_].module = nullptr;
                room_content_changed = true;
                std::cout << "Вы нашли модуль!\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            // проверка, очищена ли вся карта
            if (room_content_changed && is_map_clear()) {
                if (level == FINAL_LEVEL) {
                    final_boss_triggered_this_turn_ = true;
                    std::cout << "\nВся карта очищена! Появляется финальный босс!\n";
                    return BattleResult::FINAL_BOSS_TRIGGERED; // вызов финального босса
                }
                else {
                    std::cout << "\nВся карта очищена! Переход на следующий уровень.\n";
                    ++level;
                    reset_map(level);
                }
            }
            return BattleResult::CONTINUE;
        }

        //отрисовка карты
        void display() const {
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
            std::cout << "P - Вы, E - Враг, M - Модуль, [ ] - Пустая комната\n";
        }
};

// / Функция для управления инвентарем и экипировкой модулей
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