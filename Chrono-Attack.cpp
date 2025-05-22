#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>

using namespace std;

// Enums for game elements
enum Role { ATTACK, DEFENSE, HEALER, SUPPORT };
enum TimePeriod { FUTURISTIC_APOCALYPSE, MIDDLE_AGES, INDUSTRIAL_REVOLUTION, SPACE_AGE, FINAL_CONFRONTATION };

// Helper function to get role name
string getRoleName(Role role) {
    switch (role) {
    case ATTACK: return "Attacker";
    case DEFENSE: return "Defender";
    case HEALER: return "Healer";
    case SUPPORT: return "Supporter";
    default: return "Unknown";
    }
}

// Helper function to get time period name
string getTimePeriodName(TimePeriod period) {
    switch (period) {
    case FUTURISTIC_APOCALYPSE: return "Futuristic Apocalypse";
    case MIDDLE_AGES: return "Middle Ages";
    case INDUSTRIAL_REVOLUTION: return "Industrial Revolution";
    case SPACE_AGE: return "Space Age";
    case FINAL_CONFRONTATION: return "Final Confrontation";
    default: return "Unknown Era";
    }
}

// Module class for robot upgrades
class Module {
private:
    string name;
    Role targetRole;
    int effectValue;

public:
    Module(string n, Role r, int val) : name(n), targetRole(r), effectValue(val) {}

    string getName() const { return name; }
    Role getTargetRole() const { return targetRole; }
    int getEffectValue() const { return effectValue; }

    void display() const {
        cout << name << " (For " << getRoleName(targetRole) << ") - Effect: " << effectValue << endl;
    }
};

// Robot class
class Robot {
private:
    string name;
    Role role;
    int health;
    int maxHealth;
    int abilityResource;
    vector<Module> modules;

public:
    Robot(string n, Role r, int hp) : name(n), role(r), health(hp), maxHealth(hp), abilityResource(0) {}

    string getName() const { return name; }
    Role getRole() const { return role; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAbilityResource() const { return abilityResource; }
    const vector<Module>& getModules() const { return modules; }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
    }

    void addModule(const Module& module) {
        if (modules.size() < 3) {
            modules.push_back(module);
            // Apply module effect
            if (module.getTargetRole() == role) {
                maxHealth += module.getEffectValue() * 10;
                health += module.getEffectValue() * 10;
            }
        }
    }

    bool canUseAbility() const {
        return abilityResource >= getRequiredResourceForAbility();
    }

    int getRequiredResourceForAbility() const {
        return 10; // Base requirement, could be modified by modules
    }

    void gainResource(int amount) {
        abilityResource += amount;
    }

    void resetResource() {
        abilityResource = 0;
    }

    void displayStatus() const {
        cout << name << " (" << getRoleName(role) << ") - HP: " << health << "/" << maxHealth
            << " | Resource: " << abilityResource << "/" << getRequiredResourceForAbility() << endl;
        if (!modules.empty()) {
            cout << "  Modules: ";
            for (const auto& module : modules) {
                cout << module.getName() << ", ";
            }
            cout << endl;
        }
    }

