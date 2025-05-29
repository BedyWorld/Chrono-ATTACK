#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

const int MAP_SIZE = 10;

// Перечисляемый тип для бонусов
enum class BonusType {
    HEALTH,
    ATTACK,
    ABILITY,
    MIXED
};

// Класс модуля
class Module {
private:
    static const std::vector<std::string> namePrefixes;
    static const std::vector<std::string> nameSuffixes;

    // Генерация имени модуля
    std::string generateName() {
        std::string name = namePrefixes[rand() % namePrefixes.size()] + " " +
            nameSuffixes[rand() % nameSuffixes.size()];

        if (rand() % 100 < 30) {
            static const std::vector<std::string> prefixes = { "Прототип", "Улучшенный", "Боевой", "Элитный", "Экспериментальный" };
            name = prefixes[rand() % prefixes.size()] + " " + name;
        }
        return name;
    }

    // Генерация случайных характеристик модуля в зависимости от уровня
    void generateRandomStats(int level) {
        BonusType type = static_cast<BonusType>(rand() % 4);

        // Изменение характеристик в зависимости от уровня
        int bonusScale = level; // Каждый уровень дает бонус

        switch (type) {
        case BonusType::HEALTH:
            healthMod = (5 + rand() % 26) * bonusScale; // Модуль увеличивает здоровье на 5-30% * уровень
            attackMod = 0;
            abilityMod = 0;
            break;
        case BonusType::ATTACK:
            healthMod = 0;
            attackMod = (5 + rand() % 26) * bonusScale; // Модуль увеличивает атаку на 5-30% * уровень
            abilityMod = 0;
            break;
        case BonusType::ABILITY:
            healthMod = 0;
            attackMod = 0;
            abilityMod = (10 + rand() % 41) * bonusScale; // Модуль увеличивает способность на 10-50% * уровень
            break;
        case BonusType::MIXED:
            healthMod = (3 + rand() % 13) * bonusScale; // Модуль дает смешанные бонусы * уровень
            attackMod = (3 + rand() % 13) * bonusScale;
            abilityMod = (5 + rand() % 16) * bonusScale;
            break;
        }

        // 20% шанс на штраф к характеристикам
        if (rand() % 100 < 20) {
            int stat = rand() % 3;
            int penalty = -(1 + rand() % 5);
            if (stat == 0) healthMod += penalty;
            else if (stat == 1) attackMod += penalty;
            else abilityMod += penalty;
        }
    }

public:
    std::string name;
    int healthMod;
    int attackMod;
    int abilityMod;

    Module(int level) {
        generateRandomStats(level);
        name = generateName();
    }

    void displayInfo() const {
        std::cout << name << " (";
        bool first = true;

        if (healthMod != 0) {
            std::cout << "HP: " << healthMod;
            first = false;
        }
        if (attackMod != 0) {
            if (!first) std::cout << ", ";
            std::cout << "ATK: " << attackMod;
            first = false;
        }
        if (abilityMod != 0) {
            if (!first) std::cout << ", ";
            std::cout << "Ability: " << abilityMod;
        }
        std::cout << ")";
    }
};

const std::vector<std::string> Module::namePrefixes = {
    "Квантовый", "Плазменный", "Нано", "Кибер", "Турбо",
    "Гипер", "Мега", "Ультра", "Электро", "Нейро"
};

const std::vector<std::string> Module::nameSuffixes = {
    "усилитель", "стабилизатор", "контроллер", "модулятор",
    "трансформер", "аккумулятор", "реактор", "процессор",
    "драйвер", "адаптер"
};

// Класс робота
class Robot {
public:
    std::string name;
    int maxHealth;
    int health;
    int baseAttack;
    float baseSpecialAbilityValue;
    float currentSpecialAbilityValue;
    std::string abilityDescription;
    Module* equippedModule;

    Robot(std::string n, int mh, int ba, float sav, std::string ad)
        : name(n), maxHealth(mh), health(mh), baseAttack(ba),
        baseSpecialAbilityValue(sav), currentSpecialAbilityValue(sav),
        abilityDescription(ad), equippedModule(nullptr) {
    }

    void equipModule(Module* module) {
        if (equippedModule) {
            unequipModule();
        }

        equippedModule = module;
        applyModuleEffects();
    }

    void unequipModule() {
        if (!equippedModule) return;

        maxHealth = std::round(maxHealth * 100.0 / (100.0 + equippedModule->healthMod));
        baseAttack = std::round(baseAttack * 100.0 / (100.0 + equippedModule->attackMod));
        currentSpecialAbilityValue = baseSpecialAbilityValue * 100.0 / (100.0 + equippedModule->abilityMod);

        if (health > maxHealth) health = maxHealth;

        equippedModule = nullptr;
    }

