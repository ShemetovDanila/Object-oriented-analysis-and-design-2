// Renderer.cpp
#include "Renderer.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

// Конвертер std::string -> sf::String (совместим с SFML 3.x и старше)
static sf::String toSfString(const std::string& s) {
#if SFML_VERSION_MAJOR >= 3
    return sf::String::fromUtf8(s.begin(), s.end());
#else
    return sf::String(s);
#endif
}

Renderer::Renderer(sf::RenderWindow& w, sf::Font& f, GameState& gs)
    : win(w), font(f), state(gs) {
}

void Renderer::buildMap(Mission* mission) {
    nodes.clear(); hoveredNode = -1;
    popup.visible = false; addDrop.visible = false; addDrop.targetFormation = nullptr;
    wantsNextMission = false; wantsMenu = false;
    col2ScrollY = 0.f; legendHover = -1;

    if (!mission) return;

    int n = (int)mission->directions.size();
    float cy = TOP_H + MAP_H * 0.52f;
    if (n == 3) {
        float topY = TOP_H + MAP_H * 0.24f, step = W / 4.f;
        nodes.push_back({ {step, topY}, 44.f, 0 });
        nodes.push_back({ {step * 2.f, topY}, 44.f, 1 });
        nodes.push_back({ {step * 3.f, topY}, 44.f, 2 });
    }
    else if (n == 2) {
        nodes.push_back({ {W * 0.35f, cy}, 44.f, 0 });
        nodes.push_back({ {W * 0.65f, cy}, 44.f, 1 });
    }
    else {
        float step = W / (float)(n + 1);
        for (int i = 0; i < n; i++) nodes.push_back({ {step * (i + 1), cy}, 44.f, i });
    }
}

void Renderer::draw() {
    win.clear(C::BG);
    drawGrid(); drawTerrain(); drawEnemyArrows(); drawConnectors(); drawNodes();
    if (hoveredNode >= 0 && !popup.visible) drawHoverTooltip(hoveredNode);
    drawTopBar(); drawBottomPanel();
    if (legendHover >= 0) drawLegendTooltip();
    if (popup.visible)   drawPopup();
    if (addDrop.visible) drawAddDrop();
    if (state.phase == GamePhase::RESULT) drawResultOverlay();
}

// ── Шапка ──────────────────────────────────────────────────
void Renderer::drawTopBar() {
    rect(0, 0, W, TOP_H, C::BG_PANEL, C::BORDER);
    rect(0, TOP_H - 2.f, W, 2.f, C::GREEN_DIM, C::NONE);
    drawTxt("КОМАНДИР", 20, C::GREEN, 28.f, 12.f);
    drawTxt("ТАКТИЧЕСКАЯ КОМАНДНАЯ СИСТЕМА", 10, C::TEXT_DIM, 200.f, 17.f);
    if (state.currentMission) drawTxt(state.currentMission->title, 16, C::AMBER, W * 0.5f - 120.f, 14.f);
    if (state.currentMission) {
        std::string bud = "БЮДЖЕТ: " + std::to_string(state.remaining()) + " / " + std::to_string(state.budget) + " ОЧК";
        drawTxt(bud, 14, C::AMBER, W - 370.f, 15.f);
    }
    rect(W - 100.f, 10.f, 88.f, 30.f, sf::Color(12, 22, 12), C::BORDER);
    drawTxt("В МЕНЮ", 12, C::TEXT_DIM, W - 88.f, 17.f);
}

// ── Сетка ──────────────────────────────────────────────────
void Renderer::drawGrid() {
    for (float x = 0; x < W; x += 80.f) line({ x, TOP_H }, { x, TOP_H + MAP_H }, C::GRID);
    for (float y = TOP_H; y < TOP_H + MAP_H; y += 80.f) line({ 0, y }, { W, y }, C::GRID);
}

// ── Рельеф ─────────────────────────────────────────────────
void Renderer::drawTerrain() {
    float my = TOP_H; bool m1 = (state.currentMissionIndex == 0);
    if (m1) {
        struct T { float x, y; const char* s; sf::Color c; unsigned sz; };
        T cells[] = {
            {55,my + 250,"^ ^ ^ ^",C::FOREST,18},{38,my + 278,"^ ^ ^ ^ ^",C::FOREST,18},
            {60,my + 306,"^ ^ ^ ^",C::FOREST,18},{35,my + 334,"^ ^ ^ ^ ^",C::FOREST,18},
            {55,my + 362,"^ ^ ^ ^",C::FOREST,18},
            {1680,my + 260,"^ ^ ^ ^",C::FOREST,18},{1700,my + 288,"^ ^ ^ ^",C::FOREST,18},
            {1685,my + 316,"^ ^ ^ ^ ^",C::FOREST,18},
            {800,my + 270,"/^^\\307",C::HILL,20},{778,my + 298,"/^^^^^^\\",C::HILL,20},
            {772,my + 326,"\\________/",C::HILL,18},
            {1520,my + 90,"~ ~ ~ ~ ~",C::RIVER,18},{1535,my + 118,"~ ~ ~ ~ ~",C::RIVER,18},
            {1520,my + 146,"~ ~ ~ ~ ~",C::RIVER,18},{1530,my + 174,"~ ~ ~ ~ ~",C::RIVER,18},
            {1518,my + 202,"~ ~ ~ ~ ~",C::RIVER,18},{1525,my + 230,"~ ~ ~ ~ ~",C::RIVER,18},
        };
        for (auto& c : cells) drawTxt(c.s, c.sz, c.c, c.x, c.y);
        drawTxt("- - - - ЛИНИЯ ОБОРОНЫ - - - -", 13, C::GREEN_DIM, W * 0.5f - 160.f, my + MAP_H - 22.f);
    }
    else {
        for (float ry = my + 40; ry < my + MAP_H - 28; ry += 26.f) {
            float ox = 6.f * std::sin(ry * 0.05f);
            drawTxt("~ ~ ~ ~ ~ ~ ~", 18, C::RIVER, W * 0.5f - 70.f + ox, ry);
        }
        drawTxt("=======", 18, C::AMBER, W * 0.5f - 42.f, my + MAP_H * 0.5f - 12.f);
        drawTxt("М О С Т", 13, C::AMBER_DIM, W * 0.5f - 30.f, my + MAP_H * 0.5f + 14.f);
        drawTxt("НАШ БЕРЕГ", 14, C::GREEN_DIM, W * 0.26f, my + MAP_H * 0.5f);
        drawTxt("ПР-К", 14, C::RED_DIM, W * 0.64f, my + MAP_H * 0.5f);
    }
}