    // Role-specific abilities
    void useAbility(vector<Robot>& team, vector<Enemy>& enemies) {
        if (!canUseAbility()) {
            cout << name << " doesn't have enough resource to use ability!" << endl;
            return;
        }

        switch (role) {
        case ATTACK: {
            // Attack all enemies
            int damage = 15 + (modules.size() * 5);
            cout << name << " unleashes a powerful attack on all enemies!" << endl;
            for (auto& enemy : enemies) {
                enemy.takeDamage(damage);
                cout << "  Dealt " << damage << " damage to " << enemy.getName() << endl;
            }
            break;
        }
        case DEFENSE: {
            // Shield all allies
            int shield = 10 + (modules.size() * 3);
            cout << name << " creates a protective shield for the team!" << endl;
            for (auto& ally : team) {
                ally.heal(shield);
                cout << "  " << ally.getName() << " gained " << shield << " HP" << endl;
            }
            break;
        }
        case HEALER: {
            // Heal one ally
            Robot* mostInjured = &team[0];
            for (auto& ally : team) {
                if ((ally.getHealth() / (float)ally.getMaxHealth()) <
                    (mostInjured->getHealth() / (float)mostInjured->getMaxHealth())) {
                    mostInjured = &ally;
                }
            }
            int healAmount = 25 + (modules.size() * 8);
            mostInjured->heal(healAmount);
            cout << name << " heals " << mostInjured->getName() << " for " << healAmount << " HP" << endl;
            break;
        }
        case SUPPORT: {
            // Boost resource for all allies
            int boost = 5 + modules.size();
            cout << name << " energizes the team!" << endl;
            for (auto& ally : team) {
                ally.gainResource(boost);
                cout << "  " << ally.getName() << " gained " << boost << " resource" << endl;
            }
            break;
        }
        }

        abilityResource = 0; // Reset after using ability
    }


    void performBasicAction(vector<Robot>& enemies) {
        switch (role) {
        case ATTACK: {
            // Basic attack on random enemy
            if (!enemies.empty()) {
                int target = rand() % enemies.size();
                int damage = 8 + (rand() % 5);
                enemies[target].takeDamage(damage);
                gainResource(3);
                cout << name << " attacks " << enemies[target].getName() << " for " << damage << " damage!" << endl;
            }
            break;
        }
        case DEFENSE: {
            // Gain defense resource
            gainResource(4);
            cout << name << " prepares defenses and gains resource!" << endl;
            break;
        }
        case HEALER: {
            // Small heal to self
            int healAmount = 5 + (rand() % 3);
            heal(healAmount);
            gainResource(2);
            cout << name << " performs minor self-repair, healing " << healAmount << " HP" << endl;
            break;
        }
        case SUPPORT: {
            // Small resource gain for self
            gainResource(5);
            cout << name << " focuses and gains extra resource!" << endl;
            break;
        }
        }
    }
};

// Enemy class
class Enemy {
private:
    string name;
    int health;
    int damage;

public:
    Enemy(string n, int hp, int dmg) : name(n), health(hp), damage(dmg) {}

    string getName() const { return name; }
    int getHealth() const { return health; }

    void takeDamage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
    }

    int attack() const {
        return damage + (rand() % 3);
    }

    void displayStatus() const {
        cout << name << " - HP: " << health << endl;
    }
};

// Level class
class Level {
private:
    TimePeriod timePeriod;
    int levelNumber;
    Role requiredRole;
    int requiredResource;
    bool timeTravelTriggered;

public:
    Level(int num, TimePeriod period, Role role, int resource)
        : levelNumber(num), timePeriod(period), requiredRole(role),
        requiredResource(resource), timeTravelTriggered(false) {
    }

    TimePeriod getTimePeriod() const { return timePeriod; }
    int getLevelNumber() const { return levelNumber; }
    Role getRequiredRole() const { return requiredRole; }
    int getRequiredResource() const { return requiredResource; }
    bool isTimeTravelTriggered() const { return timeTravelTriggered; }

    void triggerTimeTravel() {
        timeTravelTriggered = true;
    }

    void displayInfo() const {
        cout << "\n=== LEVEL " << levelNumber << " ===" << endl;
        cout << "Time Period: " << getTimePeriodName(timePeriod) << endl;
        cout << "To time travel, the " << getRoleName(requiredRole)
            << " must gather " << requiredResource << " resource" << endl;
    }

    bool checkTimeTravelCondition(const vector<Robot>& team) const {
        for (const auto& robot : team) {
            if (robot.getRole() == requiredRole && robot.getAbilityResource() >= requiredResource) {
                return true;
            }
        }
        return false;
    }
};

// Game class
class Game {
private:
    vector<Robot> playerTeam;
    vector<Level> levels;
    int currentLevelIndex;
    vector<Module> availableModules;

