#ifndef GameConfig_h
#define GameConfig_h
// ������ �����
const int MAP_SIZE = 5;

// ���-�� ������� �� �����
const int FINAL_LEVEL = 3;

// ���� ������� ������
enum class BonusType {
    HEALTH,
    ATTACK,
    ABILITY,
    MIXED
};

// ��������� ������ �����
enum class BattleResult {
    CONTINUE,
    PLAYER_WON,
    PLAYER_LOST,
    FINAL_BOSS_TRIGGERED
};

#endif