void Renderer::drawConnectors() {
    int n = (int)nodes.size();
    for (int i = 0; i < n - 1; i++) line(nodes[i].pos, nodes[i + 1].pos, C::GREEN_DIM, 2.f);
    if (n == 3) line(nodes[0].pos, nodes[2].pos, sf::Color(28, 75, 28), 1.5f);
}

void Renderer::drawEnemyArrows() {
    if (!state.currentMission) return;
    for (int i = 0; i < (int)nodes.size(); i++) {
        sf::Vector2f tip = nodes[i].pos; sf::Color col = nodeColor(i);
        const auto& d = state.currentMission->directions[i];
        dashedLine({ tip.x, TOP_H + 6.f }, { tip.x, tip.y - nodes[i].r - 4.f }, col, 16.f, 8.f);
        arrowHead({ tip.x, tip.y - nodes[i].r - 4.f }, { 0.f, 1.f }, col, 16.f);
        drawTxt(d.getEnemyTypeText() + " x" + std::to_string(d.enemy_power), 13, col, tip.x - 50.f, TOP_H + 10.f);
    }
}

void Renderer::drawNodes() {
    if (!state.currentMission) return;
    for (int i = 0; i < (int)nodes.size(); i++) {
        const auto& nd = nodes[i]; const auto& d = state.currentMission->directions[nd.dirIdx];
        sf::Color col = nodeColor(i); bool hov = (hoveredNode == i), sel = (popup.visible && popup.dirIdx == i);
        if (sel) circle(nd.pos.x, nd.pos.y, nd.r + 16.f, C::NONE, sf::Color(col.r, col.g, col.b, 65), 2.5f);
        else if (hov) circle(nd.pos.x, nd.pos.y, nd.r + 10.f, C::NONE, sf::Color(col.r, col.g, col.b, 40), 2.f);
        circle(nd.pos.x, nd.pos.y, nd.r, sel ? sf::Color(22, 58, 22) : hov ? sf::Color(14, 40, 14) : sf::Color(10, 24, 10), col, sel ? 3.5f : 2.f);
        const std::string& lbl = d.label;
        std::string ltr = (lbl.find("ЗАП") != std::string::npos) ? "З" :
            (lbl.find("ЦЕНТР") != std::string::npos) ? "Ц" :
            (lbl.find("ВОСТ") != std::string::npos) ? "В" :
            (lbl.find("ПРАВ") != std::string::npos) ? "П" : std::to_string(i + 1);
        drawTxt(ltr, 24, col, nd.pos.x - 10.f, nd.pos.y - 15.f);
        drawTxt(d.label, 12, C::TEXT_DIM, nd.pos.x - (float)d.label.size() * 3.6f, nd.pos.y + nd.r + 8.f);
        if (!d.units.empty())
            drawTxt("[" + std::to_string(d.units.size()) + " ед.]", 11, C::GREEN, nd.pos.x - 22.f, nd.pos.y + nd.r + 24.f);
    }
}

void Renderer::drawHoverTooltip(int idx) {
    if (!state.currentMission || idx < 0 || idx >= (int)nodes.size()) return;
    const auto& nd = nodes[idx]; const auto& d = state.currentMission->directions[nd.dirIdx];
    float tw = 265.f, th = 105.f; float tx = nd.pos.x + nd.r + 16.f, ty = nd.pos.y - th * 0.5f;
    if (tx + tw > W - 8.f) tx = nd.pos.x - nd.r - tw - 16.f;
    if (ty < TOP_H + 4.f) ty = TOP_H + 4.f;
    rect(tx, ty, tw, th, C::BG_CARD, nodeColor(idx));
    float py = ty + 10.f;
    auto ln = [&](const std::string& s, unsigned sz, sf::Color c) { drawTxt(s, sz, c, tx + 12.f, py); py += sz * 1.75f; };
    ln(d.label, 13, nodeColor(idx));
    ln(d.getEnemyTypeText() + " x" + std::to_string(d.enemy_power), 11, C::AMBER);
    std::ostringstream fp; fp << std::fixed << std::setprecision(0) << d.getFriendlyPower();
    ln("Наша мощь: " + fp.str(), 11, C::GREEN);
    ln("[нажми чтобы открыть]", 10, C::TEXT_DIM);
}

// ── Нижняя панель ───────────────────────────────────────────
void Renderer::drawBottomPanel() {
    rect(0, PAN_Y, W, PAN_H, C::BG_PANEL, C::NONE);
    rect(0, PAN_Y, W, 2.f, C::GREEN_DIM, C::NONE);
    float col2x = COL1_W, col3x = COL1_W + COL2_W, col3w = W - col3x;
    rect(col2x, PAN_Y, 1.f, PAN_H, C::BORDER, C::NONE);
    rect(col3x, PAN_Y, 1.f, PAN_H, C::BORDER, C::NONE);
    drawCol1(0, PAN_Y, COL1_W, PAN_H);
    drawCol2(col2x, PAN_Y, COL2_W, PAN_H);
    drawCol3(col3x, PAN_Y, col3w, PAN_H);
}

