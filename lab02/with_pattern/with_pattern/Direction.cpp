#include "Direction.h"

Direction::Direction(const std::string& lbl, int ep, EnemyType et)
    : label(lbl), enemy_power(ep), enemy_type(et) {}

void Direction::assign(IMilitaryUnit* u)  { units.push_back(u); }

void Direction::removeAt(int idx) {
    if (idx >= 0 && idx < (int)units.size())
        units.erase(units.begin() + idx);
}

// Спец. коэффициент зависит от роли подразделения
float Direction::getSpecMultiplier(const std::string& ut) const {
    //                    INFANTRY  ARMOR  MIXED
    // INFANTRY (squad)    1.0      0.4    0.7
    // TANK (squad)        0.6      2.0    1.2
    // ARTILLERY (squad)   1.4      0.7    1.0
    // PLATOON             среднее по составу — используем 1.0 (взвод универсален)
    if (ut == "INFANTRY") {
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 1.0f;
        case EnemyType::ARMOR:    return 0.4f;
        case EnemyType::MIXED:    return 0.7f;
        }
    }
    if (ut == "TANK") {
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 0.6f;
        case EnemyType::ARMOR:    return 2.0f;
        case EnemyType::MIXED:    return 1.2f;
        }
    }
    if (ut == "ARTILLERY") {
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 1.4f;
        case EnemyType::ARMOR:    return 0.7f;
        case EnemyType::MIXED:    return 1.0f;
        }
    }
    // PLATOON, SQUAD общий, одиночный юнит — нет специализации
    return 1.0f;
}

float Direction::getFriendlyPower() const {
    float total = 0.f;
    for (const IMilitaryUnit* u : units)
        total += u->calculatePower() * getSpecMultiplier(u->getUnitType());
    return total;
}

float Direction::getScore() const { return getFriendlyPower() - (float)enemy_power; }

DirectionResult Direction::getResult() const {
    float s = getScore();
    if (s >= 6.f)  return DirectionResult::SUPPRESSED;
    if (s >= 0.f)  return DirectionResult::HELD;
    if (s >= -5.f) return DirectionResult::BREAKTHROUGH_LOSSES;
    return DirectionResult::BREAKTHROUGH;
}

int Direction::getBattlePoints() const {
    switch (getResult()) {
    case DirectionResult::SUPPRESSED:          return  2;
    case DirectionResult::HELD:                return  1;
    case DirectionResult::BREAKTHROUGH_LOSSES: return  0;
    case DirectionResult::BREAKTHROUGH:        return -1;
    }
    return 0;
}

std::string Direction::getResultText() const {
    switch (getResult()) {
    case DirectionResult::SUPPRESSED:          return "ПОДАВЛЕНО";
    case DirectionResult::HELD:                return "УДЕРЖАНО";
    case DirectionResult::BREAKTHROUGH_LOSSES: return "ПРОРЫВ С ПОТЕРЯМИ";
    case DirectionResult::BREAKTHROUGH:        return "ПРОРЫВ";
    }
    return "";
}

std::string Direction::getEnemyTypeText() const {
    switch (enemy_type) {
    case EnemyType::INFANTRY: return "ПЕХОТА";
    case EnemyType::ARMOR:    return "БРОНЯ";
    case EnemyType::MIXED:    return "СМЕШАННАЯ";
    }
    return "";
}
