#ifndef GameConfig_h
#define GameConfig_h
// Размер карты
const int MAP_SIZE = 5;

// Кол-во уровней до босса
const int FINAL_LEVEL = 3;

// Типы бонусов модуля
enum class BonusType {
    HEALTH,
    ATTACK,
    ABILITY,
    MIXED
};

// Состояния исхода битвы
enum class BattleResult {
    CONTINUE,
    PLAYER_WON,
    PLAYER_LOST,
    FINAL_BOSS_TRIGGERED
};

#endif