void Renderer::drawCol1(float x, float y, float w, float h) {
    if (!state.currentMission) return;
    float px = x + 18.f, py = y + 12.f;
    drawTxt("БОЕВОЙ ПРИКАЗ", 10, C::TEXT_DIM, px, py); py += 20.f;
    drawTxt(state.currentMission->title, 17, C::AMBER, px, py); py += 30.f;
    std::istringstream iss(state.currentMission->briefing); std::string ln2;
    while (std::getline(iss, ln2)) { drawTxt(ln2, 12, C::TEXT, px, py); py += 19.f; }
    py += 8.f;
    drawTxt("УГРОЗЫ ПО РУБЕЖАМ", 10, C::TEXT_DIM, px, py); py += 16.f;
    for (int i = 0; i < (int)state.currentMission->directions.size(); i++) {
        const auto& d = state.currentMission->directions[i]; sf::Color col = nodeColor(i);
        rect(px, py, w - 36.f, 46.f, C::BG_CARD, col); rect(px, py, 4.f, 46.f, col, C::NONE);
        drawTxt(d.label, 12, col, px + 10.f, py + 5.f);
        drawTxt(d.getEnemyTypeText() + "  |  мощь: " + std::to_string(d.enemy_power), 11, C::TEXT, px + 10.f, py + 24.f);
        py += 54.f;
    }
}

void Renderer::drawCol2(float x, float y, float w, float h) {
    float px = x + 16.f;
    float scrollTop = y + 28.f, scrollH = h - 30.f;
    float maxScroll = std::max(0.f, col2TotalH - scrollH);
    col2ScrollY = std::clamp(col2ScrollY, 0.f, maxScroll);
    drawTxt("ЮНИТЫ И ПОДРАЗДЕЛЕНИЯ", 10, C::TEXT_DIM, px, y + 10.f);
    if (col2TotalH > scrollH) {
        float barH = scrollH * (scrollH / col2TotalH);
        float barY = scrollTop + (col2ScrollY / std::max(1.f, maxScroll)) * (scrollH - barH);
        rect(x + w - 8.f, scrollTop, 6.f, scrollH, sf::Color(8, 18, 8), C::BORDER);
        rect(x + w - 7.f, barY, 4.f, barH, C::GREEN_DIM, C::NONE);
    }
    float cy = scrollTop - col2ScrollY;
    auto vis = [&](float iy, float ih)->bool { return iy + ih > scrollTop && iy < scrollTop + scrollH; };
    auto item = [&](float ih, auto fn) { if (vis(cy, ih)) fn(cy); cy += ih; };

    item(16.f, [&](float iy) { drawTxt("── ЮНИТЫ ─────────────────────────────", 10, C::GREEN_DIM, px, iy); });
    struct UI { const char* name; int cost; float pow; const char* type; };
    UI units[] = { {"Пехотинец",5,5.f,"INFANTRY"},{"Танк",25,0.f,"VEHICLE"},{"Артиллерия",15,0.f,"VEHICLE"} };
    for (auto& u : units) {
        item(42.f, [&](float iy) {
            rect(px, iy, w - 34.f, 40.f, C::BG_CARD, C::BORDER);
            drawTxt(u.name, 12, C::TEXT, px + 8.f, iy + 5.f);
            drawTxt(std::to_string(u.cost) + " ОЧК", 10, C::AMBER, px + w - 80.f, iy + 7.f);
            std::string pw = u.pow > 0 ? ("мощь: " + std::to_string((int)u.pow)) : "мощь: 0 (бонус в отделении)";
            drawTxt(pw, 9, C::TEXT_DIM, px + 8.f, iy + 24.f);
            });
    }
    cy += 6.f;
    item(16.f, [&](float iy) { drawTxt("── ОТДЕЛЕНИЯ (наведи = бонус) ─────────", 10, C::GREEN_DIM, px, iy); });
    struct SI { const char* name; const char* cond; int li; };
    SI sqs[] = { {"Пех. отделение","3+ пехотинца → мощь × 2",0},
                 {"Танк. отделение","Танк + 3+ пехотинца → +50",1},
                 {"Арт. отделение","Арт + 2+ пехотинца → +30",2} };
    for (auto& s : sqs) {
        item(50.f, [&](float iy) {
            bool hov = (legendHover == s.li);
            rect(px, iy, w - 34.f, 48.f, hov ? C::BG_SEL : C::BG_CARD, hov ? C::GREEN : C::GREEN_DIM);
            rect(px, iy, 4.f, 48.f, hov ? C::GREEN : C::GREEN_DIM, C::NONE);
            drawTxt(s.name, 12, hov ? C::GREEN : C::TEXT, px + 10.f, iy + 6.f);
            drawTxt(s.cond, 9, C::TEXT_DIM, px + 10.f, iy + 26.f);
            drawTxt("[?]", 9, C::GREEN_DIM, px + w - 46.f, iy + 6.f);
            });
    }
    cy += 8.f;
    item(16.f, [&](float iy) { drawTxt("── ВЗВОД ──────────────────────────────", 10, C::GREEN_DIM, px, iy); });
    item(58.f, [&](float iy) {
        rect(px, iy, w - 34.f, 56.f, C::BG_CARD, C::GOLD); rect(px, iy, 4.f, 56.f, C::GOLD, C::NONE);
        drawTxt("Взвод", 13, C::GOLD, px + 10.f, iy + 6.f);
        drawTxt("Контейнер для отделений", 9, C::TEXT_DIM, px + 10.f, iy + 24.f);
        drawTxt("3+ отд. с бонусом → мощь × 1.5", 9, C::TEXT_DIM, px + 10.f, iy + 38.f);
        });
    cy += 4.f;
    item(14.f, [&](float iy) { drawTxt("* isComposite() = true у Formation", 9, C::TEXT_DIM, px, iy); });
    col2TotalH = cy - scrollTop + col2ScrollY + 10.f;
}

