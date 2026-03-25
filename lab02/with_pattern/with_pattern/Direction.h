#pragma once
#include "IMilitaryUnit.h"
#include <vector>
#include <string>

enum class EnemyType { INFANTRY, ARMOR, MIXED };

enum class DirectionResult {
    SUPPRESSED, HELD, BREAKTHROUGH_LOSSES, BREAKTHROUGH
};

// ============================================================
//  Direction — рубеж на карте
//  Хранит один вектор IMilitaryUnit* (единый интерфейс)
// ============================================================
class Direction {
public:
    std::string              label;
    int                      enemy_power;
    EnemyType                enemy_type;
    std::vector<IMilitaryUnit*> units;  // отделения и взводы — через интерфейс

    Direction(const std::string& label, int enemy_power, EnemyType enemy_type);

    void assign(IMilitaryUnit* unit);
    void removeAt(int index);

    // Коэффициент специализации по типу подразделения
    float getSpecMultiplier(const std::string& unitType) const;

    float           getFriendlyPower() const;
    float           getScore()         const;
    DirectionResult getResult()        const;
    int             getBattlePoints()  const;
    std::string     getResultText()    const;
    std::string     getEnemyTypeText() const;
};
