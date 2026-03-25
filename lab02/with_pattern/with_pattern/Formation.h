#pragma once
#include "IMilitaryUnit.h"

// ============================================================
//  Formation — узел Компоновщика
//
//  level = 1  → Отделение (содержит Unit-ы)
//  level = 2  → Взвод     (содержит Formation level=1)
//
//  role определяет формулу бонуса:
//    "INFANTRY"   — пех. отделение
//    "TANK"       — танк. отделение
//    "ARTILLERY"  — арт. отделение
//    "PLATOON"    — взвод
// ============================================================
class Formation : public IMilitaryUnit {
public:
    Formation(const std::string& name, int level,
              const std::string& role, int maxSlots);

    // ── IMilitaryUnit ──
    float       calculatePower()  const override;
    int         getTotalCost()    const override;
    bool        hasBonus()        const override;
    std::string getName()         const override;
    std::string getDescription()  const override;
    std::string getUnitType()     const override;  // "SQUAD" или "PLATOON"
    bool        isComposite()     const override;  // true

    // ── Методы узла ──
    bool                         add(IMilitaryUnit* child) override;
    void                         remove(int index)         override;
    std::vector<IMilitaryUnit*>  getChildren()       const override;

    // ── Поля (публичные для удобства Renderer) ──
    int         level;
    std::string role;
    int         max_slots;

    // ── Фабрика подразделений ──
    static Formation* makeInfantrySquad();   // пех. отделение
    static Formation* makeTankSquad();       // танк. отделение
    static Formation* makeArtillerySquad();  // арт. отделение
    static Formation* makePlatoon();         // взвод

private:
    std::string                 name_;
    std::vector<IMilitaryUnit*> children_;   // НЕ владеет памятью

    // Вспомогательные счётчики для формул
    int   countByType(const std::string& type) const;
    bool  hasVehicle()                          const;
};