void Renderer::drawLegendTooltip() {
    struct Info { const char* title; const char* req; const char* power; const char* spec; };
    Info infos[] = {
        {"ПЕХ. ОТДЕЛЕНИЕ","3+ пехотинцев (INFANTRY)","raw × 2","vs ПЕХОТА ×1.0 | vs БРОНЯ ×0.4"},
        {"ТАНК. ОТДЕЛЕНИЕ","Танк (VEHICLE) + 3+ пехотинца","raw + 50","vs ПЕХОТА ×0.6 | vs БРОНЯ ×2.0"},
        {"АРТ. ОТДЕЛЕНИЕ","Артиллерия (VEHICLE) + 2+ пехотинца","raw + 30","vs ПЕХОТА ×1.4 | vs БРОНЯ ×0.7"},
    };
    if (legendHover < 0 || legendHover > 2) return;
    const Info& inf = infos[legendHover];
    float tw = 330.f, th = 145.f;
    float tx = legendHoverPos.x + 14.f, ty = legendHoverPos.y - 10.f;
    if (tx + tw > W - 8.f) tx = legendHoverPos.x - tw - 14.f;
    if (ty < TOP_H) ty = TOP_H + 4.f; if (ty + th > H - 4.f) ty = H - th - 4.f;
    rect(tx, ty, tw, th, C::BG_POPUP, C::GREEN, 1.5f); rect(tx, ty, tw, 3.f, C::GREEN, C::NONE);
    float py = ty + 10.f;
    auto ln = [&](const std::string& s, unsigned sz, sf::Color c) { drawTxt(s, sz, c, tx + 10.f, py); py += sz * 1.7f; };
    ln(inf.title, 13, C::GREEN); py += 2.f;
    ln("УСЛОВИЕ БОНУСА:", 10, C::TEXT_DIM); ln(inf.req, 11, C::AMBER); py += 2.f;
    ln("БОНУС:", 10, C::TEXT_DIM); ln(inf.power, 11, C::GREEN); py += 2.f;
    ln("СПЕЦ. КОЭФФИЦИЕНТ:", 10, C::TEXT_DIM); ln(inf.spec, 11, C::TEXT);
}

void Renderer::drawCol3(float x, float y, float w, float h) {
    float px = x + 18.f, py = y + 12.f;
    drawTxt("УПРАВЛЕНИЕ", 10, C::TEXT_DIM, px, py); py += 20.f;
    drawTxt("Нажми на рубеж — откроется окно", 12, C::TEXT, px, py); py += 18.f;
    drawTxt("управления составом.", 12, C::TEXT, px, py); py += 34.f;
    rect(px, py, w - 36.f, 48.f, sf::Color(12, 45, 12), C::GREEN, 2.f);
    drawTxt("[ ВЫПОЛНИТЬ ПРИКАЗ ]", 15, C::GREEN, px + 22.f, py + 13.f);
    if (state.currentMission) {
        py += 60.f;
        drawTxt("СТАТУС РУБЕЖЕЙ:", 10, C::TEXT_DIM, px, py); py += 16.f;
        for (int i = 0; i < (int)state.currentMission->directions.size(); i++) {
            const auto& d = state.currentMission->directions[i];
            std::ostringstream ss;
            ss << d.label << ": " << std::fixed << std::setprecision(0)
                << d.getFriendlyPower() << " / " << d.enemy_power << " (" << d.getResultText() << ")";
            drawTxt(ss.str(), 11, nodeColor(i), px, py); py += 19.f;
        }
    }
}

// ============================================================
//  Попап — рекурсивное дерево через isComposite()
// ============================================================
float Renderer::calcPopupH() const {
    if (!state.currentMission || popup.dirIdx < 0) return 200.f;
    const auto& d = state.currentMission->directions[popup.dirIdx];
    float h = 52.f + 34.f + 14.f;
    for (int i = 0; i < (int)d.units.size(); i++) {
        const IMilitaryUnit* u = d.units[i];
        bool exp = (i < (int)popup.expanded.size()) ? popup.expanded[i] : true;
        h += 36.f; // заголовок
        if (exp && u->isComposite()) {
            auto children = u->getChildren();
            for (int ci = 0; ci < (int)children.size(); ci++) {
                const IMilitaryUnit* ch = children[ci];
                bool cexp = (i < (int)popup.childExpanded.size() && ci < (int)popup.childExpanded[i].size())
                    ? popup.childExpanded[i][ci] : true;
                h += 30.f;
                if (cexp && ch->isComposite()) {
                    h += (float)ch->getChildren().size() * 24.f + 26.f;
                }
                else if (cexp) {
                    h += 26.f;
                }
                h += 4.f;
            }
            h += 28.f; // кнопка добавить в Formation
        }
        else if (exp && !u->isComposite()) {
            h += 0.f;
        }
        h += 4.f;
    }
    h += 32.f + 40.f;
    return std::min(h, H - TOP_H - 20.f);
}

