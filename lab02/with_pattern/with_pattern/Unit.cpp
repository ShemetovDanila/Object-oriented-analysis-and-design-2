#include "Unit.h"

Unit::Unit(const std::string& name, int cost, float basePower,
           const std::string& unitType)
    : cost(cost), base_power(basePower), name_(name), unitType_(unitType) {}

float       Unit::calculatePower()  const { return base_power; }
int         Unit::getTotalCost()    const { return cost; }
bool        Unit::hasBonus()        const { return false; }
std::string Unit::getName()         const { return name_; }
std::string Unit::getUnitType()     const { return unitType_; }
bool        Unit::isComposite()     const { return false; }

std::string Unit::getDescription() const {
    return name_ + " [" + unitType_ + "] мощь:" + std::to_string((int)base_power)
         + " стоимость:" + std::to_string(cost);
}

Unit* UnitFactory::makeRifleman()  { return new Unit("Пехотинец",   5,  5.f, "INFANTRY"); }
Unit* UnitFactory::makeTank()      { return new Unit("Танк",        25, 25.f, "VEHICLE");  }
Unit* UnitFactory::makeArtillery() { return new Unit("Артиллерия",  15, 15.f, "VEHICLE");  }
