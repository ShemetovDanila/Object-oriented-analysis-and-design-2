#include "GameState.h"

// ── Отделения ────────────────────────────────────────────────
Squad* GameState::createSquad(SquadType type) {
    squads.push_back(std::make_unique<Squad>(type));
    return squads.back().get();
}

bool GameState::deploySquad(int squadIdx, int dirIdx) {
    if (!currentMission) return false;
    if (squadIdx < 0 || squadIdx >= (int)squads.size()) return false;
    if (dirIdx   < 0 || dirIdx   >= (int)currentMission->directions.size()) return false;
    currentMission->directions[dirIdx].assignSquad(squads[squadIdx].get());
    return true;
}

void GameState::undeploySquad(int dirIdx, int squadSlotIdx) {
    if (!currentMission) return;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return;
    auto& d = currentMission->directions[dirIdx];
    if (squadSlotIdx < 0 || squadSlotIdx >= (int)d.squads.size()) return;
    // Возврат стоимости всех юнитов
    spent -= d.squads[squadSlotIdx]->getTotalCost();
    // Удаляем само отделение из коллекции GameState
    Squad* target = d.squads[squadSlotIdx];
    d.removeSquad(squadSlotIdx);
    // Удалить из squads
    for (auto it = squads.begin(); it != squads.end(); ++it) {
        if (it->get() == target) { squads.erase(it); break; }
    }
}

// ── Взводы ───────────────────────────────────────────────────
Platoon* GameState::createPlatoon() {
    platoons.push_back(std::make_unique<Platoon>());
    return platoons.back().get();
}

bool GameState::deployPlatoon(int platoonIdx, int dirIdx) {
    if (!currentMission) return false;
    if (platoonIdx < 0 || platoonIdx >= (int)platoons.size()) return false;
    if (dirIdx     < 0 || dirIdx     >= (int)currentMission->directions.size()) return false;
    currentMission->directions[dirIdx].assignPlatoon(platoons[platoonIdx].get());
    return true;
}

void GameState::undeployPlatoon(int dirIdx, int platoonSlotIdx) {
    if (!currentMission) return;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return;
    auto& d = currentMission->directions[dirIdx];
    if (platoonSlotIdx < 0 || platoonSlotIdx >= (int)d.platoons.size()) return;
    Platoon* target = d.platoons[platoonSlotIdx];
    // Возврат стоимости всех юнитов во всех отделениях взвода
    spent -= target->getTotalCost();
    d.removePlatoon(platoonSlotIdx);
    // Удалить входящие отделения из squads
    for (Squad* sq : target->squads) {
        for (auto it = squads.begin(); it != squads.end(); ++it) {
            if (it->get() == sq) { squads.erase(it); break; }
        }
    }
    // Удалить взвод из platoons
    for (auto it = platoons.begin(); it != platoons.end(); ++it) {
        if (it->get() == target) { platoons.erase(it); break; }
    }
}

bool GameState::addSquadToPlatoon(int platoonIdx, int squadIdx) {
    if (platoonIdx < 0 || platoonIdx >= (int)platoons.size()) return false;
    if (squadIdx   < 0 || squadIdx   >= (int)squads.size())   return false;
    return platoons[platoonIdx]->addSquad(squads[squadIdx].get());
}

// ── Юниты ────────────────────────────────────────────────────
bool GameState::addUnitToSquad(int dirIdx, int squadSlotIdx, const Unit& unit) {
    if (!currentMission) return false;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return false;
    auto& d = currentMission->directions[dirIdx];
    if (squadSlotIdx < 0 || squadSlotIdx >= (int)d.squads.size()) return false;
    if (remaining() < unit.cost) return false;
    if (!d.squads[squadSlotIdx]->addUnit(unit)) return false;
    spent += unit.cost;
    return true;
}

void GameState::removeUnitFromSquad(int dirIdx, int squadSlotIdx, int unitIdx) {
    if (!currentMission) return;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return;
    auto& d = currentMission->directions[dirIdx];
    if (squadSlotIdx < 0 || squadSlotIdx >= (int)d.squads.size()) return;
    Squad* sq = d.squads[squadSlotIdx];
    if (unitIdx < 0 || unitIdx >= (int)sq->slots.size()) return;
    spent -= sq->slots[unitIdx].cost;
    sq->removeUnit(unitIdx);
}

// ── Итог ─────────────────────────────────────────────────────
int GameState::confirmAndCalculate() {
    if (!currentMission) return 0;
    lastStars = currentMission->calculateStars();
    phase     = GamePhase::RESULT;
    return lastStars;
}

void GameState::reset(Mission* mission) {
    currentMission = mission;
    phase          = GamePhase::DEPLOYING;
    budget         = mission->budget;
    spent          = 0;
    lastStars      = -1;
    squads.clear();
    platoons.clear();
    for (auto& d : mission->directions) {
        d.squads.clear();
        d.platoons.clear();
    }
}