void Renderer::drawPopup() {
    if (!state.currentMission || popup.dirIdx < 0) return;
    const auto& d = state.currentMission->directions[popup.dirIdx];
    popup.sync((int)d.units.size());
    sf::Color col = nodeColor(popup.dirIdx);
    float pw = POPUP_W, ph = calcPopupH();
    float px = std::min(popup.x, W - pw - 6.f);
    float py = std::min(popup.y, TOP_H + MAP_H - ph - 4.f);
    if (py < TOP_H + 4.f) py = TOP_H + 4.f; if (px < 4.f) px = 4.f;

    rect(px + 4.f, py + 4.f, pw, ph, sf::Color(0, 0, 0, 80), C::NONE);
    rect(px, py, pw, ph, C::BG_POPUP, col, 1.f);
    rect(px, py, pw, 3.f, col, C::NONE);

    // Заголовок
    rect(px, py + 3.f, pw, 48.f, sf::Color(10, 18, 10), C::NONE);
    drawTxt(d.label, 15, col, px + 12.f, py + 8.f);
    drawTxt("Угроза: " + d.getEnemyTypeText() + " x" + std::to_string(d.enemy_power), 11, C::TEXT_DIM, px + 12.f, py + 28.f);
    rect(px + pw - 38.f, py + 8.f, 30.f, 22.f, C::BG_CARD, C::BORDER);
    drawTxt("[x]", 10, C::TEXT_DIM, px + pw - 35.f, py + 12.f);

    float cy = py + 52.f;
    rect(px + 6.f, cy, pw - 12.f, 30.f, sf::Color(8, 16, 8), C::BORDER);
    std::ostringstream fp; fp << std::fixed << std::setprecision(0) << d.getFriendlyPower();
    drawTxt("Наша мощь: " + fp.str(), 11, C::GREEN, px + 12.f, cy + 7.f);
    drawTxt("Угроза: " + std::to_string(d.enemy_power), 11, C::RED, px + pw * 0.46f, cy + 7.f);
    float sc = d.getScore(); std::string stL; sf::Color stC;
    if (sc >= 6.f) { stL = "ПОДАВЛЕНО"; stC = C::GREEN; }
    else if (sc >= 0.f) { stL = "УДЕРЖАНО"; stC = C::GREEN_DIM; }
    else if (sc >= -5.f) { stL = "ПРОРЫВ!"; stC = C::AMBER; }
    else { stL = "ПРОРЫВ!"; stC = C::RED; }
    drawTxt(stL, 10, stC, px + pw * 0.72f, cy + 8.f);
    cy += 36.f;
    drawTxt("СОСТАВ:", 10, C::TEXT_DIM, px + 8.f, cy); cy += 14.f;

    // Рекурсивное дерево
    for (int i = 0; i < (int)d.units.size(); i++) {
        IMilitaryUnit* u = d.units[i];
        bool exp = popup.expanded[i];
        bool bon = u->hasBonus();
        sf::Color uc = bon ? C::GREEN : (u->isComposite() ? C::AMBER : C::TEXT);

        // Строка верхнего уровня
        rect(px + 4.f, cy, pw - 8.f, 32.f, C::BG_CARD, uc);
        drawTxt(exp ? "▼" : "▶", 10, C::TEXT_DIM, px + 10.f, cy + 8.f);
        drawTxt(u->getName(), 12, uc, px + 26.f, cy + 7.f);
        std::ostringstream ps; ps << std::fixed << std::setprecision(0) << u->calculatePower();
        drawTxt("мощь:" + ps.str() + (bon ? " БОН" : ""), 10, C::AMBER, px + pw * 0.50f, cy + 9.f);
        // Кнопка УБРАТЬ
        rect(px + pw - 50.f, cy + 5.f, 42.f, 22.f, C::BG_CARD, C::RED_DIM);
        drawTxt("УБРАТЬ", 8, C::RED, px + pw - 48.f, cy + 10.f);
        cy += 36.f;

        if (exp && u->isComposite()) {
            while ((int)popup.childExpanded[i].size() < (int)u->getChildren().size())
                popup.childExpanded[i].push_back(true);

            auto children = u->getChildren();
            for (int ci = 0; ci < (int)children.size(); ci++) {
                IMilitaryUnit* ch = children[ci];
                bool cexp = popup.childExpanded[i][ci];
                bool cbon = ch->hasBonus();
                sf::Color cc = cbon ? C::GREEN : (ch->isComposite() ? C::AMBER : C::TEXT_DIM);

                rect(px + 14.f, cy, pw - 20.f, 28.f, C::BG_CARD, cc);
                drawTxt(cexp ? "▼" : "▶", 9, C::TEXT_DIM, px + 20.f, cy + 7.f);
                drawTxt(ch->getName(), 11, cc, px + 34.f, cy + 6.f);
                std::ostringstream cs2; cs2 << std::fixed << std::setprecision(0) << ch->calculatePower();
                drawTxt("мощь:" + cs2.str() + (cbon ? " БОН" : ""), 9, C::AMBER, px + pw * 0.52f, cy + 8.f);
                // Кнопка УБРАТЬ дочерний
                rect(px + pw - 44.f, cy + 4.f, 36.f, 18.f, C::BG_CARD, C::RED_DIM);
                drawTxt("УБР", 8, C::RED, px + pw - 42.f, cy + 8.f);
                cy += 30.f;

                if (cexp && ch->isComposite()) {
                    for (auto* gc : ch->getChildren()) {
                        rect(px + 24.f, cy, pw - 30.f, 22.f, sf::Color(8, 14, 8), C::BORDER);
                        drawTxt("└", 9, C::GREEN_DIM, px + 28.f, cy + 5.f);
                        drawTxt(gc->getName(), 10, C::TEXT, px + 40.f, cy + 5.f);
                        drawTxt(std::to_string(gc->getTotalCost()) + " оч", 9, C::AMBER_DIM, px + pw * 0.62f, cy + 6.f);
                        rect(px + pw - 36.f, cy + 1.f, 28.f, 18.f, C::BG_CARD, C::RED_DIM);
                        drawTxt("x", 9, C::RED, px + pw - 25.f, cy + 4.f);
                        cy += 24.f;
                    }
                    // + добавить юнита в дочернее Formation
                    rect(px + 24.f, cy, pw - 30.f, 22.f, C::BG_POPUP, C::GREEN_DIM);
                    drawTxt("+ добавить юнита", 9, C::GREEN_DIM, px + 34.f, cy + 5.f);
                    cy += 26.f;
                }
                else if (cexp && !ch->isComposite()) {
                    // Одиночный юнит уже показан
                }
                cy += 4.f;
            }
            // + добавить в верхнее Formation (отделение или взвод)
            rect(px + 12.f, cy, pw - 18.f, 24.f, sf::Color(8, 18, 8), C::AMBER_DIM);
            drawTxt("+ добавить в " + u->getName(), 10, C::AMBER, px + 20.f, cy + 6.f);
            cy += 28.f;
        }
        cy += 4.f;
    }

    // Кнопка + добавить на рубеж
    rect(px + 4.f, cy, pw - 8.f, 28.f, sf::Color(8, 20, 8), C::GREEN_DIM);
    drawTxt("+ добавить юнит / отделение / взвод", 11, C::GREEN, px + 14.f, cy + 7.f);
    cy += 32.f;

    rect(px, cy, pw, 1.f, C::BORDER, C::NONE);
    rect(px + 4.f, cy + 6.f, pw - 8.f, 24.f, C::BG_CARD, C::BORDER);
    drawTxt("[ ЗАКРЫТЬ ]", 11, C::TEXT_DIM, px + pw * 0.5f - 44.f, cy + 10.f);
}