    void applyModuleEffects() {
        if (!equippedModule) return;

        maxHealth = std::round(maxHealth * (100.0 + equippedModule->healthMod) / 100.0);
        baseAttack = std::round(baseAttack * (100.0 + equippedModule->attackMod) / 100.0);
        currentSpecialAbilityValue = baseSpecialAbilityValue * (100.0 + equippedModule->abilityMod) / 100.0;

        if (health > maxHealth) health = maxHealth;
    }

    void heal(Robot& target) {
        if (name != "Гобби") return; // Лечение только для Гобби
        float healAmount = currentSpecialAbilityValue;
        int oldHealth = target.health;
        target.health += healAmount;
        if (target.health > target.maxHealth) target.health = target.maxHealth;

        std::cout << target.name << " восстановил " << (target.health - oldHealth) << " HP! (" << oldHealth << " -> "
            << target.health << "/" << target.maxHealth << ")\n";
    }

    bool isAlive() const { return health > 0; }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    int calculateDamage() const {
        return baseAttack;// *(0.9 + static_cast<float>(rand()) / RAND_MAX * 0.2);
    }

    void displayStats() const {
        std::cout << name << ": HP=" << health << "/" << maxHealth
            << ", ATK=" << baseAttack
            << ", " << abilityDescription << "=" << currentSpecialAbilityValue;
        if (equippedModule) {
            std::cout << ", Модуль: " << equippedModule->name;
        }
        std::cout << "\n";
    }

    void useSpecialAbility(std::vector<Robot>& team) {
        if (name == "Гобби") {
            std::cout << "Выберите робота для лечения:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                if (team[i].isAlive()) {
                    std::cout << i + 1 << ". " << team[i].name
                        << " (HP: " << team[i].health << "/"
                        << team[i].maxHealth << ")\n";
                }
            }
            int choice;
            std::cin >> choice;
            if (choice > 0 && choice <= team.size() && team[choice - 1].isAlive()) {
                heal(team[choice - 1]);
            }
        }
        else if (name == "Бобби") {
            std::cout << name << " использует супер атаку! Урон увеличен на 50%!\n";
        }
        else if (name == "Робби") {
            for (auto& robot : team) {
                robot.health += currentSpecialAbilityValue;
                if (robot.health > robot.maxHealth) robot.health = robot.maxHealth;
            }
            std::cout << name << " усилил защиту всей команды на "
                << currentSpecialAbilityValue << " HP!\n";
        }
        else if (name == "Зомби") {
            for (auto& robot : team) {
                robot.baseAttack += currentSpecialAbilityValue;
            }
            std::cout << name << " увеличил атаку всей команды на "
                << currentSpecialAbilityValue << "!\n";
        }
    }
};

// Класс инвентаря
class Inventory {
private:
    std::vector<Module*> modules;

public:
    ~Inventory() {
        for (auto module : modules) {
            delete module;
        }
    }

    void addModule(Module* module) {
        modules.push_back(module);
        std::cout << "Добавлен модуль: ";
        module->displayInfo();
        std::cout << std::endl;
    }

    void display() const {
        if (modules.empty()) {
            std::cout << "Инвентарь пуст\n";
            return;
        }

        std::cout << "Инвентарь модулей:\n";
        for (size_t i = 0; i < modules.size(); ++i) {
            std::cout << i + 1 << ". ";
            modules[i]->displayInfo();
            std::cout << std::endl;
        }
    }

    Module* getModule(size_t index) {
        if (index > 0 && index <= modules.size()) {
            return modules[index - 1];
        }
        return nullptr;
    }

    void removeModule(size_t index) {
        if (index > 0 && index <= modules.size()) {
            modules.erase(modules.begin() + index - 1);
        }
    }

    size_t size() const { return modules.size(); }
};

// Класс боевой системы
class BattleSystem {
private:
    std::vector<Robot>& playerTeam;
    Robot& enemy;

public:
    BattleSystem(std::vector<Robot>& team, Robot& e) : playerTeam(team), enemy(e) {}

