#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include "GameConfig.h" // ��� BonusType

class Module {
private:
    // ������ ��� ��������� �����
    static const std::vector<std::string> name_prefixes_;
    static const std::vector<std::string> name_suffixes_;

    // ���������� ��������� ��� ��� ������
    std::string generate_name();
    // ���������� ��������� �������������� � ����������� �� ������
    void generate_random_stats(int level);

public:
    std::string name_;
    int health_mod_;
    int attack_mod_;
    int ability_mod_;

    // �����������, ��������� ������ ��� ������������� ������
    Module(int level);

    // ���������� ���������� � ������
    void display_info() const;
};

#endif