// ── Дропдаун ────────────────────────────────────────────────
void Renderer::drawAddDrop() {
    float dw = 360.f, px = std::min(addDrop.x, W - dw - 6.f), py = addDrop.y;
    if (py + 400.f > H) py = H - 400.f;

    if (addDrop.mode == 1) {
        // Добавить юнит в Formation
        float dh = 28.f + 3 * 36.f + 8.f;
        rect(px, py, dw, dh, C::BG_POPUP, C::GREEN_DIM);
        float iy = py + 8.f;
        drawTxt("ДОБАВИТЬ ЮНИТА:", 10, C::TEXT_DIM, px + 10.f, iy); iy += 20.f;
        // Временные объекты для отображения
        Unit* tmp1 = UnitFactory::makeRifleman();
        Unit* tmp2 = UnitFactory::makeTank();
        Unit* tmp3 = UnitFactory::makeArtillery();
        Unit* cat[] = { tmp1, tmp2, tmp3 };
        for (int i = 0; i < 3; i++) {
            Unit* u = cat[i];
            bool can = (state.remaining() >= u->cost);
            rect(px + 6.f, iy, dw - 12.f, 32.f, C::BG_CARD, can ? C::BORDER : C::RED_DIM);
            drawTxt(u->getName(), 12, can ? C::TEXT : C::TEXT_DIM, px + 14.f, iy + 8.f);
            drawTxt(std::to_string(u->cost) + " ОЧК", 10, C::AMBER, px + dw - 68.f, iy + 9.f);
            iy += 36.f;
        }
        delete tmp1; delete tmp2; delete tmp3;
        return;
    }

    // mode==0: добавить на рубеж
    float dh = 28.f + 3 * 22.f + 4.f + 22.f + 3 * 52.f + 4.f + 56.f + 8.f;
    rect(px, py, dw, dh, C::BG_POPUP, C::GREEN_DIM);
    float iy = py + 8.f;
    drawTxt("ДОБАВИТЬ НА РУБЕЖ:", 10, C::TEXT_DIM, px + 10.f, iy); iy += 20.f;

    drawTxt("ЮНИТЫ:", 9, C::TEXT_DIM, px + 10.f, iy); iy += 18.f;
    const char* unames[] = { "Пехотинец (5 оч)","Танк (25 оч)","Артиллерия (15 оч)" };
    int ucosts[] = { 5,25,15 };
    for (int i = 0; i < 3; i++) {
        bool can = (state.remaining() >= ucosts[i]);
        rect(px + 6.f, iy, dw - 12.f, 20.f, C::BG_CARD, can ? C::BORDER : C::RED_DIM);
        drawTxt(unames[i], 10, can ? C::TEXT : C::TEXT_DIM, px + 14.f, iy + 3.f);
        iy += 22.f;
    }
    iy += 4.f;

    drawTxt("ОТДЕЛЕНИЯ:", 9, C::TEXT_DIM, px + 10.f, iy); iy += 18.f;
    struct SR { const char* name; const char* desc; };
    SR rows[] =
    { {"Пех. отделение","3+ пехотинца → мощь × 2"},
      {"Танк. отделение","Танк + 3 пехотинца → +50"},
      {"Арт. отделение","Арт + 2 пехотинца → +30"} };
    for (auto& r : rows) {
        rect(px + 6.f, iy, dw - 12.f, 44.f, C::BG_CARD, C::GREEN_DIM);
        rect(px + 6.f, iy, 3.f, 44.f, C::GREEN_DIM, C::NONE);
        drawTxt(r.name, 12, C::GREEN, px + 16.f, iy + 5.f);
        drawTxt(r.desc, 9, C::TEXT_DIM, px + 16.f, iy + 26.f);
        iy += 52.f;
    }
    iy += 4.f;

    rect(px + 6.f, iy, dw - 12.f, 48.f, C::BG_CARD, C::GOLD);
    rect(px + 6.f, iy, 3.f, 48.f, C::GOLD, C::NONE);
    drawTxt("Взвод", 12, C::GOLD, px + 16.f, iy + 5.f);
    drawTxt("3+ отд. с бонусом → мощь × 1.5", 9, C::TEXT_DIM, px + 16.f, iy + 26.f);
}

// ── Результат ───────────────────────────────────────────────
void Renderer::drawResultOverlay() {
    if (!state.currentMission) return;
    sf::RectangleShape bg(sf::Vector2f(W, H)); bg.setFillColor(sf::Color(0, 0, 0, 195)); win.draw(bg);
    float bw = 760.f, bh = 106.f + (float)state.currentMission->directions.size() * 72.f + 140.f;
    float bx = (W - bw) * 0.5f, by = (H - bh) * 0.5f; if (by < 4.f) by = 4.f;
    rect(bx, by, bw, bh, C::BG_PANEL, C::GREEN_DIM); rect(bx, by, bw, 3.f, C::GREEN_DIM, C::NONE);
    float px = bx + 30.f, py = by + 22.f;
    int stars = state.lastStars; sf::Color vc = (stars == 3) ? C::GREEN : (stars >= 1) ? C::AMBER : C::RED;
    std::string verdict = (stars == 3) ? "БЛЕСТЯЩЕ! ВСЕ РУБЕЖИ ЗАКРЫТЫ." :
        (stars == 2) ? "ВЫПОЛНЕНО. ЕСТЬ СЛАБЫЕ МЕСТА." :
        (stars == 1) ? "С ПОТЕРЯМИ. СЕРЬЁЗНЫЕ ОШИБКИ." : "ПРОВАЛ. ПОЗИЦИЯ ПОТЕРЯНА.";
    drawTxt("РЕЗУЛЬТАТ БОЯ", 12, C::TEXT_DIM, px, py); py += 22.f;
    drawStars(stars, px, py, 32.f); py += 46.f;
    drawTxt(verdict, 18, vc, px, py); py += 36.f;
    for (const auto& d : state.currentMission->directions) {
        DirectionResult dr = d.getResult();
        sf::Color dc = (dr == DirectionResult::SUPPRESSED) ? C::GREEN :
            (dr == DirectionResult::HELD) ? C::GREEN_DIM :
            (dr == DirectionResult::BREAKTHROUGH_LOSSES) ? C::AMBER : C::RED;
        rect(px, py, bw - 60.f, 60.f, C::BG_CARD, dc); rect(px, py, 4.f, 60.f, dc, C::NONE);
        drawTxt(d.label + "  —  " + d.getResultText(), 13, dc, px + 12.f, py + 6.f);
        std::ostringstream ss;
        ss << "Наша: " << std::fixed << std::setprecision(0) << d.getFriendlyPower()
            << "  Угроза: " << d.enemy_power << "  Счёт: " << std::fixed << std::setprecision(0) << d.getScore();
        drawTxt(ss.str(), 11, C::TEXT, px + 12.f, py + 28.f); py += 68.f;
    }
    py += 8.f;
    float hw = (bw - 60.f) * 0.5f - 8.f;
    rect(px, py, hw, 44.f, C::BG_CARD, C::BORDER); drawTxt("ПЕРЕИГРАТЬ", 13, C::TEXT, px + 28.f, py + 13.f);
    rect(px + hw + 14.f, py, hw, 44.f, sf::Color(10, 40, 10), C::GREEN, 2.f);
    drawTxt("СЛЕДУЮЩАЯ МИССИЯ  ->", 13, C::GREEN, px + hw + 24.f, py + 13.f);
}

