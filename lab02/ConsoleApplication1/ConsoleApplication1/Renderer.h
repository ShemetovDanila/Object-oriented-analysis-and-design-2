#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include <string>
#include <vector>

namespace C {
    const sf::Color BG         = sf::Color(10,  18,  10);
    const sf::Color BG_PANEL   = sf::Color(14,  22,  14);
    const sf::Color BG_CARD    = sf::Color(18,  30,  18);
    const sf::Color BG_SEL     = sf::Color(15,  45,  15);
    const sf::Color BG_POPUP   = sf::Color(11,  19,  11);
    const sf::Color BORDER     = sf::Color(35,  70,  35);
    const sf::Color BORDER_HOT = sf::Color(65,  120, 65);
    const sf::Color GREEN      = sf::Color(90,  255, 90);
    const sf::Color GREEN_DIM  = sf::Color(35,  110, 35);
    const sf::Color AMBER      = sf::Color(255, 170, 0);
    const sf::Color AMBER_DIM  = sf::Color(110, 65,  0);
    const sf::Color RED        = sf::Color(255, 65,  65);
    const sf::Color RED_DIM    = sf::Color(110, 18,  18);
    const sf::Color TEXT       = sf::Color(195, 218, 185);
    const sf::Color TEXT_DIM   = sf::Color(80,  112, 72);
    const sf::Color GRID       = sf::Color(18,  32,  18);
    const sf::Color RIVER      = sf::Color(25,  50,  130);
    const sf::Color FOREST     = sf::Color(18,  65,  18);
    const sf::Color HILL       = sf::Color(75,  85,  38);
    const sf::Color STAR_ON    = sf::Color(255, 215, 0);
    const sf::Color STAR_OFF   = sf::Color(45,  45,  25);
    const sf::Color GOLD       = sf::Color(255, 200, 50);
    const sf::Color NONE       = sf::Color(0,   0,   0, 0);
}

struct MapNode { sf::Vector2f pos; float r = 44.f; int dirIdx = 0; };

// Что сейчас открыто в попапе для рубежа
struct Popup {
    bool  visible = false;
    int   dirIdx  = -1;
    float x = 0.f, y = 0.f;
    // expanded[si] = развёрнуто ли отделение
    std::vector<bool> squadExp;
    // expanded для взводов (и каждое отделение внутри)
    std::vector<bool> platoonExp;
    std::vector<std::vector<bool>> platoonSquadExp;

    void openFor(int dir, float px, float py,
                 int numSquads, int numPlatoons) {
        visible = true; dirIdx = dir; x = px; y = py;
        squadExp.assign(numSquads, true);
        platoonExp.assign(numPlatoons, true);
        platoonSquadExp.resize(numPlatoons);
        // будет синхронизировано позже
    }
};

// Дропдаун добавления
// target = 0: добавить на рубеж (юнит/отделение/взвод)
// target = 1: добавить юнита в отделение
struct AddDrop {
    bool   visible    = false;
    int    dirIdx     = -1;
    int    squadIdx   = -1;   // -1=добавить на рубеж, -10=выбор типа отд. во взвод, >=0=добавить юнит в отд.
    Squad* targetSquad = nullptr; // прямой указатель для взводных отделений
    float  x = 0.f, y = 0.f;
};

class Renderer {
public:
    Renderer(sf::RenderWindow& win, sf::Font& font, GameState& state);

    void draw();
    void handleMouseMove(sf::Vector2f pos);
    void handleMouseClick(sf::Vector2f pos);
    void handleMouseScroll(float delta, sf::Vector2f pos);
    void buildMap(Mission* mission);

    bool wantsNextMission = false;
    bool wantsMenu        = false;   // флаг возврата в главное меню

private:
    sf::RenderWindow& win;
    sf::Font&         font;
    GameState&        state;

    static constexpr float W       = 1920.f;
    static constexpr float H       = 900.f;
    static constexpr float TOP_H   = 50.f;
    static constexpr float MAP_H   = 510.f;
    static constexpr float PAN_Y   = TOP_H + MAP_H;
    static constexpr float PAN_H   = H - PAN_Y;
    static constexpr float COL1_W  = 500.f;
    static constexpr float COL2_W  = 440.f;
    static constexpr float POPUP_W = 480.f;

    std::vector<MapNode> nodes;
    int     hoveredNode    = -1;
    Popup   popup;
    AddDrop addDrop;

    // Прокрутка col2
    float col2ScrollY  = 0.f;
    float col2TotalH   = 600.f;

    // Тултип легенды
    int          legendHover = -1;  // 0=пех, 1=танк, 2=арт
    sf::Vector2f legendHoverPos;

    void drawTopBar();
    void drawGrid();
    void drawTerrain();
    void drawConnectors();
    void drawEnemyArrows();
    void drawNodes();
    void drawHoverTooltip(int idx);
    void drawBottomPanel();
    void drawCol1(float x, float y, float w, float h);
    void drawCol2(float x, float y, float w, float h);
    void drawCol3(float x, float y, float w, float h);
    void drawLegendTooltip();
    void drawPopup();
    void drawAddDrop();
    void drawResultOverlay();

    float calcPopupH() const;
    void  syncPopup();

    // Примитивы
    void rect(float x, float y, float w, float h,
              sf::Color fill, sf::Color border = C::NONE, float thick = 1.f);
    void circle(float cx, float cy, float r,
                sf::Color fill, sf::Color border, float thick = 2.f);
    void line(sf::Vector2f a, sf::Vector2f b, sf::Color col, float thick = 1.f);
    void dashedLine(sf::Vector2f a, sf::Vector2f b, sf::Color col,
                    float dash = 12.f, float gap = 8.f);
    void drawTxt(const std::string& s, unsigned sz,
                 sf::Color col, float x, float y);
    // Текст с отсечкой по Y (для скролла)
    bool txtVisible(float y, unsigned sz, float clipY, float clipH) const;
    void arrowHead(sf::Vector2f tip, sf::Vector2f dir,
                   sf::Color col, float sz = 14.f);
    void drawStars(int n, float x, float y, float sz = 32.f);

    sf::Color nodeColor(int idx) const;
    bool      nodeHit(int idx, sf::Vector2f p) const;
};
