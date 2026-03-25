#include "Formation.h"
#include <sstream>

Formation::Formation(const std::string& name, int level,
                     const std::string& role, int maxSlots)
    : level(level), role(role), max_slots(maxSlots), name_(name) {}

// ── Вспомогательные счётчики ────────────────────────────────
int Formation::countByType(const std::string& type) const {
    int n = 0;
    for (const auto* c : children_)
        if (c->getUnitType() == type) n++;
    return n;
}

bool Formation::hasVehicle() const {
    for (const auto* c : children_)
        if (c->getUnitType() == "VEHICLE") return true;
    return false;
}

// ── Методы узла ─────────────────────────────────────────────
bool Formation::add(IMilitaryUnit* child) {
    if ((int)children_.size() >= max_slots) return false;
    children_.push_back(child);
    return true;
}

void Formation::remove(int index) {
    if (index >= 0 && index < (int)children_.size())
        children_.erase(children_.begin() + index);
}

std::vector<IMilitaryUnit*> Formation::getChildren() const {
    return children_;
}

// ── hasBonus ────────────────────────────────────────────────
bool Formation::hasBonus() const {
    if (role == "INFANTRY")  return countByType("INFANTRY") >= 3;
    if (role == "TANK")      return hasVehicle() && countByType("INFANTRY") >= 3;
    if (role == "ARTILLERY") return hasVehicle() && countByType("INFANTRY") >= 2;
    if (role == "PLATOON") {
        int bonusCount = 0;
        for (const auto* c : children_)
            if (c->hasBonus()) bonusCount++;
        return bonusCount >= 3;
    }
    return false;
}

// ── calculatePower ──────────────────────────────────────────
// Ключевой метод Компоновщика:
// Formation рекурсивно вызывает calculatePower() у своих детей
float Formation::calculatePower() const {
    // Сумма мощей всех детей (рекурсия)
    float raw = 0.f;
    for (const auto* c : children_)
        raw += c->calculatePower();

    // Применяем бонус в зависимости от роли
    if (role == "INFANTRY")  return countByType("INFANTRY") >= 3 ? raw * 2.f : raw;
    if (role == "TANK")      return (hasVehicle() && countByType("INFANTRY") >= 3)
                                    ? raw + 50.f : raw;
    if (role == "ARTILLERY") return (hasVehicle() && countByType("INFANTRY") >= 2)
                                    ? raw + 30.f : raw;
    if (role == "PLATOON") {
        // Взвод: бонус если 3+ дочерних Formation с активным бонусом
        int bonusCount = 0;
        for (const auto* c : children_)
            if (c->hasBonus()) bonusCount++;
        return bonusCount >= 3 ? raw * 1.5f : raw;
    }
    return raw;
}

// ── getTotalCost ─────────────────────────────────────────────
// Рекурсивно суммирует стоимость всех детей
int Formation::getTotalCost() const {
    int total = 0;
    for (const auto* c : children_)
        total += c->getTotalCost();
    return total;
}

// ── Метаданные ──────────────────────────────────────────────
std::string Formation::getName()      const { return name_; }
std::string Formation::getUnitType()  const { return (level == 2) ? "PLATOON" : "SQUAD"; }
bool        Formation::isComposite()  const { return true; }

std::string Formation::getDescription() const {
    std::ostringstream ss;
    ss << name_ << " [" << children_.size() << "/" << max_slots << "]"
       << " мощь:" << (int)calculatePower()
       << (hasBonus() ? " БОН" : "");
    return ss.str();
}

// ── Фабрика ─────────────────────────────────────────────────
Formation* Formation::makeInfantrySquad() {
    return new Formation("Пех. отделение",  1, "INFANTRY",  8);
}
Formation* Formation::makeTankSquad() {
    return new Formation("Танк. отделение", 1, "TANK",      5);
}
Formation* Formation::makeArtillerySquad() {
    return new Formation("Арт. отделение",  1, "ARTILLERY", 5);
}
Formation* Formation::makePlatoon() {
    return new Formation("Взвод", 2, "PLATOON", 9); // 3 отделения × до 3 юнитов
}
