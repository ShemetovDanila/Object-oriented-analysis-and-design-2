#include "Squad.h"
#include <sstream>

Squad::Squad(SquadType t) : type(t) {
    switch (t) {
    case SquadType::INFANTRY_SQUAD:
        name = "Пех. отделение"; max_slots = 8; break;
    case SquadType::TANK_SQUAD:
        name = "Танк. отделение"; max_slots = 5; break;
    case SquadType::ARTILLERY_SQUAD:
        name = "Арт. отделение"; max_slots = 5; break;
    }
}

bool Squad::addUnit(const Unit& unit) {
    if ((int)slots.size() >= max_slots) return false;
    slots.push_back(unit);
    return true;
}

void Squad::removeUnit(int idx) {
    if (idx >= 0 && idx < (int)slots.size())
        slots.erase(slots.begin() + idx);
}

// Вспомогательные счётчики
static int countType(const std::vector<Unit>& slots, UnitType t) {
    int n = 0;
    for (const auto& u : slots) if (u.type == t) n++;
    return n;
}
static bool hasVehicle(const std::vector<Unit>& slots) {
    for (const auto& u : slots) if (u.type == UnitType::VEHICLE) return true;
    return false;
}

bool Squad::hasBonus() const {
    int inf = countType(slots, UnitType::INFANTRY);
    switch (type) {
    case SquadType::INFANTRY_SQUAD:
        return inf >= 3;
    case SquadType::TANK_SQUAD:
        return hasVehicle(slots) && inf >= 3;
    case SquadType::ARTILLERY_SQUAD:
        return hasVehicle(slots) && inf >= 2;
    }
    return false;
}

float Squad::calculatePower() const {
    // Базовая мощь = сумма base_power всех юнитов
    float raw = 0.f;
    float infantryOnlyPower = 0.f; // Накапливаем мощь только пехотинцев

    for (const auto& u : slots) {
        raw += u.base_power;
        if (u.type == UnitType::INFANTRY) {
            infantryOnlyPower += u.base_power;
        }
    }

    int inf = countType(slots, UnitType::INFANTRY);

    switch (type) {
    case SquadType::INFANTRY_SQUAD:
        // Бонус: 3+ пехотинцев → коэффициент x2
        if (inf >= 3) return raw + infantryOnlyPower;
        return raw;

    case SquadType::TANK_SQUAD:
        // Бонус: танк + 3+ пехотинцев → +50
        if (hasVehicle(slots) && inf >= 3) return raw + 50.f;
        return raw;

    case SquadType::ARTILLERY_SQUAD:
        // Бонус: артиллерия + 2+ пехотинцев → +30
        if (hasVehicle(slots) && inf >= 2) return raw + 30.f;
        return raw;
    }
    return raw;
}

int Squad::getTotalCost() const {
    int total = 0;
    for (const auto& u : slots) total += u.cost;
    return total;
}

std::string Squad::getBonusDescription() const {
    switch (type) {
    case SquadType::INFANTRY_SQUAD:
        return "3+ пехотинца -> мощь x2";
    case SquadType::TANK_SQUAD:
        return "Танк + 3+ пехотинца -> +50 мощи";
    case SquadType::ARTILLERY_SQUAD:
        return "Артиллерия + 2+ пехотинца -> +30 мощи";
    }
    return "";
}

std::string Squad::getDescription() const {
    std::ostringstream ss;
    ss << name << " [" << slots.size() << "/" << max_slots << "]"
       << " мощь:" << calculatePower()
       << (hasBonus() ? " БОНУС" : "");
    return ss.str();
}
