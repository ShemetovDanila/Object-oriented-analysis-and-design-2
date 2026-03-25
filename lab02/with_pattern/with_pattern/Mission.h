#pragma once
#include "Direction.h"
#include <vector>
#include <string>

class Mission {
public:
    std::string            title;
    std::string            briefing;
    int                    budget;
    std::vector<Direction> directions;

    Mission(const std::string& t, const std::string& b, int bud);
    void        addDirection(const Direction& d);
    int         calculateStars()  const;
    std::string getFullReport()   const;

private:
    bool hasBreakthrough()    const;
    int  countBreakthroughs() const;
};

namespace MissionFactory {
    Mission createMission1();
    Mission createMission2();
}
