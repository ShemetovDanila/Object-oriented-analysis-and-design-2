// Menu.cpp
#include "Menu.h"
#include <cmath>
#include <sstream>

Menu::Menu(sf::RenderWindow& w, sf::Font& f) : win(w), font(f) {
    levels = {
        {0,"ВЫСОТА 307","Оборонительная операция",
         "Противник наносит удар тремя группами с разных направлений.\n"
         "Бронеколонна наступает с запада, пехота давит в центре,\n"
         "разведка просачивается с востока.\n\n"
         "Удержите высоту до подхода подкрепления.",
         "Бюджет: 100 очков","3 рубежа: ЗАПАДНЫЙ / ЦЕНТРАЛЬНЫЙ / ВОСТОЧНЫЙ"},
        {1,"ПЕРЕПРАВА","Наступательная операция",
         "Противник занял оборону на другом берегу реки.\n"
         "Два укреплённых узла прикрывают мост и правый фланг.\n\n"
         "Захватите мост и удержите плацдарм.\n"
         "Используйте взводы для прорыва укреплённой обороны.",
         "Бюджет: 150 очков","2 рубежа: ЦЕНТРАЛЬНЫЙ / ПРАВЫЙ"},
    };
}

void Menu::draw() {
    drawBackground();
    float divX = W * 0.5f;
    line({ divX, 80.f }, { divX, H - 40.f }, sf::Color(35, 70, 35), 1.f);
    drawLeftPanel(0, 0, divX, H);
    drawRightPanel(divX, 0, W - divX, H);
}

void Menu::drawBackground() {
    win.clear(sf::Color(10, 18, 10));
    for (float x = 0; x < W; x += 80.f) {
        sf::RectangleShape ln(sf::Vector2f(1.f, H));
        ln.setPosition(sf::Vector2f(x, 0.f));
        ln.setFillColor(sf::Color(18, 32, 18));
        win.draw(ln);
    }
    for (float y = 0; y < H; y += 80.f) {
        sf::RectangleShape ln(sf::Vector2f(W, 1.f));
        ln.setPosition(sf::Vector2f(0.f, y));
        ln.setFillColor(sf::Color(18, 32, 18));
        win.draw(ln);
    }
    rect(0, 0, W, 70.f, sf::Color(14, 22, 14), sf::Color(35, 70, 35));
    rect(0, 68.f, W, 2.f, sf::Color(35, 110, 35), sf::Color(0, 0, 0, 0));
    drawTxt("КОМАНДИР", 28, sf::Color(90, 255, 90), 40.f, 16.f);
    drawTxt("ТАКТИЧЕСКАЯ КОМАНДНАЯ СИСТЕМА  |  ГЛАВНОЕ МЕНЮ", 12, sf::Color(80, 112, 72), 260.f, 24.f);
    drawTxt("v2.0  [Composite Pattern]", 11, sf::Color(50, 90, 50), W - 220.f, 26.f);
}