    void startBattle() {
        system("cls");
        std::cout << "\nБитва с " << enemy.name << " началась!\n";

        while (true) {
            for (auto& robot : playerTeam) {
                if (robot.isAlive()) {
                    playerTurn(robot);
                    if (!enemy.isAlive()) {
                        return; // Враги уничтожены, выходим из боя
                    }
                }
            }

            enemyTurn();

            if (std::all_of(playerTeam.begin(), playerTeam.end(), [](const Robot& r) { return !r.isAlive(); })) {
                std::cout << "Ваша команда уничтожена!\n";
                return;
            }
        }
    }

private:
    void playerTurn(Robot& robot) {
        std::cout << "\nХод " << robot.name << ":\n";
        std::cout << "1. Атака\n";
        std::cout << "2. Особое умение (" << robot.abilityDescription << ")\n";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int damage = robot.calculateDamage();
            if (robot.name == "Бобби" && rand() % 100 < 30) {
                damage = static_cast<int>(damage * 1.5);
                std::cout << "Критический удар! ";
            }
            std::cout << robot.name << " атакует и наносит " << damage << " урона!\n";
            enemy.takeDamage(damage);
        }
        else if (choice == 2) {
            robot.useSpecialAbility(playerTeam);
        }
    }

    void enemyTurn() {
        system("cls");
        int damage = enemy.calculateDamage();
        std::cout << enemy.name << " атакует команду и наносит " << damage << " урона каждому!\n";

        for (auto& robot : playerTeam) {
            if (robot.isAlive()) {
                robot.takeDamage(damage);
                if (!robot.isAlive()) {
                    std::cout << robot.name << " уничтожен!\n";
                }
            }
        }
    }
};

// Класс карты игры
class GameMap {
private:
    struct Room {
        bool hasEnemy;
        Module* module;
        bool discovered;

        Room() : hasEnemy(false), module(nullptr), discovered(false) {}
    };

    Room map[MAP_SIZE][MAP_SIZE]; // Матрица комнат
    int playerX, playerY;

    // Инициализация комнаты
    void initializeRoom(int x, int y, int level) {
        map[x][y].hasEnemy = (rand() % 100 < (20 + level * 10)); // Увеличение шанса на врага с уровнем
        if (map[x][y].hasEnemy) {
            map[x][y].module = nullptr; // Убираем модуль
        }
        else if (!map[x][y].module) {
            if (rand() % 100 < (20 + level * 5)) { // Увеличение шанса появления модуля с уровнем
                map[x][y].module = new Module(level);
            }
        }
    }

    // Проверка, изучены ли все комнаты
    bool allRoomsDiscovered() const {
        for (int x = 0; x < MAP_SIZE; ++x) {
            for (int y = 0; y < MAP_SIZE; ++y) {
                if (x != playerX || y != playerY) { // Игрок должен быть исключен из этой проверки
                    if (!map[x][y].discovered) {
                        return false; // Найдена неизученная комната
                    }
                }
            }
        }
        return true; // Все комнаты изучены
    }

public:
    GameMap() : playerX(0), playerY(0) {
        srand(static_cast<unsigned>(time(nullptr)));
        for (int x = 0; x < MAP_SIZE; ++x) {
            for (int y = 0; y < MAP_SIZE; ++y) {
                initializeRoom(x, y, 1); // Инициализация карты на старте
            }
        }
        map[playerX][playerY].discovered = true; // Помечаем начальную комнату как открытую
    }

    void movePlayer(int dx, int dy, std::vector<Robot>& team, Inventory& inventory, int& level) {
        int newX = playerX + dx;
        int newY = playerY + dy;

        if (newX >= 0 && newX < MAP_SIZE && newY >= 0 && newY < MAP_SIZE) {
            playerX = newX;
            playerY = newY;
            map[playerX][playerY].discovered = true; // Комната становится открытой
            checkRoom(team, inventory, level);
        }
        else {
            std::cout << "Нельзя выйти за пределы карты!\n";
        }
    }

    void checkRoom(std::vector<Robot>& team, Inventory& inventory, int& level) {
        if (map[playerX][playerY].hasEnemy) {
            Robot enemy("Враждебный дрон", 50 + (level * 10), 15 + (level * 2), 0, "Атака");
            BattleSystem battle(team, enemy);
            battle.startBattle();
            map[playerX][playerY].hasEnemy = false; // Убираем врага из комнаты

            // Условия для повышения уровня
            if (level == 1 && team[0].calculateDamage() >= 50) { // Условие для перехода на уровень 2
                std::cout << "Бобби нанес 50 урона, уровень повышен до 2!\n";
                level = 2;
            }
            else if (level == 2 && team[1].baseAttack >= 25) { // Условие для перехода на уровень 3
                std::cout << "Робби усилил защиту команды на 25 единиц, уровень повышен до 3!\n";
                level = 3;
            }
            else if (level == 3 && team[2].health >= 50) { // Условие для перехода на уровень 4
                std::cout << "Гобби восстановил 50 единиц здоровья, уровень повышен до 4!\n";
                level = 4;
            }
            else if (level == 4 && team[3].baseAttack >= 100) { // Условие для выхода на финал
                std::cout << "Зомби усилил команду на 100%, вы завершили игру!\n";
                exit(0); // Выход из игры
            }

            // Обновляем всю комнату
            initializeRoom(playerX, playerY, level);
        }
        else if (map[playerX][playerY].module) {
            inventory.addModule(map[playerX][playerY].module);
            map[playerX][playerY].module = nullptr; // Убираем модуль из комнаты
        }
        else {
            std::cout << "Комната пуста.\n";
        }
    }

