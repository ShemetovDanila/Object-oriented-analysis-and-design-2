#pragma once
#include <string>

enum class UnitType { INFANTRY, VEHICLE };

struct Unit {
    std::string name;
    int         cost;
    int         base_power;  // пехота=5, техника=0
    UnitType    type;
    Unit(const std::string& n, int c, int bp, UnitType t)
        : name(n), cost(c), base_power(bp), type(t) {}
};

namespace UnitCatalog {
    // Пехотинец: мощь 5, стоимость 5
    inline Unit Rifleman()  { return Unit("Пехотинец", 5,  5, UnitType::INFANTRY); }
    // Танк: мощь 25 (бонус +50 в танк. отд. при 3+ пехотинцах), стоимость 25
    inline Unit Tank()      { return Unit("Танк",     25,  25, UnitType::VEHICLE);  }
    // Артиллерия: мощь 15 (бонус +30 в арт. отд. при 2+ пехотинцах), стоимость 15
    inline Unit Artillery() { return Unit("Артиллерия",15, 15, UnitType::VEHICLE);  }
}