void Menu::drawLeftPanel(float x, float y, float w, float h) {
    float px = x + 60.f, py = y + 100.f;
    drawTxt("О ИГРЕ", 11, sf::Color(80, 112, 72), px, py); py += 28.f;
    drawTxt("Вы — командир", 36, sf::Color(90, 255, 90), px, py); py += 52.f;
    drawTxt("военного подразделения", 36, sf::Color(90, 255, 90), px, py); py += 70.f;
    struct Para { const char* text; unsigned sz; sf::Color col; float gap; };
    Para paras[] = {
        {"Получайте боевые задачи и принимайте тактические решения.",14,sf::Color(195,218,185),22.f},
        {"От вас зависит исход каждого сражения.",14,sf::Color(195,218,185),38.f},
        {"Закупайте юнитов, формируйте отделения и взводы,",14,sf::Color(195,218,185),22.f},
        {"расставляйте силы по рубежам — и идите в бой.",14,sf::Color(195,218,185),50.f},
    };
    for (auto& p : paras) { drawTxt(p.text, p.sz, p.col, px, py); py += p.gap; }
    rect(px, py, w - 120.f, 1.f, sf::Color(35, 70, 35), sf::Color(0, 0, 0, 0)); py += 16.f;
    drawTxt("УПРАВЛЕНИЕ", 11, sf::Color(80, 112, 72), px, py); py += 26.f;
    struct Ctrl { const char* key; const char* desc; };
    Ctrl ctrls[] = {
        {"НАЖАТЬ НА РУБЕЖ", "Открыть окно управления составом"},
        {"+ ДОБАВИТЬ",       "Выбрать юнит / отделение / взвод"},
        {"УБРАТЬ",           "Снять подразделение (бюджет возвращается)"},
        {"ВЫПОЛНИТЬ ПРИКАЗ", "Начать бой и получить оценку"},
        {"R",                "Перезапустить миссию"},
        {"ESC",              "Вернуться в меню"},
    };
    for (auto& c : ctrls) {
        rect(px, py, w - 120.f, 38.f, sf::Color(16, 28, 16), sf::Color(35, 70, 35));
        rect(px, py, 3.f, 38.f, sf::Color(35, 110, 35), sf::Color(0, 0, 0, 0));
        drawTxt(c.key, 12, sf::Color(90, 255, 90), px + 14.f, py + 5.f);
        drawTxt(c.desc, 11, sf::Color(120, 160, 110), px + 14.f, py + 22.f);
        py += 46.f;
    }
    py += 20.f;
    rect(px, py, w - 120.f, 1.f, sf::Color(35, 70, 35), sf::Color(0, 0, 0, 0)); py += 20.f;
    drawTxt("СИСТЕМА ОЦЕНКИ", 11, sf::Color(80, 112, 72), px, py); py += 24.f;
    struct Star { const char* s; const char* desc; sf::Color col; };
    Star stars[] = {
        {"★★★","Блестяще — все рубежи закрыты надёжно",sf::Color(255,215,0)},
        {"★★☆","Выполнено — есть слабые места",sf::Color(255,170,0)},
        {"★☆☆","С потерями — серьёзные ошибки",sf::Color(255,100,50)},
        {"☆☆☆","Провал — позиция потеряна",sf::Color(255,65,65)},
    };
    for (auto& s : stars) {
        drawTxt(s.s, 14, s.col, px, py);
        drawTxt(s.desc, 12, sf::Color(150, 190, 140), px + 56.f, py + 1.f);
        py += 24.f;
    }
}

void Menu::drawRightPanel(float x, float y, float w, float h) {
    float px = x + 50.f, py = y + 100.f;
    drawTxt("ВЫБЕРИТЕ МИССИЮ", 11, sf::Color(80, 112, 72), px, py); py += 28.f;
    drawTxt("Доступные операции", 24, sf::Color(195, 218, 185), px, py); py += 50.f;
    float cardH = 230.f, cardW = w - 100.f;
    for (int i = 0; i < (int)levels.size(); i++) {
        drawLevelCard(levels[i], px, py, cardW, cardH, hoveredLevel == i);
        py += cardH + 20.f;
    }
    py += 20.f;
    drawTxt("Нажмите на миссию чтобы начать", 12, sf::Color(60, 100, 60), px, py);
}