    void display() const {
        system("cls");
        std::cout << "\nКарта:\n";
        for (int x = 0; x < MAP_SIZE; ++x) {
            for (int y = 0; y < MAP_SIZE; ++y) {
                if (x == playerX && y == playerY) {
                    std::cout << "[P]"; // Спецсимвол для игрока
                }
                else if (map[x][y].discovered) {
                    if (map[x][y].hasEnemy) std::cout << "[E]"; // Враг
                    else if (map[x][y].module) std::cout << "[M]"; // Модуль
                    else std::cout << "[ ]"; // Пустая комната
                }
                else {
                    std::cout << "[?]"; // Неизвестно, что в комнате
                }
                std::cout << " ";
            }
            std::cout << "\n";
        }
        std::cout << "P - Вы, E - Враг, M - Модуль, ? - Неизвестно\n";
    }
};

// Функция управления модулями
void manageEquipment(std::vector<Robot>& team, Inventory& inventory) {
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
                std::cout << i + 1 << ". " << team[i].name;
                if (team[i].equippedModule) {
                    std::cout << " (экипирован " << team[i].equippedModule->name << ")";
                }
                std::cout << "\n";
            }

            int robotChoice;
            std::cin >> robotChoice;

            if (robotChoice < 1 || robotChoice > team.size()) {
                std::cout << "Неверный выбор робота!\n";
                continue;
            }

            inventory.display();
            std::cout << "Выберите модуль для экипировки (0 - отмена): ";

            int moduleChoice;
            std::cin >> moduleChoice;

            if (moduleChoice == 0) continue;

            Module* module = inventory.getModule(moduleChoice);
            if (module) {
                team[robotChoice - 1].equipModule(module);
                inventory.removeModule(moduleChoice);
            }
            else {
                std::cout << "Неверный выбор модуля!\n";
            }
        }
        else if (choice == 3) {
            std::cout << "Выберите робота для снятия модуля:\n";
            for (size_t i = 0; i < team.size(); ++i) {
                std::cout << i + 1 << ". " << team[i].name;
                if (team[i].equippedModule) {
                    std::cout << " (экипирован " << team[i].equippedModule->name << ")";
                }
                else {
                    std::cout << " (нет модуля)";
                }
                std::cout << "\n";
            }

            int robotChoice;
            std::cin >> robotChoice;

            if (robotChoice < 1 || robotChoice > team.size()) {
                std::cout << "Неверный выбор робота!\n";
                continue;
            }

            if (team[robotChoice - 1].equippedModule) {
                inventory.addModule(team[robotChoice - 1].equippedModule);
                team[robotChoice - 1].unequipModule();
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
    }
}

int main() {
    setlocale(LC_ALL, "Russian"); // Установка локали для русского языка
    srand(static_cast<unsigned>(time(0)));

    std::vector<Robot> team = {
        Robot("Бобби", 60, 20, 1.5f, "Множитель урона"),
        Robot("Робби", 80, 15, 10, "Бонус защиты"),
        Robot("Гобби", 50, 10, 15, "Лечение"),
        Robot("Зомби", 40, 25, 5, "Бонус атаки")
    };

    GameMap gameMap;
    Inventory inventory;
    int currentLevel = 1; // Уровень игры

    while (true) {
        gameMap.display();

        std::cout << "\nТекущие характеристики команды:\n";
        for (const auto& robot : team) {
            if (robot.isAlive()) {
                robot.displayStats();
            }
            else {
                std::cout << robot.name << " уничтожен\n";
            }
        }

        std::cout << "Текущий уровень: " << currentLevel << "\n"; // Отображение текущего уровня

        std::cout << "\nКоманды: w - вверх, s - вниз, a - влево, d - вправо, m - управление модулями, q - выход\n";
        char input;
        std::cin >> input;

        if (input == 'q') break;

        switch (input) {
        case 'w': gameMap.movePlayer(-1, 0, team, inventory, currentLevel); break;
        case 's': gameMap.movePlayer(1, 0, team, inventory, currentLevel); break;
        case 'a': gameMap.movePlayer(0, -1, team, inventory, currentLevel); break;
        case 'd': gameMap.movePlayer(0, 1, team, inventory, currentLevel); break;
        case 'm': manageEquipment(team, inventory); break;
        default: std::cout << "Неизвестная команда\n";
        }

        // Проверка на пересоздание карты, если все комнаты изучены
        if (std::all_of(team.begin(), team.end(), [](const Robot& r) { return !r.isAlive(); })) {
            std::cout << "Все роботы уничтожены! Игра окончена.\n";
            break;
        }
    }

    return 0;
}