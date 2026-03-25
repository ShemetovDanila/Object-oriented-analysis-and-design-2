#pragma once
#include "Mission.h"
#include "Formation.h"
#include "Unit.h"
#include <vector>
#include <memory>

enum class GamePhase { DEPLOYING, RESULT };

// ============================================================
//  GameState — весь игровой state
//
//  Единственный владелец памяти через unique_ptr<IMilitaryUnit>
//  Все Direction хранят только указатели (не владеют)
// ============================================================
class GameState {
public:
    Mission*   currentMission      = nullptr;
    GamePhase  phase               = GamePhase::DEPLOYING;
    int        budget              = 0;
    int        spent               = 0;
    int        currentMissionIndex = 0;
    int        lastStars           = -1;

    int remaining() const { return budget - spent; }

    // Все созданные объекты — хранятся здесь
    std::vector<std::unique_ptr<IMilitaryUnit>> owned;

    // ── Создание ──────────────────────────────────────────
    // Создаёт Formation, регистрирует в owned, возвращает ptr
    Formation* createFormation(Formation* f);

    // Создаёт Unit, регистрирует в owned, возвращает ptr
    Unit* createUnit(Unit* u);

    // ── Рубеж ─────────────────────────────────────────────
    // Назначить IMilitaryUnit на рубеж
    bool assignToDirection(IMilitaryUnit* unit, int dirIdx);

    // Убрать с рубежа по индексу — возвращает стоимость
    void removeFromDirection(int dirIdx, int slotIdx);

    // ── Добавить юнит в Formation ─────────────────────────
    // dirIdx, slotIdx = индекс Formation на рубеже
    // Если Formation — взвод, subIdx = индекс отделения внутри
    bool addUnitToFormation(IMilitaryUnit* formation,
                            Unit* unit);

    // Убрать юнит из Formation по индексу дочернего элемента
    void removeUnitFromFormation(IMilitaryUnit* formation, int childIdx);

    // ── Итог ──────────────────────────────────────────────
    int  confirmAndCalculate();
    void reset(Mission* mission);

private:
    // Полное рекурсивное удаление объекта из owned
    // (объект мог быть вложен в Formation, но owned владеет всеми)
    void returnCost(IMilitaryUnit* u);
};
