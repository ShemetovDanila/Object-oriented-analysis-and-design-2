#pragma once
#include "Squad.h"
#include <vector>
#include <string>

class Platoon {
public:
    std::string         name = "Взвод";
    std::vector<Squad*> squads;          // до 3 отделений (не владеет)
    static constexpr int MAX_SQUADS = 3;

    // Добавить отделение во взвод
    bool addSquad(Squad* sq);
    void removeSquad(int idx);

    // Все 3 отделения заполнены И у всех активен бонус
    bool hasBonus() const;

    // Мощь: сумма отделений. Если hasBonus() → x1.5
    float calculatePower() const;

    // Стоимость всех юнитов (для возврата бюджета)
    int getTotalCost() const;

    std::string getDescription() const;
};