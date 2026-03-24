#include "Platoon.h"
#include <sstream>

bool Platoon::addSquad(Squad* sq) {
    squads.push_back(sq);
    return true;
}

void Platoon::removeSquad(int idx) {
    if (idx >= 0 && idx < (int)squads.size())
        squads.erase(squads.begin() + idx);
}

bool Platoon::hasBonus() const {
    // Бонус: 3 или более отделений, и хотя бы 3 из них с активным бонусом
    int bonusCount = 0;
    for (const Squad* sq : squads)
        if (sq->hasBonus()) bonusCount++;
    return bonusCount >= 3;
}

float Platoon::calculatePower() const {
    float total = 0.f;
    for (const Squad* sq : squads)
        total += sq->calculatePower();
    return hasBonus() ? total * 1.5f : total;
}

int Platoon::getTotalCost() const {
    int total = 0;
    for (const Squad* sq : squads)
        total += sq->getTotalCost();
    return total;
}

std::string Platoon::getDescription() const {
    std::ostringstream ss;
    ss << name << " [" << squads.size() << "/3 отд.]"
        << " мощь:" << calculatePower()
        << (hasBonus() ? " БОНУС x1.5" : "");
    return ss.str();
}