    void initializeTeam() {
        playerTeam.push_back(Robot("Blitz", ATTACK, 80));
        playerTeam.push_back(Robot("Bulk", DEFENSE, 120));
        playerTeam.push_back(Robot("Mend", HEALER, 90));
        playerTeam.push_back(Robot("Boost", SUPPORT, 70));
    }

    void initializeLevels() {
        levels.push_back(Level(1, FUTURISTIC_APOCALYPSE, ATTACK, 25));
        levels.push_back(Level(2, MIDDLE_AGES, DEFENSE, 20));
        levels.push_back(Level(3, INDUSTRIAL_REVOLUTION, HEALER, 30));
        levels.push_back(Level(4, SPACE_AGE, SUPPORT, 15));
        levels.push_back(Level(5, FINAL_CONFRONTATION, ATTACK, 40)); // Final boss level
    }

    void initializeModules() {
        // Attack modules
        availableModules.push_back(Module("Plasma Cannon", ATTACK, 3));
        availableModules.push_back(Module("Energy Blade", ATTACK, 2));
        availableModules.push_back(Module("Rocket Fist", ATTACK, 4));

        // Defense modules
        availableModules.push_back(Module("Reactive Armor", DEFENSE, 3));
        availableModules.push_back(Module("Force Field", DEFENSE, 2));
        availableModules.push_back(Module("Nanite Shield", DEFENSE, 4));

        // Healer modules
        availableModules.push_back(Module("Medi-Nanites", HEALER, 3));
        availableModules.push_back(Module("Repair Beam", HEALER, 2));
        availableModules.push_back(Module("Bio-Restorer", HEALER, 4));

        // Support modules
        availableModules.push_back(Module("Energy Core", SUPPORT, 3));
        availableModules.push_back(Module("Boost Matrix", SUPPORT, 2));
        availableModules.push_back(Module("Harmonizer", SUPPORT, 4));
    }

    void displayTeamStatus() const {
        cout << "\n=== YOUR TEAM ===" << endl;
        for (const auto& robot : playerTeam) {
            robot.displayStatus();
        }
    }

    void exploreRoom() {
        cout << "\nExploring the area..." << endl;

        // Random encounter (70% chance)
        if (rand() % 100 < 70) {
            startCombat();
        }
        else {
            // Find a module (30% chance)
            if (rand() % 100 < 30) {
                findModule();
            }
            else {
                cout << "You find nothing of interest in this area." << endl;
            }
        }
    }

    void findModule() {
        // Filter modules for the current level's required role
        vector<Module> suitableModules;
        Role requiredRole = levels[currentLevelIndex].getRequiredRole();

        for (const auto& module : availableModules) {
            if (module.getTargetRole() == requiredRole) {
                suitableModules.push_back(module);
            }
        }

        if (!suitableModules.empty()) {
            int index = rand() % suitableModules.size();
            Module foundModule = suitableModules[index];

            cout << "You found a module: ";
            foundModule.display();

            // Find the robot with the matching role
            for (auto& robot : playerTeam) {
                if (robot.getRole() == foundModule.getTargetRole()) {
                    robot.addModule(foundModule);
                    cout << robot.getName() << " equipped the " << foundModule.getName() << "!" << endl;
                    break;
                }
            }
        }
        else {
            cout << "You found a broken module that can't be used." << endl;
        }
    }

