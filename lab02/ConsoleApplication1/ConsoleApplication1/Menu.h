#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

struct LevelInfo {
    int         index;
    std::string title;
    std::string subtitle;
    std::string description;
    std::string budget;
    std::string directions;
};

class Menu {
public:
    Menu(sf::RenderWindow& win, sf::Font& font);

    // Возвращает индекс выбранного уровня (-1 = ещё не выбрано)
    int  getSelectedLevel() const { return selectedLevel; }
    void resetSelection()         { selectedLevel = -1; hoveredLevel = -1; }

    void draw();
    void handleMouseMove(sf::Vector2f pos);
    void handleMouseClick(sf::Vector2f pos);

private:
    sf::RenderWindow& win;
    sf::Font&         font;

    int selectedLevel = -1;
    int hoveredLevel  = -1;

    static constexpr float W = 1920.f;
    static constexpr float H = 900.f;

    std::vector<LevelInfo> levels;

    void drawBackground();
    void drawLeftPanel(float x, float y, float w, float h);
    void drawRightPanel(float x, float y, float w, float h);
    void drawLevelCard(const LevelInfo& lv, float x, float y,
                       float w, float h, bool hovered);

    void drawTxt(const std::string& s, unsigned sz,
                 sf::Color col, float x, float y);
    void rect(float x, float y, float w, float h,
              sf::Color fill, sf::Color border, float thick = 1.f);
    void line(sf::Vector2f a, sf::Vector2f b, sf::Color col, float thick = 1.f);
};
