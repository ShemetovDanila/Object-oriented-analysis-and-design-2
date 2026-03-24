#pragma once
#include "Squad.h"
#include "Platoon.h"
#include <vector>
#include <string>

enum class EnemyType { INFANTRY, ARMOR, MIXED };

enum class DirectionResult {
    SUPPRESSED, HELD, BREAKTHROUGH_LOSSES, BREAKTHROUGH
};

class Direction {
public:
    std::string           label;
    int                   enemy_power;
    EnemyType             enemy_type;
    std::vector<Unit*>    units;
    std::vector<Squad*>   squads;    // отдельные отделения
    std::vector<Platoon*> platoons;  // взвода

    Direction(const std::string& label, int enemy_power, EnemyType enemy_type);

    void assignUnit(Unit* u);
    void removeUnit(int idx);
    void assignSquad(Squad* sq);
    void removeSquad(int idx);
    void assignPlatoon(Platoon* pl);
    void removePlatoon(int idx);

    float           getSpecMultiplier(SquadType st) const;
    float           getFriendlyPower() const;
    float           getScore()         const;
    DirectionResult getResult()        const;
    int             getBattlePoints()  const;
    std::string     getResultText()    const;
    std::string     getEnemyTypeText() const;
};