// ── Обработка ввода ─────────────────────────────────────────
void Renderer::handleMouseScroll(float delta, sf::Vector2f pos) {
    float col2x = COL1_W, scrollH = PAN_H - 30.f;
    if (pos.x >= col2x && pos.x <= col2x + COL2_W && pos.y >= PAN_Y && pos.y <= H) {
        float maxScroll = std::max(0.f, col2TotalH - scrollH);
        col2ScrollY = std::clamp(col2ScrollY - delta * 30.f, 0.f, maxScroll);
    }
}

void Renderer::handleMouseMove(sf::Vector2f pos) {
    hoveredNode = -1; legendHover = -1;
    if (popup.visible || addDrop.visible) return;
    for (int i = 0; i < (int)nodes.size(); i++) if (nodeHit(i, pos)) { hoveredNode = i; break; }
    float col2x = COL1_W, scrollTop = PAN_Y + 28.f, scrollH = PAN_H - 30.f;
    if (pos.x >= col2x && pos.x <= col2x + COL2_W && pos.y >= scrollTop && pos.y <= scrollTop + scrollH) {
        float cy = scrollTop - col2ScrollY + 16.f + 3 * 42.f + 6.f + 16.f + 16.f;
        for (int i = 0; i < 3; i++) {
            if (pos.y >= cy && pos.y <= cy + 50.f && pos.y >= scrollTop && pos.y <= scrollTop + scrollH) {
                legendHover = i; legendHoverPos = { pos.x, cy }; break;
            }
            cy += 50.f;
        }
    }
}

void Renderer::handleMouseClick(sf::Vector2f pos) {
    if (!state.currentMission) return;

    // Кнопка В МЕНЮ
    if (pos.x >= W - 100.f && pos.y >= 10.f && pos.y <= 40.f) {
        wantsMenu = true; popup.visible = false; addDrop.visible = false; return;
    }

    // ── Дропдаун ──
    if (addDrop.visible) {
        float dw = 360.f, px = std::min(addDrop.x, W - dw - 6.f), py = addDrop.y;
        if (py + 400.f > H) py = H - 400.f;

        if (addDrop.mode == 1 && addDrop.targetFormation) {
            float iy = py + 28.f;
            if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f) {
                // Пехотинец
                if (pos.y >= iy && pos.y <= iy + 32.f) {
                    if (state.remaining() >= 5) {
                        Unit* nu = state.createUnit(UnitFactory::makeRifleman());
                        state.addUnitToFormation(addDrop.targetFormation, nu);
                    }
                    addDrop.visible = false; addDrop.targetFormation = nullptr; return;
                }
                iy += 36.f;
                // Танк
                if (pos.y >= iy && pos.y <= iy + 32.f) {
                    if (state.remaining() >= 25) {
                        Unit* nu = state.createUnit(UnitFactory::makeTank());
                        state.addUnitToFormation(addDrop.targetFormation, nu);
                    }
                    addDrop.visible = false; addDrop.targetFormation = nullptr; return;
                }
                iy += 36.f;
                // Артиллерия
                if (pos.y >= iy && pos.y <= iy + 32.f) {
                    if (state.remaining() >= 15) {
                        Unit* nu = state.createUnit(UnitFactory::makeArtillery());
                        state.addUnitToFormation(addDrop.targetFormation, nu);
                    }
                    addDrop.visible = false; addDrop.targetFormation = nullptr; return;
                }
            }
            addDrop.visible = false; addDrop.targetFormation = nullptr; return;
        }

        // mode==0: добавить на рубеж
        float iy = py + 28.f + 18.f;
        int ucosts[] = { 5,25,15 };
        // Пехотинец
        if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f && pos.y >= iy && pos.y <= iy + 20.f) {
            if (state.remaining() >= ucosts[0]) {
                Unit* u = state.createUnit(UnitFactory::makeRifleman());
                state.spent += u->cost;
                state.assignToDirection(u, addDrop.dirIdx);
            }
            addDrop.visible = false; return;
        }
        iy += 22.f;
        // Танк
        if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f && pos.y >= iy && pos.y <= iy + 20.f) {
            if (state.remaining() >= ucosts[1]) {
                Unit* u = state.createUnit(UnitFactory::makeTank());
                state.spent += u->cost;
                state.assignToDirection(u, addDrop.dirIdx);
            }
            addDrop.visible = false; return;
        }
        iy += 22.f;
        // Артиллерия
        if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f && pos.y >= iy && pos.y <= iy + 20.f) {
            if (state.remaining() >= ucosts[2]) {
                Unit* u = state.createUnit(UnitFactory::makeArtillery());
                state.spent += u->cost;
                state.assignToDirection(u, addDrop.dirIdx);
            }
            addDrop.visible = false; return;
        }
        iy += 22.f + 4.f;
        // Отделения (3 блока)
        for (int i = 0; i < 3; i++) {
            if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f && pos.y >= iy && pos.y <= iy + 44.f) {
                Formation* f = nullptr;
                if (i == 0) f = state.createFormation(Formation::makeInfantrySquad());
                else if (i == 1) f = state.createFormation(Formation::makeTankSquad());
                else if (i == 2) f = state.createFormation(Formation::makeArtillerySquad());
                if (f) state.assignToDirection(f, addDrop.dirIdx);
                addDrop.visible = false; return;
            }
            iy += 52.f;
        }
        iy += 4.f;
        // Взвод (зона внизу)
        if (pos.x >= px + 6.f && pos.x <= px + dw - 6.f && pos.y >= iy && pos.y <= iy + 48.f) {
#ifdef HAS_MAKE_PLATOON
            Formation* pl = state.createFormation(Formation::makePlatoon());
            if (pl) state.assignToDirection(pl, addDrop.dirIdx);
#endif
            addDrop.visible = false; return;
        }

        addDrop.visible = false; return;
    }

    // Если кликнули по узлу на карте — открыть попап
    for (int i = 0; i < (int)nodes.size(); i++) {
        if (nodeHit(i, pos)) {
            popup.openFor(nodes[i].dirIdx, pos.x, pos.y, (int)state.currentMission->directions[nodes[i].dirIdx].units.size());
            return;
        }
    }
}