    void startCombat() {
        cout << "\nENEMIES APPEAR!\n" << endl;

        // Create enemies based on current level
        vector<Enemy> enemies;
        int enemyCount = 2 + (currentLevelIndex / 2);

        for (int i = 0; i < enemyCount; i++) {
            string enemyName = "Drone MK-" + to_string(rand() % 10 + 1);
            int health = 30 + (currentLevelIndex * 10);
            int damage = 5 + (currentLevelIndex * 2);
            enemies.push_back(Enemy(enemyName, health, damage));
        }

        // Combat loop
        while (!enemies.empty() && !allRobotsDefeated()) {
            // Display combat status
            cout << "\n=== COMBAT ===" << endl;
            cout << "Enemies:" << endl;
            for (const auto& enemy : enemies) {
                enemy.displayStatus();
            }

            displayTeamStatus();

            // Player turn
            cout << "\nYour turn! Choose actions for your robots:" << endl;
            for (auto& robot : playerTeam) {
                if (robot.getHealth() > 0) {
                    cout << "\n" << robot.getName() << "'s turn:" << endl;
                    cout << "1. Basic Action" << endl;
                    cout << "2. Use Ability (Costs " << robot.getRequiredResourceForAbility() << " resource)" << endl;
                    cout << "Choice: ";

                    int choice;
                    cin >> choice;

                    if (choice == 2 && robot.canUseAbility()) {
                        robot.useAbility(playerTeam, enemies);

                        // Check if time travel condition is met
                        if (levels[currentLevelIndex].checkTimeTravelCondition(playerTeam)) {
                            cout << "\n=== TIME TRAVEL TRIGGERED ===" << endl;
                            cout << robot.getName() << " has gathered enough resource to time travel!" << endl;
                            levels[currentLevelIndex].triggerTimeTravel();
                            return; // End combat
                        }
                    }
                    else {
                        //robot.performBasicAction(enemies);
                    }
                }
            }

            // Remove defeated enemies
            enemies.erase(
                remove_if(enemies.begin(), enemies.end(),
                    [](const Enemy& e) { return e.getHealth() <= 0; }),
                enemies.end());

            // Enemy turn if any remain
            if (!enemies.empty()) {
                cout << "\nEnemy turn!" << endl;
                for (auto& enemy : enemies) {
                    if (!playerTeam.empty()) {
                        // Enemy attacks random alive robot
                        vector<int> aliveIndices;
                        for (int i = 0; i < playerTeam.size(); i++) {
                            if (playerTeam[i].getHealth() > 0) {
                                aliveIndices.push_back(i);
                            }
                        }

                        if (!aliveIndices.empty()) {
                            int targetIndex = aliveIndices[rand() % aliveIndices.size()];
                            int damage = enemy.attack();
                            playerTeam[targetIndex].takeDamage(damage);
                            cout << enemy.getName() << " attacks " << playerTeam[targetIndex].getName()
                                << " for " << damage << " damage!" << endl;
                        }
                    }
                }
            }

            // Check for defeated robots
            for (auto& robot : playerTeam) {
                if (robot.getHealth() <= 0) {
                    cout << robot.getName() << " has been defeated!" << endl;
                }
            }
        }

        // Combat results
        if (enemies.empty()) {
            cout << "\nYou won the battle!" << endl;

            // Heal all robots a bit after combat
            for (auto& robot : playerTeam) {
                if (robot.getHealth() > 0) {
                    robot.heal(10 + (currentLevelIndex * 2));
                }
                else {
                    // Revive defeated robots with 25% health
                    robot.heal(robot.getMaxHealth() / 4);
                }
            }
        }
    }

    bool allRobotsDefeated() const {
        for (const auto& robot : playerTeam) {
            if (robot.getHealth() > 0) {
                return false;
            }
        }
        return true;
    }

    void timeTravel() {
        cout << "\n=== TIME TRAVEL INITIATED ===" << endl;
        cout << "The team activates their time travel mechanism!" << endl;

        // Reset robots for new level
        for (auto& robot : playerTeam) {
            robot.heal(robot.getMaxHealth()); // Full heal
            robot.resetResource();
        }

        currentLevelIndex++;

        if (currentLevelIndex < levels.size()) {
            cout << "You arrive in: " << getTimePeriodName(levels[currentLevelIndex].getTimePeriod()) << endl;
        }
    }

