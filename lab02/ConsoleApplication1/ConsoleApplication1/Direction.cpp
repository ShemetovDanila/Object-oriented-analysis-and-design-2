#include "Direction.h"

Direction::Direction(const std::string& lbl, int ep, EnemyType et)
    : label(lbl), enemy_power(ep), enemy_type(et) {}

void Direction::assignUnit(Unit* u) { units.push_back(u); }

void Direction::removeUnit(int idx) {
    if (idx >= 0 && idx < (int)units.size())
        units.erase(units.begin() + idx);
}

void Direction::assignSquad(Squad* s)     { squads.push_back(s); }
void Direction::assignPlatoon(Platoon* p) { platoons.push_back(p); }

void Direction::removeSquad(int idx) {
    if (idx >= 0 && idx < (int)squads.size())
        squads.erase(squads.begin() + idx);
}
void Direction::removePlatoon(int idx) {
    if (idx >= 0 && idx < (int)platoons.size())
        platoons.erase(platoons.begin() + idx);
}

float Direction::getSpecMultiplier(SquadType st) const {
    switch (st) {
    case SquadType::INFANTRY_SQUAD:
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 1.0f;
        case EnemyType::ARMOR:    return 0.4f;
        case EnemyType::MIXED:    return 0.7f;
        } break;
    case SquadType::TANK_SQUAD:
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 0.6f;
        case EnemyType::ARMOR:    return 2.0f;
        case EnemyType::MIXED:    return 1.2f;
        } break;
    case SquadType::ARTILLERY_SQUAD:
        switch (enemy_type) {
        case EnemyType::INFANTRY: return 1.4f;
        case EnemyType::ARMOR:    return 0.7f;
        case EnemyType::MIXED:    return 1.0f;
        } break;
    }
    return 1.0f;
}

float Direction::getFriendlyPower() const {
    float total = 0.f;

    // + 1. Одиночные юниты (считаем просто по базе, без спец-бонусов)
    for (const Unit* u : units) {
        total += u->base_power;
    }

    // 2. Отдельные отделения
    for (const Squad* sq : squads)
        total += sq->calculatePower() * getSpecMultiplier(sq->type);

    // 3. Взводы — применяем мультипликатор к каждому отделению внутри
    for (const Platoon* pl : platoons) {
        float plPow = 0.f;
        for (const Squad* sq : pl->squads)
            plPow += sq->calculatePower() * getSpecMultiplier(sq->type);
        if (pl->hasBonus()) plPow *= 1.5f;
        total += plPow;
    }
    return total;
}

float           Direction::getScore()  const { return getFriendlyPower() - (float)enemy_power; }

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