// ── Вспомогательные отрисовки и утилиты ─────────────────────

void Renderer::rect(float x, float y, float w, float h,
    sf::Color fill, sf::Color border, float thick) {
    sf::RectangleShape r(sf::Vector2f(w, h));
    r.setPosition(sf::Vector2f(x, y));
    r.setFillColor(fill);
    if (border != C::NONE && thick > 0.f) {
        r.setOutlineColor(border);
        r.setOutlineThickness(thick);
    }
    else {
        r.setOutlineThickness(0.f);
    }
    win.draw(r);
}

void Renderer::circle(float cx, float cy, float r,
    sf::Color fill, sf::Color border, float thick) {
    sf::CircleShape c(r);
    c.setPosition(sf::Vector2f(cx - r, cy - r));
    c.setFillColor(fill);
    if (border != C::NONE && thick > 0.f) {
        c.setOutlineColor(border);
        c.setOutlineThickness(thick);
    }
    else c.setOutlineThickness(0.f);
    win.draw(c);
}

void Renderer::line(sf::Vector2f a, sf::Vector2f b, sf::Color col, float /*thick*/) {
    // SFML 3: используем sf::PrimitiveType::Lines
    sf::VertexArray va(sf::PrimitiveType::Lines, 2);
    va[0].position = a;
    va[0].color = col;
    va[1].position = b;
    va[1].color = col;
    win.draw(va);
}

void Renderer::dashedLine(sf::Vector2f a, sf::Vector2f b, sf::Color col, float dash, float gap) {
    sf::Vector2f diff = b - a;
    float len = std::hypot(diff.x, diff.y);
    if (len < 1e-3f) return;
    sf::Vector2f dir = diff / len;
    float pos = 0.f;
    while (pos < len) {
        float seg = std::min(dash, len - pos);
        sf::Vector2f s = a + dir * pos;
        sf::Vector2f e = a + dir * (pos + seg);
        line(s, e, col, 1.f);
        pos += dash + gap;
    }
}

void Renderer::drawTxt(const std::string& s, unsigned sz, sf::Color col, float x, float y) {
    // Надёжный способ для SFML 3.x: создать объект и задать свойства через сеттеры
    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(sz);
    t.setString(toSfString(s));
    t.setFillColor(col);
    t.setPosition(sf::Vector2f(x, y));
    win.draw(t);
}

void Renderer::arrowHead(sf::Vector2f tip, sf::Vector2f dir, sf::Color col, float sz) {
    float len = std::hypot(dir.x, dir.y);
    sf::Vector2f d = (len > 0.001f) ? dir / len : sf::Vector2f(0.f, 1.f);
    sf::Vector2f perp(-d.y, d.x);
    sf::ConvexShape tri;
    tri.setPointCount(3);
    tri.setPoint(0, tip);
    tri.setPoint(1, tip - d * sz + perp * (sz * 0.5f));
    tri.setPoint(2, tip - d * sz - perp * (sz * 0.5f));
    tri.setFillColor(col);
    win.draw(tri);
}

void Renderer::drawStars(int n, float x, float y, float sz) {
    for (int i = 0; i < 3; i++) {
        sf::CircleShape s(sz * 0.5f, 6);
        s.setPosition(sf::Vector2f(x + i * (sz + 8.f), y));
        s.setFillColor(i < n ? C::STAR_ON : C::STAR_OFF);
        win.draw(s);
    }
}

sf::Color Renderer::nodeColor(int idx) const {
    switch (idx % 4) {
    case 0: return C::GREEN;
    case 1: return C::AMBER;
    case 2: return C::RED;
    default: return C::GREEN_DIM;
    }
}

bool Renderer::nodeHit(int idx, sf::Vector2f p) const {
    if (idx < 0 || idx >= (int)nodes.size()) return false;
    const MapNode& n = nodes[idx];
    float dx = p.x - n.pos.x;
    float dy = p.y - n.pos.y;
    return dx * dx + dy * dy <= n.r * n.r;
}

sf::Color Renderer::roleColor(const std::string& role) const {
    if (role == "INFANTRY") return C::GREEN;
    if (role == "VEHICLE") return C::AMBER;
    return C::TEXT;
}

// Заглушка для рекурсивной отрисовки — при необходимости можно расширить
float Renderer::drawUnitTree(IMilitaryUnit* unit, float px, float cy,
    float pw, int topIdx, int depth,
    std::vector<bool>& exp,
    std::vector<std::vector<bool>>& childExp) {
    if (!unit) return cy;
    rect(px, cy, pw, 28.f, C::BG_CARD, C::BORDER);
    drawTxt(unit->getName(), 11, C::TEXT, px + 6.f, cy + 4.f);
    return cy + 28.f;
}
