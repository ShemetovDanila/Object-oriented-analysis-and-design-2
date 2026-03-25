#include "Mission.h"
#include <sstream>

Mission::Mission(const std::string& t, const std::string& b, int bud)
    : title(t), briefing(b), budget(bud) {}

void Mission::addDirection(const Direction& d) { directions.push_back(d); }

bool Mission::hasBreakthrough() const {
    for (const auto& d : directions)
        if (d.getResult() == DirectionResult::BREAKTHROUGH) return true;
    return false;
}

int Mission::countBreakthroughs() const {
    int n = 0;
    for (const auto& d : directions)
        if (d.getResult() == DirectionResult::BREAKTHROUGH ||
            d.getResult() == DirectionResult::BREAKTHROUGH_LOSSES) n++;
    return n;
}

int Mission::calculateStars() const {
    if (countBreakthroughs() > (int)directions.size() / 2) return 0;
    int total = 0, maxPts = (int)directions.size() * 2;
    for (const auto& d : directions) total += d.getBattlePoints();
    float ratio = (float)total / (float)maxPts;
    int stars = (ratio >= 0.85f) ? 3 : (ratio >= 0.60f) ? 2 : (ratio >= 0.35f) ? 1 : 0;
    if (stars == 3 && hasBreakthrough()) stars = 2;
    return stars;
}

std::string Mission::getFullReport() const {
    std::ostringstream ss;
    ss << "=== " << title << " ===\n";
    for (const auto& d : directions)
        ss << d.label << ": " << d.getResultText()
           << " (" << d.getFriendlyPower() << " vs " << d.enemy_power << ")\n";
    ss << "ЗВЁЗДЫ: " << calculateStars() << "/3\n";
    return ss.str();
}

Mission MissionFactory::createMission1() {
    Mission m("ВЫСОТА 307",
        "Удержать высоту 307 до подхода подкрепления.\n"
        "Бронеколонна с запада, пехота в центре, разведка с востока.",
        100);
    m.addDirection(Direction("ЗАПАДНЫЙ",    40, EnemyType::ARMOR));
    m.addDirection(Direction("ЦЕНТРАЛЬНЫЙ", 25, EnemyType::INFANTRY));
    m.addDirection(Direction("ВОСТОЧНЫЙ",   15, EnemyType::INFANTRY));
    return m;
}

Mission MissionFactory::createMission2() {
    Mission m("ПЕРЕПРАВА",
        "Захватить мост и удержать плацдарм.\n"
        "Рекомендуется использовать взводы для усиленного натиска.",
        150);
    m.addDirection(Direction("ЦЕНТРАЛЬНЫЙ", 55, EnemyType::MIXED));
    m.addDirection(Direction("ПРАВЫЙ",      45, EnemyType::ARMOR));
    return m;
}
