#pragma once
#include "Mission.h"
#include "Squad.h"
#include "Platoon.h"
#include <vector>
#include <memory>

enum class GamePhase { DEPLOYING, RESULT };

class GameState {
public:
    Mission*   currentMission      = nullptr;
    GamePhase  phase               = GamePhase::DEPLOYING;
    int        budget              = 0;
    int        spent               = 0;
    int        currentMissionIndex = 0;
    int        lastStars           = -1;

    int remaining() const { return budget - spent; }

    // Все созданные отделения (владеют памятью)
    std::vector<std::unique_ptr<Squad>>   squads;
    // Все созданные взводы (владеют памятью)
    std::vector<std::unique_ptr<Platoon>> platoons;

    // ── Отделения ──
    Squad*  createSquad(SquadType type);
    bool    deploySquad(int squadIdx, int dirIdx);
    // Убрать отделение с рубежа + вернуть стоимость всех юнитов
    void    undeploySquad(int dirIdx, int squadSlotIdx);

    // ── Взводы ──
    Platoon* createPlatoon();
    bool     deployPlatoon(int platoonIdx, int dirIdx);
    // Убрать взвод с рубежа + вернуть стоимость всех юнитов
    void     undeployPlatoon(int dirIdx, int platoonSlotIdx);
    // Добавить отделение во взвод (отделение уже на рубеже или в списке)
    bool     addSquadToPlatoon(int platoonIdx, int squadIdx);

    // ── Юниты ──
    // Добавить юнит в отделение на рубеже (прямая закупка)
    bool    addUnitToSquad(int dirIdx, int squadSlotIdx, const Unit& unit);
    // Убрать юнит из отделения — возврат стоимости
    void    removeUnitFromSquad(int dirIdx, int squadSlotIdx, int unitIdx);

    // ── Итог ──
    int  confirmAndCalculate();
    void reset(Mission* mission);
};
