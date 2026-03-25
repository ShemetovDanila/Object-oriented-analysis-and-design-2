#pragma once
#include "IMilitaryUnit.h"

// ============================================================
//  Unit — лист Компоновщика
//  Три вида: пехотинец, танк, артиллерия
// ============================================================
class Unit : public IMilitaryUnit {
public:
    Unit(const std::string& name, int cost, float basePower,
         const std::string& unitType);

    float       calculatePower()  const override;  // просто base_power
    int         getTotalCost()    const override;  // cost
    bool        hasBonus()        const override;  // всегда false (юнит не имеет бонуса)
    std::string getName()         const override;
    std::string getDescription()  const override;
    std::string getUnitType()     const override;  // "INFANTRY" или "VEHICLE"
    bool        isComposite()     const override;  // false

    int   cost;
    float base_power;

private:
    std::string name_;
    std::string unitType_;   // "INFANTRY" | "VEHICLE"
};

namespace UnitFactory {
    Unit* makeRifleman();   // Пехотинец  cost=5  power=5  INFANTRY
    Unit* makeTank();       // Танк       cost=25 power=25  VEHICLE
    Unit* makeArtillery();  // Артиллерия cost=15 power=15  VEHICLE
}