void Menu::drawLevelCard(const LevelInfo& lv, float x, float y, float w, float h, bool hovered) {
    sf::Color fillCol = hovered ? sf::Color(20, 42, 20) : sf::Color(14, 24, 14);
    sf::Color bordCol = hovered ? sf::Color(90, 255, 90) : sf::Color(50, 100, 50);
    rect(x, y, w, h, fillCol, bordCol, hovered ? 2.f : 1.f);
    std::string numStr = "0" + std::to_string(lv.index + 1);

    // ИСПРАВЛЕНИЕ: Передаем шрифт прямо в конструктор
    sf::Text numT(font, sf::String::fromUtf8(numStr.begin(), numStr.end()), 72);
    numT.setFillColor(hovered ? sf::Color(30, 80, 30) : sf::Color(22, 45, 22));
    numT.setPosition(sf::Vector2f(x + w - 100.f, y + h * 0.5f - 44.f));
    win.draw(numT);

    rect(x, y, 4.f, h, hovered ? sf::Color(90, 255, 90) : sf::Color(50, 120, 50), sf::Color(0, 0, 0, 0));
    float px = x + 20.f, py = y + 16.f;
    drawTxt(lv.subtitle, 11, sf::Color(80, 112, 72), px, py); py += 22.f;
    drawTxt(lv.title, 26, hovered ? sf::Color(90, 255, 90) : sf::Color(195, 218, 185), px, py); py += 40.f;
    std::istringstream iss(lv.description); std::string ln2; int cnt = 0;
    while (std::getline(iss, ln2) && cnt < 4) {
        if (ln2.empty()) { py += 8.f; continue; }
        drawTxt(ln2, 12, sf::Color(130, 165, 120), px, py); py += 19.f; cnt++;
    }
    py = y + h - 48.f;
    rect(px, py, w - 40.f, 1.f, sf::Color(35, 70, 35), sf::Color(0, 0, 0, 0)); py += 10.f;
    drawTxt(lv.budget, 11, sf::Color(255, 170, 0), px, py);
    drawTxt(lv.directions, 11, sf::Color(80, 112, 72), px + 190.f, py);
    if (hovered) drawTxt("НАЧАТЬ  ->", 13, sf::Color(90, 255, 90), x + w - 140.f, y + h - 34.f);
}

void Menu::handleMouseMove(sf::Vector2f pos) {
    hoveredLevel = -1;
    float x = W * 0.5f + 50.f, py = 100.f + 28.f + 50.f;
    float cardH = 230.f, cardW = W * 0.5f - 100.f;
    for (int i = 0; i < (int)levels.size(); i++) {
        // ИСПРАВЛЕНИЕ: Новый конструктор FloatRect для SFML 3 (Position, Size)
        sf::FloatRect r(sf::Vector2f(x, py), sf::Vector2f(cardW, cardH));

        if (r.contains(pos)) { hoveredLevel = i; break; }
        py += cardH + 20.f;
    }
}

void Menu::handleMouseClick(sf::Vector2f pos) {
    float x = W * 0.5f + 50.f, py = 100.f + 28.f + 50.f;
    float cardH = 230.f, cardW = W * 0.5f - 100.f;
    for (int i = 0; i < (int)levels.size(); i++) {
        // ИСПРАВЛЕНИЕ: Новый конструктор FloatRect для SFML 3 (Position, Size)
        sf::FloatRect r(sf::Vector2f(x, py), sf::Vector2f(cardW, cardH));

        if (r.contains(pos)) { selectedLevel = i; return; }
        py += cardH + 20.f;
    }
}

void Menu::drawTxt(const std::string& s, unsigned sz, sf::Color col, float x, float y) {
    // ИСПРАВЛЕНИЕ: Передаем шрифт и строку в конструктор, так как в SFML 3 нет конструктора по умолчанию
    sf::Text t(font, sf::String::fromUtf8(s.begin(), s.end()), sz);
    t.setFillColor(col);
    t.setPosition(sf::Vector2f(x, y));
    win.draw(t);
}

void Menu::rect(float x, float y, float w, float h, sf::Color fill, sf::Color border, float thick) {
    sf::RectangleShape s(sf::Vector2f(w, h));
    s.setPosition(sf::Vector2f(x, y));
    s.setFillColor(fill);
    if (border.a > 0) { s.setOutlineColor(border); s.setOutlineThickness(thick); }
    win.draw(s);
}

void Menu::line(sf::Vector2f a, sf::Vector2f b, sf::Color col, float thick) {
    sf::Vector2f d = b - a;
    float len = std::sqrt(d.x * d.x + d.y * d.y);
    if (len < 0.01f) return;
    sf::RectangleShape s(sf::Vector2f(len, thick));
    s.setPosition(a);
    constexpr float RAD2DEG = 180.f / 3.14159265358979323846f;
    float angleDeg = std::atan2(d.y, d.x) * RAD2DEG;
    s.setRotation(sf::degrees(angleDeg)); // ИСПРАВЛЕНИЕ: В SFML 3 setRotation принимает sf::Angle, используем sf::degrees()
    s.setFillColor(col);
    win.draw(s);
}