    void handleFinalBoss() {
        cout << "\n=== FINAL CONFRONTATION ===" << endl;
        cout << "You face the mad scientist in his ultimate form!" << endl;

        vector<Enemy> enemies;
        enemies.push_back(Enemy("Mad Scientist", 200, 15));

        // Special final boss combat
        int phase = 1;
        while (!enemies.empty() && !allRobotsDefeated()) {
            // Display combat status
            cout << "\n=== BOSS PHASE " << phase << " ===" << endl;
            cout << "Enemy:" << endl;
            enemies[0].displayStatus();

            displayTeamStatus();

            // Player turn
            cout << "\nYour turn! Choose actions for your robots:" << endl;
            for (auto& robot : playerTeam) {
                if (robot.getHealth() > 0) {
                    cout << "\n" << robot.getName() << "'s turn:" << endl;
                    cout << "1. Basic Action" << endl;
                    cout << "2. Use Ability (Costs " << robot.getRequiredResourceForAbility() << " resource)" << endl;
                    cout << "Choice: ";

                    int choice;
                    cin >> choice;

                    if (choice == 2 && robot.canUseAbility()) {
                        robot.useAbility(playerTeam, enemies);

                        // Special condition for final boss - all robots must use their abilities
                        if (phase == 1 && enemies[0].getHealth() < 150) {
                            cout << "\nThe boss changes form!" << endl;
                            enemies[0] = Enemy("Mad Scientist Phase 2", 150, 20);
                            phase = 2;
                        }
                        else if (phase == 2 && enemies[0].getHealth() < 75) {
                            cout << "\nThe boss enters his final form!" << endl;
                            enemies[0] = Enemy("Mad Scientist Final Form", 75, 25);
                            phase = 3;
                        }
                    }
                    else {
                        //robot.performBasicAction(enemies);
                    }
                }
            }

            // Remove defeated enemies
            enemies.erase(
                remove_if(enemies.begin(), enemies.end(),
                    [](const Enemy& e) { return e.getHealth() <= 0; }),
                enemies.end());

            // Boss attack
            if (!enemies.empty()) {
                cout << "\nBoss attack!" << endl;
                for (auto& enemy : enemies) {
                    // Boss attacks all robots
                    for (auto& robot : playerTeam) {
                        if (robot.getHealth() > 0) {
                            int damage = enemy.attack();
                            robot.takeDamage(damage);
                            cout << enemy.getName() << " attacks " << robot.getName()
                                << " for " << damage << " damage!" << endl;
                        }
                    }
                }
            }

            // Check for defeated robots
            for (auto& robot : playerTeam) {
                if (robot.getHealth() <= 0) {
                    cout << robot.getName() << " has been defeated!" << endl;
                }
            }
        }

        if (enemies.empty()) {
            cout << "\n=== VICTORY ===" << endl;
            cout << "You have defeated the mad scientist and saved the timeline!" << endl;
        }
    }

public:
    Game() : currentLevelIndex(0) {
        srand(time(0)); // Seed random number generator
        initializeTeam();
        initializeLevels();
        initializeModules();
    }

    void start() {
        cout << "=== CHRONO ATTACK ===" << endl;
        cout << "A text-based RPG adventure through time!" << endl;

        while (currentLevelIndex < levels.size()) {
            levels[currentLevelIndex].displayInfo();
            displayTeamStatus();

            cout << "\nWhat would you like to do?" << endl;
            cout << "1. Explore the area" << endl;
            cout << "2. Check team status" << endl;
            cout << "3. Attempt to time travel" << endl;
            cout << "Choice: ";

            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                exploreRoom();
                break;
            case 2:
                displayTeamStatus();
                break;
            case 3:
                if (levels[currentLevelIndex].isTimeTravelTriggered()) {
                    timeTravel();
                }
                else {
                    cout << "You haven't met the requirements for time travel yet!" << endl;
                    cout << "The " << getRoleName(levels[currentLevelIndex].getRequiredRole())
                        << " needs to gather " << levels[currentLevelIndex].getRequiredResource()
                        << " resource first." << endl;
                }
                break;
            default:
                cout << "Invalid choice!" << endl;
            }

            // Check for game over
            if (allRobotsDefeated()) {
                cout << "\n=== GAME OVER ===" << endl;
                cout << "All your robots have been defeated!" << endl;
                return;
            }
        }

        // Final boss battle
        handleFinalBoss();
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}