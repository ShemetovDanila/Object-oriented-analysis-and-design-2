#include "GameState.h"

// ── Создание ─────────────────────────────────────────────────
Formation* GameState::createFormation(Formation* f) {
    owned.emplace_back(f);
    return f;
}

Unit* GameState::createUnit(Unit* u) {
    owned.emplace_back(u);
    return u;
}

// ── Рубеж ────────────────────────────────────────────────────
bool GameState::assignToDirection(IMilitaryUnit* unit, int dirIdx) {
    if (!currentMission) return false;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return false;
    currentMission->directions[dirIdx].assign(unit);
    return true;
}

void GameState::removeFromDirection(int dirIdx, int slotIdx) {
    if (!currentMission) return;
    if (dirIdx < 0 || dirIdx >= (int)currentMission->directions.size()) return;
    auto& d = currentMission->directions[dirIdx];
    if (slotIdx < 0 || slotIdx >= (int)d.units.size()) return;

    IMilitaryUnit* target = d.units[slotIdx];
    // Возвращаем стоимость
    returnCost(target);
    d.removeAt(slotIdx);

    // Удаляем из owned (и все дочерние если Formation)
    owned.erase(
        std::remove_if(owned.begin(), owned.end(),
            [target](const std::unique_ptr<IMilitaryUnit>& p){
                return p.get() == target;
            }),
        owned.end()
    );
}

// ── Добавить юнит в Formation ────────────────────────────────
bool GameState::addUnitToFormation(IMilitaryUnit* formation, Unit* unit) {
    if (!formation || !unit) return false;
    if (!formation->isComposite()) return false;
    if (remaining() < unit->cost) return false;
    if (!formation->add(unit)) return false;
    spent += unit->cost;
    return true;
}

// Убрать юнит из Formation — возврат стоимости
void GameState::removeUnitFromFormation(IMilitaryUnit* formation, int childIdx) {
    if (!formation || !formation->isComposite()) return;
    auto children = formation->getChildren();
    if (childIdx < 0 || childIdx >= (int)children.size()) return;
    IMilitaryUnit* child = children[childIdx];
    returnCost(child);
    formation->remove(childIdx);
    // Удаляем из owned
    owned.erase(
        std::remove_if(owned.begin(), owned.end(),
            [child](const std::unique_ptr<IMilitaryUnit>& p){
                return p.get() == child;
            }),
        owned.end()
    );
}

// ── Рекурсивный возврат стоимости ───────────────────────────
void GameState::returnCost(IMilitaryUnit* u) {
    if (!u) return;
    if (!u->isComposite()) {
        spent -= u->getTotalCost();
        return;
    }
    // Formation: рекурсивно возвращаем стоимость детей
    for (IMilitaryUnit* child : u->getChildren()) {
        returnCost(child);
        // Убираем дочерние из owned
        owned.erase(
            std::remove_if(owned.begin(), owned.end(),
                [child](const std::unique_ptr<IMilitaryUnit>& p){
                    return p.get() == child;
                }),
            owned.end()
        );
    }
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
    owned.clear();  // unique_ptr освобождает всю память
    for (auto& d : mission->directions)
        d.units.clear();
}
