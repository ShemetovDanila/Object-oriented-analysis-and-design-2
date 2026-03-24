#pragma once
#include "Unit.h"
#include <vector>
#include <string>

enum class SquadType { INFANTRY_SQUAD, TANK_SQUAD, ARTILLERY_SQUAD };

class Squad {
public:
    SquadType         type;
    std::string       name;
    std::vector<Unit> slots;
    int               max_slots = 8;

    explicit Squad(SquadType t);

    // Добавить любой юнит (только лимит слотов)
    bool addUnit(const Unit& unit);
    void removeUnit(int index);

    // Активный бонус — выполнены ли условия подразделения
    bool  hasBonus() const;

    // Итоговая мощь с учётом бонуса
    float calculatePower() const;

    // Суммарная стоимость (для возврата)
    int   getTotalCost() const;

    std::string getBonusDescription() const;
    std::string getDescription() const;
};
