#include "Renderer.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

static sf::String U(const std::string& s){return sf::String::fromUtf8(s.begin(),s.end());}

Renderer::Renderer(sf::RenderWindow& w,sf::Font& f,GameState&  gs):win(w),font(f),state(gs){}

void Renderer::buildMap(Mission* mission){
    nodes.clear(); hoveredNode=-1; popup.visible=false;
    addDrop.visible=false; addDrop.targetSquad=nullptr;
    wantsNextMission=false; wantsMenu=false;
    col2ScrollY=0.f; legendHover=-1;
    int n=(int)mission->directions.size(); float cy=TOP_H+MAP_H*0.52f;
    if(n==3){
        float topY=TOP_H+MAP_H*0.24f, step=W/4.f;
        nodes.push_back({{step*1.f,topY},44.f,0});
        nodes.push_back({{step*2.f,topY},44.f,1});
        nodes.push_back({{step*3.f,topY},44.f,2});
    }else if(n==2){
        nodes.push_back({{W*0.35f,cy},44.f,0});
        nodes.push_back({{W*0.65f,cy},44.f,1});
    }else{
        float step=W/(float)(n+1);
        for(int i=0;i<n;i++) nodes.push_back({{step*(i+1),cy},44.f,i});
    }
}

void Renderer::draw(){
    win.clear(C::BG);
    drawGrid(); drawTerrain(); drawEnemyArrows(); drawConnectors(); drawNodes();
    if(hoveredNode>=0&&!popup.visible) drawHoverTooltip(hoveredNode);
    drawTopBar(); drawBottomPanel();
    if(legendHover>=0) drawLegendTooltip();
    if(popup.visible)   drawPopup();
    if(addDrop.visible) drawAddDrop();
    if(state.phase==GamePhase::RESULT) drawResultOverlay();
}

// ── Шапка ──────────────────────────────────────────────────
void Renderer::drawTopBar(){
    rect(0,0,W,TOP_H,C::BG_PANEL,C::BORDER);
    rect(0,TOP_H-2.f,W,2.f,C::GREEN_DIM,C::NONE);
    drawTxt("КОМАНДИР",20,C::GREEN,28.f,12.f);
    drawTxt("ТАКТИЧЕСКАЯ КОМАНДНАЯ СИСТЕМА",10,C::TEXT_DIM,200.f,17.f);
    if(state.currentMission) drawTxt(state.currentMission->title,16,C::AMBER,W*0.5f-120.f,14.f);
    if(state.currentMission){
        std::string bud="БЮДЖЕТ: "+std::to_string(state.remaining())+" / "+std::to_string(state.budget)+" ОЧК";
        drawTxt(bud,14,C::AMBER,W-370.f,15.f);
    }
    // Кнопка В МЕНЮ
    rect(W-100.f, 10.f, 88.f, 32.f, sf::Color(12,22,12), C::BORDER);
    drawTxt("В МЕНЮ", 12, C::TEXT_DIM, W-88.f, 18.f);
}

// ── Сетка ──────────────────────────────────────────────────
void Renderer::drawGrid(){
    for(float x=0;x<W;x+=80.f) line({x,TOP_H},{x,TOP_H+MAP_H},C::GRID);
    for(float y=TOP_H;y<TOP_H+MAP_H;y+=80.f) line({0,y},{W,y},C::GRID);
}

// ── Рельеф ─────────────────────────────────────────────────
void Renderer::drawTerrain(){
    float my=TOP_H; bool m1=(state.currentMissionIndex==0);
    if(m1){
        struct T{float x,y;const char* s;sf::Color c;unsigned sz;};
        T cells[]={
            {55, my+250,"^ ^ ^ ^",  C::FOREST,18},{38, my+278,"^ ^ ^ ^ ^",C::FOREST,18},
            {60, my+306,"^ ^ ^ ^",  C::FOREST,18},{35, my+334,"^ ^ ^ ^ ^",C::FOREST,18},
            {55, my+362,"^ ^ ^ ^",  C::FOREST,18},{40, my+390,"^ ^ ^ ^ ^",C::FOREST,18},
            {1680,my+260,"^ ^ ^ ^", C::FOREST,18},{1700,my+288,"^ ^ ^ ^", C::FOREST,18},
            {1685,my+316,"^ ^ ^ ^ ^",C::FOREST,18},{1695,my+344,"^ ^ ^ ^",C::FOREST,18},
            {800, my+270,"/^^\\307", C::HILL,20},  {778, my+298,"/^^^^^^\\",C::HILL,20},
            {772, my+326,"\\________/",C::HILL,18},
            {1520,my+90, "~ ~ ~ ~ ~",C::RIVER,18}, {1535,my+118,"~ ~ ~ ~ ~",C::RIVER,18},
            {1520,my+146,"~ ~ ~ ~ ~",C::RIVER,18}, {1530,my+174,"~ ~ ~ ~ ~",C::RIVER,18},
            {1518,my+202,"~ ~ ~ ~ ~",C::RIVER,18}, {1525,my+230,"~ ~ ~ ~ ~",C::RIVER,18},
            {1515,my+258,"~ ~ ~ ~ ~",C::RIVER,18}, {1522,my+286,"~ ~ ~ ~ ~",C::RIVER,18},
            {1170,my+360,". . . . .",C::FOREST,16},{1190,my+384,". . . . .",C::FOREST,16},
        };
        for(auto& c:cells) drawTxt(c.s,c.sz,c.c,c.x,c.y);
        drawTxt("- - - - ЛИНИЯ ОБОРОНЫ - - - -",13,C::GREEN_DIM,W*0.5f-160.f,my+MAP_H-22.f);
    }else{
        for(float ry=my+40;ry<my+MAP_H-28;ry+=26.f){
            float ox=6.f*std::sin(ry*0.05f);
            drawTxt("~ ~ ~ ~ ~ ~ ~",18,C::RIVER,W*0.5f-70.f+ox,ry);
        }
        drawTxt("=======",  18,C::AMBER,    W*0.5f-42.f,my+MAP_H*0.5f-12.f);
        drawTxt("М О С Т",  13,C::AMBER_DIM,W*0.5f-30.f,my+MAP_H*0.5f+14.f);
        drawTxt("НАШ БЕРЕГ",14,C::GREEN_DIM,W*0.26f,    my+MAP_H*0.5f);
        drawTxt("ПР-К",     14,C::RED_DIM,  W*0.64f,    my+MAP_H*0.5f);
    }
}

void Renderer::drawConnectors(){
    int n=(int)nodes.size();
    for(int i=0;i<n-1;i++) line(nodes[i].pos,nodes[i+1].pos,C::GREEN_DIM,2.f);
    if(n==3) line(nodes[0].pos,nodes[2].pos,sf::Color(28,75,28),1.5f);
}

void Renderer::drawEnemyArrows(){
    if(!state.currentMission)return;
    for(int i=0;i<(int)nodes.size();i++){
        sf::Vector2f tip=nodes[i].pos; sf::Color col=nodeColor(i);
        const auto& d=state.currentMission->directions[i];
        sf::Vector2f from={tip.x,TOP_H+6.f};
        dashedLine(from,{tip.x,tip.y-nodes[i].r-4.f},col,16.f,8.f);
        arrowHead({tip.x,tip.y-nodes[i].r-4.f},{0.f,1.f},col,16.f);
        drawTxt(d.getEnemyTypeText()+" x"+std::to_string(d.enemy_power),13,col,tip.x-50.f,TOP_H+10.f);
    }
}

void Renderer::drawNodes(){
    if(!state.currentMission)return;
    for(int i=0;i<(int)nodes.size();i++){
        const auto& nd=nodes[i]; const auto& d=state.currentMission->directions[nd.dirIdx];
        sf::Color col=nodeColor(i); bool hov=(hoveredNode==i),sel=(popup.visible&&popup.dirIdx==i);
        if(sel) circle(nd.pos.x,nd.pos.y,nd.r+16.f,C::NONE,sf::Color(col.r,col.g,col.b,65),2.5f);
        else if(hov) circle(nd.pos.x,nd.pos.y,nd.r+10.f,C::NONE,sf::Color(col.r,col.g,col.b,40),2.f);
        sf::Color fill=sel?sf::Color(22,58,22):hov?sf::Color(14,40,14):sf::Color(10,24,10);
        circle(nd.pos.x,nd.pos.y,nd.r,fill,col,sel?3.5f:2.f);
        const std::string& lbl=d.label;
        std::string ltr=(lbl.find("ЗАП")!=std::string::npos)?"З":
                        (lbl.find("ЦЕНТР")!=std::string::npos)?"Ц":
                        (lbl.find("ВОСТ")!=std::string::npos)?"В":
                        (lbl.find("ПРАВ")!=std::string::npos)?"П":std::to_string(i+1);
        drawTxt(ltr,24,col,nd.pos.x-10.f,nd.pos.y-15.f);
        drawTxt(d.label,12,C::TEXT_DIM,nd.pos.x-(float)d.label.size()*3.6f,nd.pos.y+nd.r+8.f);    
        int total = (int)d.squads.size() + (int)d.platoons.size() + (int)d.units.size();
        if (total > 0) drawTxt("[" + std::to_string(total) + " ед.]", 11, C::GREEN, nd.pos.x - 22.f, nd.pos.y + nd.r + 24.f);
    }
}

void Renderer::drawHoverTooltip(int idx){
    if(!state.currentMission||idx<0||idx>=(int)nodes.size())return;
    const auto& nd=nodes[idx]; const auto& d=state.currentMission->directions[nd.dirIdx];
    float tw=265.f,th=100.f;
    float tx=nd.pos.x+nd.r+16.f,ty=nd.pos.y-th*0.5f;
    if(tx+tw>W-8.f) tx=nd.pos.x-nd.r-tw-16.f;
    if(ty<TOP_H+4.f) ty=TOP_H+4.f;
    rect(tx,ty,tw,th,C::BG_CARD,nodeColor(idx));
    float py=ty+10.f;
    auto ln=[&](const std::string& s,unsigned sz,sf::Color c){drawTxt(s,sz,c,tx+12.f,py);py+=sz*1.75f;};
    ln(d.label,13,nodeColor(idx));
    ln(d.getEnemyTypeText()+" x"+std::to_string(d.enemy_power),11,C::AMBER);
    std::ostringstream fp; fp<<std::fixed<<std::setprecision(0)<<d.getFriendlyPower();
    ln("Наша мощь: "+fp.str(),11,C::GREEN);
    ln("[нажми чтобы открыть]",10,C::TEXT_DIM);
}

// ── Нижняя панель ──────────────────────────────────────────
void Renderer::drawBottomPanel(){
    rect(0,PAN_Y,W,PAN_H,C::BG_PANEL,C::NONE);
    rect(0,PAN_Y,W,2.f,C::GREEN_DIM,C::NONE);
    float col2x=COL1_W,col3x=COL1_W+COL2_W,col3w=W-col3x;
    rect(col2x,PAN_Y,1.f,PAN_H,C::BORDER,C::NONE);
    rect(col3x,PAN_Y,1.f,PAN_H,C::BORDER,C::NONE);
    drawCol1(0,PAN_Y,COL1_W,PAN_H);
    drawCol2(col2x,PAN_Y,COL2_W,PAN_H);
    drawCol3(col3x,PAN_Y,col3w,PAN_H);
}

// ── Колонка 1: Приказ ──────────────────────────────────────
void Renderer::drawCol1(float x,float y,float w,float h){
    if(!state.currentMission)return;
    float px=x+18.f,py=y+12.f;
    drawTxt("БОЕВОЙ ПРИКАЗ",10,C::TEXT_DIM,px,py); py+=20.f;
    drawTxt(state.currentMission->title,17,C::AMBER,px,py); py+=30.f;
    std::istringstream iss(state.currentMission->briefing); std::string ln2;
    while(std::getline(iss,ln2)){drawTxt(ln2,12,C::TEXT,px,py);py+=19.f;}
    py+=8.f;
    drawTxt("УГРОЗЫ ПО РУБЕЖАМ",10,C::TEXT_DIM,px,py); py+=16.f;
    for(int i=0;i<(int)state.currentMission->directions.size();i++){
        const auto& d=state.currentMission->directions[i]; sf::Color col=nodeColor(i);
        rect(px,py,w-36.f,46.f,C::BG_CARD,col); rect(px,py,4.f,46.f,col,C::NONE);
        drawTxt(d.label,12,col,px+10.f,py+5.f);
        drawTxt(d.getEnemyTypeText()+"  |  мощь: "+std::to_string(d.enemy_power),11,C::TEXT,px+10.f,py+24.f);
        py+=54.f;
    }
}

// ── Колонка 2: Легенда со скроллом ─────────────────────────
void Renderer::drawCol2(float x,float y,float w,float h){
    float px=x+16.f;
    float scrollTop=y+28.f;
    float scrollH=h-30.f;
    float maxScroll=std::max(0.f,col2TotalH-scrollH);
    col2ScrollY=std::clamp(col2ScrollY,0.f,maxScroll);

    // Заголовок (не скроллируется)
    drawTxt("ЮНИТЫ И ПОДРАЗДЕЛЕНИЯ",10,C::TEXT_DIM,px,y+10.f);

    // Полоса прокрутки справа
    if(col2TotalH>scrollH){
        float barH=scrollH*(scrollH/col2TotalH);
        float barY=scrollTop+(col2ScrollY/maxScroll)*(scrollH-barH);
        rect(x+w-8.f,scrollTop,6.f,scrollH,sf::Color(8,18,8),C::BORDER);
        rect(x+w-7.f,barY,4.f,barH,C::GREEN_DIM,C::NONE);
    }

    // Clip: рисуем только то что в [scrollTop, scrollTop+scrollH]
    // используем простую проверку видимости по Y
    float cy=scrollTop-col2ScrollY;

    auto skip=[&](float itemH){cy+=itemH;};
    auto vis=[&](float cy2,float itemH)->bool{return cy2+itemH>scrollTop&&cy2<scrollTop+scrollH;};
    auto drawItem=[&](float itemH,auto fn){
        if(vis(cy,itemH)) fn(cy);
        cy+=itemH;
    };

    // ── Юниты ─────────────────────────────────────────
    drawItem(16.f,[&](float iy){drawTxt("── ЮНИТЫ ─────────────────────────────",10,C::GREEN_DIM,px,iy);});

    struct UI{const char* name;int cost;int pow;const char* note;};
    UI units[]={
        {"Пехотинец",  5, 5,"мощь: 5"},
        {"Танк",      25, 25,"мощь: 25 (бонус в отд.)"},
        {"Артиллерия",15, 15,"мощь: 15 (бонус в отд.)"},
    };
    for(auto& u:units){
        drawItem(42.f,[&](float iy){
            rect(px,iy,w-34.f,40.f,C::BG_CARD,C::BORDER);
            drawTxt(u.name,12,C::TEXT,px+8.f,iy+5.f);
            drawTxt(std::to_string(u.cost)+" ОЧК",10,C::AMBER,px+w-80.f,iy+7.f);
            drawTxt(u.note,9,C::TEXT_DIM,px+8.f,iy+24.f);
        });
    }

    cy+=6.f;

    // ── Отделения ───────────────────────────────────
    drawItem(16.f,[&](float iy){drawTxt("── ОТДЕЛЕНИЯ (наведи = требования) ───",10,C::GREEN_DIM,px,iy);});

    struct SI{const char* name;const char* slots;int li;};
    SI sqs[]={
        {"Пех. отделение", "до 8 юнитов | бонус: 3+ пехотинца = мощь x2",0},
        {"Танк. отделение","до 5 юнитов | бонус: танк+3 пехотинца = +50", 1},
        {"Арт. отделение", "до 5 юнитов | бонус: арт+2 пехотинца = +30",  2},
    };
    for(auto& s:sqs){
        drawItem(50.f,[&](float iy){
            bool hov=(legendHover==s.li);
            rect(px,iy,w-34.f,48.f,hov?C::BG_SEL:C::BG_CARD,hov?C::GREEN:C::GREEN_DIM);
            rect(px,iy,4.f,48.f,hov?C::GREEN:C::GREEN_DIM,C::NONE);
            drawTxt(s.name,12,hov?C::GREEN:C::TEXT,px+10.f,iy+6.f);
            drawTxt(s.slots,9,C::TEXT_DIM,px+10.f,iy+26.f);
            drawTxt("[?]",9,C::GREEN_DIM,px+w-46.f,iy+6.f);
        });
    }

    cy+=8.f;

    // ── Взвод ─────────────────────────────────────
    drawItem(16.f,[&](float iy){drawTxt("── ВЗВОД ──────────────────────────────",10,C::GREEN_DIM,px,iy);});
    drawItem(58.f,[&](float iy){
        rect(px,iy,w-34.f,56.f,C::BG_CARD,C::GOLD);
        rect(px,iy,4.f,56.f,C::GOLD,C::NONE);
        drawTxt("Взвод",13,C::GOLD,px+10.f,iy+6.f);
        drawTxt("Контейнер для 3 отделений",9,C::TEXT_DIM,px+10.f,iy+24.f);
        drawTxt("Бонус: все 3 отд. с абилкой = мощь x1.5",9,C::TEXT_DIM,px+10.f,iy+38.f);
    });

    cy+=4.f;
    drawItem(14.f,[&](float iy){drawTxt("* техника без бойцов не даёт бонус",9,C::TEXT_DIM,px,iy);});

    col2TotalH=cy-scrollTop+col2ScrollY+10.f;
}

// ── Тултип легенды ─────────────────────────────────────────
void Renderer::drawLegendTooltip(){
    struct Info{const char* title;const char* req;const char* power;const char* spec;};
    Info infos[]={
        {"ПЕХ. ОТДЕЛЕНИЕ",
         "3+ пехотинцев (INFANTRY)",
         "raw = сумма мощей. Бонус: raw x2",
         "vs ПЕХОТА x1.0 | vs БРОНЯ x0.4"},
        {"ТАНК. ОТДЕЛЕНИЕ",
         "Танк (VEHICLE) + 3+ пехотинца",
         "raw = мощь пехоты. Бонус: +50",
         "vs ПЕХОТА x0.6 | vs БРОНЯ x2.0"},
        {"АРТ. ОТДЕЛЕНИЕ",
         "Артиллерия (VEHICLE) + 2+ пехотинца",
         "raw = мощь пехоты. Бонус: +30",
         "vs ПЕХОТА x1.4 | vs БРОНЯ x0.7"},
    };
    if(legendHover<0||legendHover>2)return;
    const Info& inf=infos[legendHover];
    float tw=330.f,th=150.f;
    float tx=legendHoverPos.x+14.f,ty=legendHoverPos.y-10.f;
    if(tx+tw>W-8.f) tx=legendHoverPos.x-tw-14.f;
    if(ty<TOP_H)   ty=TOP_H+4.f;
    if(ty+th>H-4.f) ty=H-th-4.f;
    rect(tx,ty,tw,th,C::BG_POPUP,C::GREEN,1.5f);
    rect(tx,ty,tw,3.f,C::GREEN,C::NONE);
    float py=ty+10.f;
    auto ln=[&](const std::string& s,unsigned sz,sf::Color c){drawTxt(s,sz,c,tx+10.f,py);py+=sz*1.7f;};
    ln(inf.title,13,C::GREEN);
    py+=2.f;
    ln("УСЛОВИЕ БОНУСА:",10,C::TEXT_DIM);
    ln(inf.req,11,C::AMBER);
    py+=2.f;
    ln("ФОРМУЛА:",10,C::TEXT_DIM);
    ln(inf.power,11,C::GREEN);
    py+=2.f;
    ln("СПЕЦ. КОЭФФИЦИЕНТ:",10,C::TEXT_DIM);
    ln(inf.spec,11,C::TEXT);
}

// ── Колонка 3: Управление ──────────────────────────────────
void Renderer::drawCol3(float x,float y,float w,float h){
    float px=x+18.f,py=y+12.f;
    drawTxt("УПРАВЛЕНИЕ",10,C::TEXT_DIM,px,py); py+=20.f;
    drawTxt("Нажми на рубеж — откроется окно.",12,C::TEXT,px,py); py+=18.f;
    drawTxt("Добавляй: юнит / отделение / взвод.",12,C::TEXT,px,py); py+=34.f;
    rect(px,py,w-36.f,48.f,sf::Color(12,45,12),C::GREEN,2.f);
    drawTxt("[ ВЫПОЛНИТЬ ПРИКАЗ ]",15,C::GREEN,px+22.f,py+13.f);
    if(state.currentMission){
        py+=60.f;
        drawTxt("СТАТУС РУБЕЖЕЙ:",10,C::TEXT_DIM,px,py); py+=16.f;
        for(int i=0;i<(int)state.currentMission->directions.size();i++){
            const auto& d=state.currentMission->directions[i];
            std::ostringstream ss;
            ss<<d.label<<": "<<std::fixed<<std::setprecision(0)
              <<d.getFriendlyPower()<<" / "<<d.enemy_power<<" ("<<d.getResultText()<<")";
            drawTxt(ss.str(),11,nodeColor(i),px,py); py+=19.f;
        }
    }
}

// ============================================================
//  Попап рубежа — дерево: [взводы...] [отделения...] [+ добавить]
// ============================================================
float Renderer::calcPopupH() const {

    if (!state.currentMission || popup.dirIdx < 0) return 200.f;
    const auto& d = state.currentMission->directions[popup.dirIdx];

    auto getExp = [](const std::vector<bool>& v, int i) {
        return (i >= 0 && i < (int)v.size()) ? v[i] : true;
        };

    float h = 52.f + 34.f + 14.f; // header + мощь-строка + "СОСТАВ:"

    // + Одиночные юниты
    h += (float)d.units.size() * 30.f;

    // Взводы
    for (int pi = 0;pi < (int)d.platoons.size();pi++) {
        h += 38.f;
        bool pexp = (pi < (int)popup.platoonExp.size()) ? popup.platoonExp[pi] : true;
        if (pexp) {
            const Platoon* pl = d.platoons[pi];
            for (int si = 0;si < (int)pl->squads.size();si++) {
                h += 32.f;
                bool sexp = (pi < (int)popup.platoonSquadExp.size() && si < (int)popup.platoonSquadExp[pi].size())
                    ? popup.platoonSquadExp[pi][si] : true;
                if (sexp) h += (float)pl->squads[si]->slots.size() * 24.f + 28.f;
                h += 4.f;
            }
            h += 28.f;
        }
        h += 4.f;
    }
    // Отдельные отделения
    for (int si = 0;si < (int)d.squads.size();si++) {
        h += 36.f;
        bool sexp = (si < (int)popup.squadExp.size()) ? popup.squadExp[si] : true;
        if (sexp) h += (float)d.squads[si]->slots.size() * 24.f + 28.f;
        h += 4.f;
    }
    h += 34.f + 38.f;
    return std::min(h, H - TOP_H - 20.f);
}

void Renderer::syncPopup(){
    if(!state.currentMission||popup.dirIdx<0)return;
    const auto& d=state.currentMission->directions[popup.dirIdx];
    while((int)popup.squadExp.size()<(int)d.squads.size()) popup.squadExp.push_back(true);
    while((int)popup.platoonExp.size()<(int)d.platoons.size()) popup.platoonExp.push_back(true);
    popup.platoonSquadExp.resize(d.platoons.size());
    for(int pi=0;pi<(int)d.platoons.size();pi++){
        while((int)popup.platoonSquadExp[pi].size()<(int)d.platoons[pi]->squads.size())
            popup.platoonSquadExp[pi].push_back(true);
    }
}

void Renderer::drawPopup(){
    if(!state.currentMission||popup.dirIdx<0)return;
    syncPopup();
    const auto& d=state.currentMission->directions[popup.dirIdx];
    sf::Color col=nodeColor(popup.dirIdx);
    float pw=POPUP_W,ph=calcPopupH();
    float px=std::min(popup.x,W-pw-6.f);
    float py=std::min(popup.y,TOP_H+MAP_H-ph-4.f);
    if(py<TOP_H+4.f) py=TOP_H+4.f; if(px<4.f) px=4.f;

    rect(px+4.f,py+4.f,pw,ph,sf::Color(0,0,0,80),C::NONE);
    rect(px,py,pw,ph,C::BG_POPUP,col,1.f);
    rect(px,py,pw,3.f,col,C::NONE);

    // Заголовок
    rect(px,py+3.f,pw,48.f,sf::Color(10,18,10),C::NONE);
    drawTxt(d.label,15,col,px+12.f,py+8.f);
    drawTxt("Угроза: "+d.getEnemyTypeText()+" x"+std::to_string(d.enemy_power),
            11,C::TEXT_DIM,px+12.f,py+28.f);
    rect(px+pw-38.f,py+8.f,30.f,22.f,C::BG_CARD,C::BORDER);
    drawTxt("[x]",10,C::TEXT_DIM,px+pw-35.f,py+12.f);

    float cy=py+52.f;

    // Строка мощи
    rect(px+6.f,cy,pw-12.f,28.f,sf::Color(8,16,8),C::BORDER);
    std::ostringstream fp; fp<<std::fixed<<std::setprecision(0)<<d.getFriendlyPower();
    drawTxt("Наша мощь: "+fp.str(),11,C::GREEN,px+12.f,cy+6.f);
    drawTxt("Угроза: "+std::to_string(d.enemy_power),11,C::RED,px+pw*0.46f,cy+6.f);
    float sc=d.getScore(); std::string stL; sf::Color stC;
    if(sc>=6.f){stL="ПОДАВЛЕНО";stC=C::GREEN;}
    else if(sc>=0.f){stL="УДЕРЖАНО";stC=C::GREEN_DIM;}
    else if(sc>=-5.f){stL="ПРОРЫВ!";stC=C::AMBER;}
    else{stL="ПРОРЫВ!";stC=C::RED;}
    drawTxt(stL,10,stC,px+pw*0.72f,cy+7.f);
    cy+=34.f;

    drawTxt("СОСТАВ:",10,C::TEXT_DIM,px+8.f,cy); cy+=14.f;
    
    // Юниты
    for (int ui = 0; ui < (int)d.units.size(); ui++) {
        const Unit* u = d.units[ui];
        rect(px + 4.f, cy, pw - 8.f, 26.f, sf::Color(10, 30, 10), C::GREEN_DIM);
        drawTxt(u->name, 11, C::TEXT, px + 12.f, cy + 5.f);
        drawTxt(std::to_string(u->cost) + " оч", 9, C::AMBER_DIM, px + pw - 85.f, cy + 7.f);

        // Кнопка удаления юнита
        rect(px + pw - 45.f, cy + 3.f, 38.f, 20.f, C::BG_CARD, C::RED_DIM);
        drawTxt("УДАЛ", 8, C::RED, px + pw - 42.f, cy + 7.f);
        cy += 30.f;
    }

    // ── Взводы ──
    for(int pi=0;pi<(int)d.platoons.size();pi++){
        Platoon* pl=d.platoons[pi];
        bool pexp=popup.platoonExp[pi];
        bool pbon=pl->hasBonus();
        sf::Color pc=pbon?C::GOLD:C::AMBER;

        rect(px+4.f,cy,pw-8.f,34.f,sf::Color(15,22,8),pc);
        drawTxt(pexp?"▼":"▶",10,C::AMBER_DIM,px+10.f,cy+8.f);
        drawTxt("ВЗВОД  ["+std::to_string(pl->squads.size())+"/3 отд.]",13,pc,px+26.f,cy+7.f);
        std::ostringstream pw2; pw2<<std::fixed<<std::setprecision(0)<<pl->calculatePower();
        drawTxt("мощь:"+pw2.str()+(pbon?" x1.5":""),10,C::AMBER,px+pw*0.52f,cy+9.f);
        // Кнопка удалить взвод
        rect(px+pw-50.f,cy+5.f,42.f,22.f,C::BG_CARD,C::RED_DIM);
        drawTxt("УБРАТЬ",9,C::RED,px+pw-48.f,cy+10.f);
        cy+=38.f;

        if(pexp){
            for(int si=0;si<(int)pl->squads.size();si++){
                Squad* sq=pl->squads[si];
                bool sexp=popup.platoonSquadExp[pi][si];
                bool sbon=sq->hasBonus();
                sf::Color sqc=sbon?C::GREEN:C::AMBER;

                rect(px+16.f,cy,pw-22.f,30.f,C::BG_CARD,sqc);
                drawTxt(sexp?"▼":"▶",9,C::GREEN_DIM,px+22.f,cy+8.f);
                drawTxt(sq->name,11,sqc,px+36.f,cy+7.f);
                std::ostringstream ps; ps<<std::fixed<<std::setprecision(0)<<sq->calculatePower();
                drawTxt("мощь:"+ps.str()+(sbon?" БОН":""),9,C::AMBER,px+pw*0.48f,cy+9.f);
                // Кнопка УБРАТЬ отделение из взвода
                rect(px+pw-50.f,cy+4.f,42.f,20.f,C::BG_CARD,C::RED_DIM);
                drawTxt("УБРАТЬ",8,C::RED,px+pw-48.f,cy+9.f);
                cy+=32.f;
                if(sexp){
                    for(int ui=0;ui<(int)sq->slots.size();ui++){
                        const Unit& u=sq->slots[ui];
                        rect(px+24.f,cy,pw-30.f,22.f,sf::Color(8,14,8),C::BORDER);
                        drawTxt("└",9,C::GREEN_DIM,px+28.f,cy+5.f);
                        drawTxt(u.name,10,C::TEXT,px+40.f,cy+5.f);
                        drawTxt(std::to_string(u.cost)+" оч",9,C::AMBER_DIM,px+pw*0.62f,cy+6.f);
                        rect(px+pw-38.f,cy+1.f,30.f,18.f,C::BG_CARD,C::RED_DIM);
                        drawTxt("x",9,C::RED,px+pw-27.f,cy+4.f);
                        cy+=24.f;
                    }
                    rect(px+24.f,cy,pw-30.f,24.f,C::BG_POPUP,C::GREEN_DIM);
                    drawTxt("+ добавить юнита",9,C::GREEN_DIM,px+34.f,cy+6.f);
                    cy+=28.f;
                }
                cy+=4.f;
            }
            // Добавить отделение во взвод
            rect(px+12.f,cy,pw-18.f,26.f,sf::Color(8,20,8),C::AMBER_DIM);
            drawTxt("+ добавить отделение во взвод",10,C::AMBER,px+22.f,cy+7.f);
            cy+=28.f;
        }
        cy+=4.f;
    }

    // ── Отдельные отделения ──
    for(int si=0;si<(int)d.squads.size();si++){
        Squad* sq=d.squads[si];
        bool sexp=popup.squadExp[si];
        bool sbon=sq->hasBonus();
        sf::Color sqc=sbon?C::GREEN:C::AMBER;

        rect(px+4.f,cy,pw-8.f,32.f,C::BG_CARD,sqc);
        drawTxt(sexp?"▼":"▶",10,C::GREEN_DIM,px+10.f,cy+8.f);
        drawTxt(sq->name,12,sqc,px+26.f,cy+7.f);
        std::ostringstream ps; ps<<std::fixed<<std::setprecision(0)<<sq->calculatePower();
        drawTxt("мощь:"+ps.str()+(sbon?" БОН":""),10,C::AMBER,px+pw*0.52f,cy+9.f);
        // Кнопка УБРАТЬ
        rect(px+pw-50.f,cy+5.f,42.f,22.f,C::BG_CARD,C::RED_DIM);
        drawTxt("УБРАТЬ",9,C::RED,px+pw-48.f,cy+10.f);
        cy+=36.f;

        if(sexp){
            for(int ui=0;ui<(int)sq->slots.size();ui++){
                const Unit& u=sq->slots[ui];
                rect(px+12.f,cy,pw-18.f,22.f,sf::Color(8,14,8),C::BORDER);
                drawTxt("└",9,C::GREEN_DIM,px+18.f,cy+5.f);
                drawTxt(u.name,10,C::TEXT,px+30.f,cy+5.f);
                drawTxt(std::to_string(u.cost)+" оч",9,C::AMBER_DIM,px+pw*0.60f,cy+6.f);
                rect(px+pw-38.f,cy+1.f,30.f,18.f,C::BG_CARD,C::RED_DIM);
                drawTxt("x",9,C::RED,px+pw-27.f,cy+4.f);
                cy+=24.f;
            }
            rect(px+12.f,cy,pw-18.f,24.f,C::BG_POPUP,C::GREEN_DIM);
            drawTxt("+ добавить юнита",10,C::GREEN_DIM,px+22.f,cy+6.f);
            cy+=28.f;
        }
        cy+=4.f;
    }

    // Кнопка "добавить на рубеж"
    rect(px+4.f,cy,pw-8.f,30.f,sf::Color(8,20,8),C::GREEN_DIM);
    drawTxt("+ добавить юнит / отделение / взвод",11,C::GREEN,px+16.f,cy+8.f);
    cy+=34.f;

    // Footer
    rect(px,cy,pw,1.f,C::BORDER,C::NONE);
    rect(px+4.f,cy+6.f,pw-8.f,24.f,C::BG_CARD,C::BORDER);
    drawTxt("[ ЗАКРЫТЬ ОКНО ]",11,C::TEXT_DIM,px+pw*0.5f-56.f,cy+10.f);
}

// ── Дропдаун ───────────────────────────────────────────────
void Renderer::drawAddDrop(){
    float dw=360.f;
    float px=std::min(addDrop.x,W-dw-6.f),py=addDrop.y;
    if(py+400.f>H) py=H-400.f;

    // Режим: добавить юнита во взводное отделение
    if(addDrop.squadIdx==-20){
        float dh=28.f+3*36.f+8.f;
        rect(px,py,dw,dh,C::BG_POPUP,C::GREEN_DIM);
        float iy=py+8.f;
        drawTxt("ДОБАВИТЬ ЮНИТА В ОТДЕЛЕНИЕ:",10,C::TEXT_DIM,px+10.f,iy); iy+=20.f;
        Unit cat[] = { UnitCatalog::Rifleman(), UnitCatalog::Tank(), UnitCatalog::Artillery() };
        for (auto& u : cat) {
            bool can = (state.remaining() >= u.cost);
            rect(px + 6.f, iy, dw - 12.f, 20.f, C::BG_CARD, can ? C::BORDER : C::RED_DIM);
            drawTxt(u.name, 11, can ? C::TEXT : C::TEXT_DIM, px + 14.f, iy + 3.f);
            drawTxt(std::to_string(u.cost) + " ОЧК", 9, C::AMBER, px + dw - 68.f, iy + 4.f);
            iy += 22.f;
        }
        return;
    }

    // Режим выбора типа отделения для взвода
    if(addDrop.squadIdx==-10){
        float dh=28.f+3*58.f+8.f;
        rect(px,py,dw,dh,C::BG_POPUP,C::AMBER);
        rect(px,py,dw,2.f,C::AMBER,C::NONE);
        float iy=py+8.f;
        drawTxt("ДОБАВИТЬ ОТДЕЛЕНИЕ ВО ВЗВОД:",10,C::AMBER,px+10.f,iy); iy+=20.f;
        struct SR{SquadType t;const char* name;const char* desc;};
        SR rows[]={
            {SquadType::INFANTRY_SQUAD,  "Пех. отделение",  "Бонус: 3+ пехотинца = мощь x2"},
            {SquadType::TANK_SQUAD,      "Танк. отделение", "Бонус: танк + 3 пехотинца = +50"},
            {SquadType::ARTILLERY_SQUAD, "Арт. отделение",  "Бонус: арт + 2 пехотинца = +30"},
        };
        for(auto& r:rows){
            rect(px+6.f,iy,dw-12.f,50.f,C::BG_CARD,C::GREEN_DIM);
            rect(px+6.f,iy,3.f,50.f,C::GREEN_DIM,C::NONE);
            drawTxt(r.name,13,C::GREEN,px+16.f,iy+6.f);
            drawTxt(r.desc,10,C::TEXT_DIM,px+16.f,iy+28.f);
            drawTxt("БЕСПЛАТНО",8,C::GREEN_DIM,px+dw-72.f,iy+8.f);
            iy+=58.f;
        }
        return;
    }

    if(addDrop.squadIdx==-1){
        // Добавить на рубеж: юнит, отделение, взвод
        float dh = 28.f + 18.f + 3 * 22.f + 4.f + 18.f + 3 * 58.f + 4.f + 62.f + 10.f;
        rect(px,py,dw,dh,C::BG_POPUP,C::GREEN_DIM);
        float iy=py+8.f;
        drawTxt("ДОБАВИТЬ НА РУБЕЖ:",10,C::TEXT_DIM,px+10.f,iy); iy+=20.f;

        // Юниты
// Юниты
        drawTxt("ЮНИТЫ:", 9, C::TEXT_DIM, px + 10.f, iy); iy += 18.f;
        Unit cat[] = { UnitCatalog::Rifleman(), UnitCatalog::Tank(), UnitCatalog::Artillery() };
        for (auto& u : cat) {
            bool can = (state.remaining() >= u.cost);
            rect(px + 6.f, iy, dw - 12.f, 20.f, C::BG_CARD, can ? C::BORDER : C::RED_DIM);
            drawTxt(u.name, 11, can ? C::TEXT : C::TEXT_DIM, px + 14.f, iy + 3.f);
            drawTxt(std::to_string(u.cost) + " ОЧК", 9, C::AMBER, px + dw - 68.f, iy + 4.f);
            iy += 22.f;
        }
        iy+=4.f;

        // Отделения
        drawTxt("ОТДЕЛЕНИЯ:",9,C::TEXT_DIM,px+10.f,iy); iy+=18.f;
        struct SR{SquadType t;const char* n;};
        SR rows[]={{SquadType::INFANTRY_SQUAD,"Пех. отделение"},
                   {SquadType::TANK_SQUAD,   "Танк. отделение"},
                   {SquadType::ARTILLERY_SQUAD,"Арт. отделение"}};
        for(auto& r:rows){
            rect(px+6.f,iy,dw-12.f,50.f,C::BG_CARD,C::GREEN_DIM);
            rect(px+6.f,iy,3.f,50.f,C::GREEN_DIM,C::NONE);
            drawTxt(r.n,11,C::GREEN,px+16.f,iy+6.f);
            drawTxt("Пустой контейнер — добавишь юнитов внутри",9,C::TEXT_DIM,px+16.f,iy+26.f);
            drawTxt("БЕСПЛАТНО",8,C::GREEN_DIM,px+dw-72.f,iy+8.f);
            iy+=58.f;
        }
        iy+=4.f;

        // Взвод
        rect(px+6.f,iy,dw-12.f,60.f,C::BG_CARD,C::GOLD);
        rect(px+6.f,iy,3.f,60.f,C::GOLD,C::NONE);
        drawTxt("Взвод",12,C::GOLD,px+16.f,iy+6.f);
        drawTxt("Контейнер для 3 отделений",9,C::TEXT_DIM,px+16.f,iy+24.f);
        drawTxt("Бонус при 3 активных отд. = мощь x1.5",9,C::TEXT_DIM,px+16.f,iy+38.f);
        drawTxt("БЕСПЛАТНО",8,C::GOLD,px+dw-72.f,iy+8.f);
        iy+=62.f;

    } else {
        // Добавить юнита в отделение
        float dh=28.f+3*36.f+8.f;
        rect(px,py,dw,dh,C::BG_POPUP,C::GREEN_DIM);
        float iy=py+8.f;
        drawTxt("ДОБАВИТЬ ЮНИТА:",10,C::TEXT_DIM,px+10.f,iy); iy+=20.f;
        Unit cat[]={UnitCatalog::Rifleman(),UnitCatalog::Tank(),UnitCatalog::Artillery()};
        for(auto& u:cat){
            bool can=(state.remaining()>=u.cost);
            rect(px+6.f,iy,dw-12.f,32.f,C::BG_CARD,can?C::BORDER:C::RED_DIM);
            drawTxt(u.name,12,can?C::TEXT:C::TEXT_DIM,px+14.f,iy+8.f);
            drawTxt(std::to_string(u.cost)+" ОЧК",10,C::AMBER,px+dw-68.f,iy+9.f);
            iy+=36.f;
        }
    }
}

// ── Результат ──────────────────────────────────────────────
void Renderer::drawResultOverlay(){
    if(!state.currentMission)return;
    sf::RectangleShape bg({W,H}); bg.setFillColor(sf::Color(0,0,0,195)); win.draw(bg);
    float bw=760.f,bh=106.f+(float)state.currentMission->directions.size()*74.f+140.f;
    float bx=(W-bw)*0.5f,by=(H-bh)*0.5f; if(by<4.f)by=4.f;
    rect(bx,by,bw,bh,C::BG_PANEL,C::GREEN_DIM);
    rect(bx,by,bw,3.f,C::GREEN_DIM,C::NONE);
    float px=bx+30.f,py=by+22.f;
    int stars=state.lastStars; sf::Color vc=(stars==3)?C::GREEN:(stars>=1)?C::AMBER:C::RED;
    std::string verdict=(stars==3)?"БЛЕСТЯЩЕ! ВСЕ РУБЕЖИ ЗАКРЫТЫ.":
                        (stars==2)?"ВЫПОЛНЕНО. ЕСТЬ СЛАБЫЕ МЕСТА.":
                        (stars==1)?"С ПОТЕРЯМИ. СЕРЬЁЗНЫЕ ОШИБКИ.":"ПРОВАЛ. ПОЗИЦИЯ ПОТЕРЯНА.";
    drawTxt("РЕЗУЛЬТАТ БОЯ",12,C::TEXT_DIM,px,py); py+=22.f;
    drawStars(stars,px,py,32.f); py+=46.f;
    drawTxt(verdict,18,vc,px,py); py+=36.f;
    for(const auto& d:state.currentMission->directions){
        DirectionResult dr=d.getResult();
        sf::Color dc=(dr==DirectionResult::SUPPRESSED)?C::GREEN:
                     (dr==DirectionResult::HELD)?C::GREEN_DIM:
                     (dr==DirectionResult::BREAKTHROUGH_LOSSES)?C::AMBER:C::RED;
        rect(px,py,bw-60.f,60.f,C::BG_CARD,dc); rect(px,py,4.f,60.f,dc,C::NONE);
        drawTxt(d.label+"  —  "+d.getResultText(),13,dc,px+12.f,py+6.f);
        std::ostringstream ss;
        ss<<"Наша: "<<std::fixed<<std::setprecision(0)<<d.getFriendlyPower()
          <<"  Угроза: "<<d.enemy_power
          <<"  Счёт: "<<std::fixed<<std::setprecision(0)<<d.getScore();
        drawTxt(ss.str(),11,C::TEXT,px+12.f,py+28.f); py+=68.f;
    }
    py+=8.f;
    float hw=(bw-60.f)*0.5f-8.f;
    rect(px,py,hw,44.f,C::BG_CARD,C::BORDER); drawTxt("ПЕРЕИГРАТЬ",13,C::TEXT,px+28.f,py+13.f);
    rect(px+hw+14.f,py,hw,44.f,sf::Color(10,40,10),C::GREEN,2.f);
    drawTxt("СЛЕДУЮЩАЯ МИССИЯ  ->",13,C::GREEN,px+hw+24.f,py+13.f);
}

// ============================================================
//  Ввод
// ============================================================
void Renderer::handleMouseScroll(float delta,sf::Vector2f pos){
    float col2x=COL1_W;
    if(pos.x>=col2x&&pos.x<=col2x+COL2_W&&pos.y>=PAN_Y&&pos.y<=H){
        float scrollH=PAN_H-30.f;
        float maxScroll=std::max(0.f,col2TotalH-scrollH);
        col2ScrollY=std::clamp(col2ScrollY-delta*30.f,0.f,maxScroll);
    }
}

void Renderer::handleMouseMove(sf::Vector2f pos){
    hoveredNode=-1; legendHover=-1;
    if(popup.visible||addDrop.visible)return;
    for(int i=0;i<(int)nodes.size();i++) if(nodeHit(i,pos)){hoveredNode=i;break;}

    // Hover по отделениям в легенде col2
    float col2x=COL1_W,scrollTop=PAN_Y+28.f,scrollH=PAN_H-30.f;
    if(pos.x>=col2x&&pos.x<=col2x+COL2_W&&pos.y>=scrollTop&&pos.y<=scrollTop+scrollH){
        // Пересчитаем позиции отделений
        float cy=scrollTop-col2ScrollY;
        cy+=16.f+3*42.f+6.f+16.f+16.f; // юниты + заголовки
        int sqH[]={50,50,50};
        for(int i=0;i<3;i++){
            float iy=cy;
            if(pos.y>=iy&&pos.y<=iy+sqH[i]&&pos.y>=scrollTop&&pos.y<=scrollTop+scrollH){
                legendHover=i; legendHoverPos={pos.x,iy}; break;
            }
            cy+=sqH[i]+0.f;
        }
    }
}

void Renderer::handleMouseClick(sf::Vector2f pos){
    if(!state.currentMission)return;

    // Кнопка В МЕНЮ (шапка)
    if(pos.x>=W-100.f&&pos.x<=W-12.f&&pos.y>=10.f&&pos.y<=42.f){
        wantsMenu=true; popup.visible=false; addDrop.visible=false; return;
    }

    // ── Дропдаун ──
    if(addDrop.visible){
        float dw = 360.f;
        float px = std::min(addDrop.x, W - dw - 6.f);
        float py = addDrop.y;

        // Режим: добавить юнита в взводное отделение через прямой указатель
        if(addDrop.squadIdx==-20 && addDrop.targetSquad!=nullptr){
            float iy=py+28.f;
            Unit cat[]={UnitCatalog::Rifleman(),UnitCatalog::Tank(),UnitCatalog::Artillery()};
            for(auto& u:cat){
                if(pos.y>=iy&&pos.y<=iy+32.f&&pos.x>=px+6.f&&pos.x<=px+dw-6.f){
                    if(state.remaining()>=u.cost &&
                       (int)addDrop.targetSquad->slots.size()<addDrop.targetSquad->max_slots){
                        addDrop.targetSquad->slots.push_back(u);
                        state.spent+=u.cost;
                    }
                    addDrop.visible=false; addDrop.targetSquad=nullptr; return;
                }
                iy+=36.f;
            }
            addDrop.visible=false; addDrop.targetSquad=nullptr; return;
        }

        // Режим: выбор типа отделения для добавления во взвод
        // addDrop.dirIdx = -(pi+2), addDrop.squadIdx = -10
        if(addDrop.squadIdx==-10){
            int pi = -(addDrop.dirIdx+2); // восстанавливаем индекс взвода
            float iy=py+28.f;
            SquadType sqT[]={SquadType::INFANTRY_SQUAD,
                             SquadType::TANK_SQUAD,
                             SquadType::ARTILLERY_SQUAD};
            for(int i=0;i<3;i++){
                if(pos.y>=iy&&pos.y<=iy+50.f&&pos.x>=px+6.f&&pos.x<=px+dw-6.f){
                    // Находим взвод на рубеже popup.dirIdx
                    auto& d=state.currentMission->directions[popup.dirIdx];
                    if(pi>=0&&pi<(int)d.platoons.size()){
                        Squad* sq2=state.createSquad(sqT[i]);
                        d.platoons[pi]->addSquad(sq2);
                        syncPopup();
                    }
                    addDrop.visible=false; return;
                }
                iy+=58.f;
            }
            addDrop.visible=false; return;
        }

        if (addDrop.squadIdx == -1) {
            float iy = py + 28.f;
            iy += 18.f; // <--- ДОБАВЛЕНО СМЕЩЕНИЕ: учитываем заголовок "ЮНИТЫ:"

            Unit cat[] = { UnitCatalog::Rifleman(),UnitCatalog::Tank(),UnitCatalog::Artillery() };

            for (auto& u : cat) {
                if (pos.y >= iy && pos.y <= iy + 20.f && pos.x >= px + 6.f && pos.x <= px + dw - 6.f) {
                    if (state.remaining() >= u.cost) {
                        auto& dir = state.currentMission->directions[addDrop.dirIdx];
                        dir.assignUnit(new Unit(u));
                        state.spent += u.cost;
                    }
                    addDrop.visible = false; syncPopup(); return;
                }
                iy += 22.f;
            }
            iy += 4.f + 18.f; // <--- ДОБАВЛЕНО СМЕЩЕНИЕ: учитываем заголовок "ОТДЕЛЕНИЯ:"

            // Отделения
            SquadType sqT[] = { SquadType::INFANTRY_SQUAD,SquadType::TANK_SQUAD,SquadType::ARTILLERY_SQUAD };
            for (int i = 0;i < 3;i++) {
                if (pos.y >= iy && pos.y <= iy + 50.f && pos.x >= px + 6.f && pos.x <= px + dw - 6.f) {
                    state.createSquad(sqT[i]);
                    state.deploySquad((int)state.squads.size() - 1, addDrop.dirIdx);
                    addDrop.visible = false; syncPopup(); return;
                }
                iy += 58.f;
            }
            iy += 4.f; // Отступ перед взводом

            // Взвод
            if (pos.y >= iy && pos.y <= iy + 60.f && pos.x >= px + 6.f && pos.x <= px + dw - 6.f) {
                state.createPlatoon();
                state.deployPlatoon((int)state.platoons.size() - 1, addDrop.dirIdx);
                addDrop.visible = false; syncPopup(); return;
            }
        } else {
            // Добавить юнита в отделение
            float iy=py+28.f;
            Unit cat[]={UnitCatalog::Rifleman(),UnitCatalog::Tank(),UnitCatalog::Artillery()};
            for(auto& u:cat){
                if(pos.y>=iy&&pos.y<=iy+32.f&&pos.x>=px+6.f&&pos.x<=px+dw-6.f){
                    state.addUnitToSquad(addDrop.dirIdx,addDrop.squadIdx,u);
                    addDrop.visible=false; return;
                }
                iy+=36.f;
            }
        }
        addDrop.visible=false; return;
    }

    // ── Попап ──
    if(popup.visible){
        syncPopup();
        const auto& d=state.currentMission->directions[popup.dirIdx];
        float pw=POPUP_W,ph=calcPopupH();
        float px=std::min(popup.x,W-pw-6.f);
        float py=std::min(popup.y,TOP_H+MAP_H-ph-4.f);
        if(py<TOP_H+4.f)py=TOP_H+4.f; if(px<4.f)px=4.f;

        // Кнопка [x] закрыть
        if(pos.x>=px+pw-38.f&&pos.x<=px+pw-8.f&&pos.y>=py+8.f&&pos.y<=py+30.f){
            popup.visible=false; addDrop.visible=false; return;
        }

        float cy=py+52.f+34.f+14.f;

        for (int ui = 0; ui < (int)d.units.size(); ui++) {
            if (pos.y >= cy + 3.f && pos.y <= cy + 23.f && pos.x >= px + pw - 45.f && pos.x <= px + pw - 7.f) {
                state.spent -= d.units[ui]->cost;
                state.currentMission->directions[popup.dirIdx].removeUnit(ui);
                syncPopup();
                return;
            }
            cy += 30.f;
        }

        // Взводы
        for(int pi=0;pi<(int)d.platoons.size();pi++){
            Platoon* pl=d.platoons[pi];
            bool pexp=popup.platoonExp[pi];

            // Кнопка УБРАТЬ взвод
            if(pos.y>=cy+5.f&&pos.y<=cy+27.f&&pos.x>=px+pw-50.f&&pos.x<=px+pw-8.f){
                state.undeployPlatoon(popup.dirIdx,pi);
                popup.platoonExp.erase(popup.platoonExp.begin()+pi);
                popup.platoonSquadExp.erase(popup.platoonSquadExp.begin()+pi);
                return;
            }
            // Клик по заголовку взвода
            if(pos.y>=cy&&pos.y<=cy+34.f&&pos.x>=px+4.f&&pos.x<=px+pw-54.f){
                popup.platoonExp[pi]=!popup.platoonExp[pi]; addDrop.visible=false; return;
            }
            cy+=38.f;

            if(pexp){
                for(int si=0;si<(int)pl->squads.size();si++){
                    Squad* sq=pl->squads[si];
                    bool sexp=popup.platoonSquadExp[pi][si];

                    // Кнопка УБРАТЬ отделение из взвода
                    if(pos.y>=cy+4.f&&pos.y<=cy+24.f&&pos.x>=px+pw-50.f&&pos.x<=px+pw-8.f){
                        // Возврат стоимости юнитов
                        state.spent -= sq->getTotalCost();
                        pl->removeSquad(si);
                        // Удалить из state.squads
                        for(auto it=state.squads.begin();it!=state.squads.end();++it)
                            if(it->get()==sq){state.squads.erase(it);break;}
                        syncPopup(); return;
                    }
                    // Клик по заголовку отделения (раскрыть/свернуть)
                    if(pos.y>=cy&&pos.y<=cy+30.f&&pos.x>=px+16.f&&pos.x<=px+pw-54.f){
                        popup.platoonSquadExp[pi][si]=!popup.platoonSquadExp[pi][si];
                        addDrop.visible=false; return;
                    }
                    cy+=32.f;
                    if(sexp){
                        for(int ui=0;ui<(int)sq->slots.size();ui++){
                            if(pos.y>=cy+1.f&&pos.y<=cy+21.f&&pos.x>=px+pw-38.f&&pos.x<=px+pw-8.f){
                                // Убрать юнит из взводного отделения — возврат стоимости
                                state.spent -= sq->slots[ui].cost;
                                sq->removeUnit(ui);
                                return;
                            }
                            cy+=24.f;
                        }
                        // + добавить юнита во взводное отделение — через targetSquad
                        if(pos.y>=cy&&pos.y<=cy+24.f&&pos.x>=px+24.f&&pos.x<=px+pw-6.f){
                            addDrop.visible     = true;
                            addDrop.squadIdx    = -20;   // флаг: добавить юнита через targetSquad
                            addDrop.targetSquad = sq;
                            addDrop.dirIdx      = popup.dirIdx;
                            addDrop.x = px+24.f; addDrop.y = cy+24.f;
                            return;
                        }
                        cy+=28.f;
                    }
                    cy+=4.f;
                }
                // + добавить отделение во взвод — показываем дропдаун выбора типа
                if(pos.y>=cy&&pos.y<=cy+26.f&&pos.x>=px+12.f&&pos.x<=px+pw-6.f){
                    addDrop.visible     = true;
                    addDrop.dirIdx      = -(pi+2);
                    addDrop.squadIdx    = -10;
                    addDrop.targetSquad = nullptr;
                    addDrop.x = px+12.f;
                    addDrop.y = cy+26.f;
                    return;
                }
                cy+=28.f;
            }
            cy+=4.f;
        }

        // Отдельные отделения
        for(int si=0;si<(int)d.squads.size();si++){
            Squad* sq=d.squads[si];
            bool sexp=popup.squadExp[si];

            // Кнопка УБРАТЬ отделение
            if(pos.y>=cy+5.f&&pos.y<=cy+27.f&&pos.x>=px+pw-50.f&&pos.x<=px+pw-8.f){
                state.undeploySquad(popup.dirIdx,si);
                popup.squadExp.erase(popup.squadExp.begin()+si);
                return;
            }
            // Клик по заголовку
            if(pos.y>=cy&&pos.y<=cy+32.f&&pos.x>=px+4.f&&pos.x<=px+pw-54.f){
                popup.squadExp[si]=!popup.squadExp[si]; addDrop.visible=false; return;
            }
            cy+=36.f;
            if(sexp){
                for(int ui=0;ui<(int)sq->slots.size();ui++){
                    if(pos.y>=cy+1.f&&pos.y<=cy+21.f&&pos.x>=px+pw-38.f&&pos.x<=px+pw-8.f){
                        state.removeUnitFromSquad(popup.dirIdx,si,ui); return;
                    }
                    cy+=24.f;
                }
                if(pos.y>=cy&&pos.y<=cy+24.f&&pos.x>=px+12.f&&pos.x<=px+pw-6.f){
                    addDrop.visible=true; addDrop.dirIdx=popup.dirIdx;
                    addDrop.squadIdx=si; addDrop.targetSquad=nullptr;
                    addDrop.x=px+12.f; addDrop.y=cy+24.f; return;
                }
                cy+=28.f;
            }
            cy+=4.f;
        }

        // Кнопка добавить на рубеж
        if(pos.y>=cy&&pos.y<=cy+30.f&&pos.x>=px+4.f&&pos.x<=px+pw-4.f){
            addDrop.visible=true; addDrop.dirIdx=popup.dirIdx; addDrop.squadIdx=-1;
            addDrop.targetSquad=nullptr; addDrop.x=px+4.f; addDrop.y=cy+30.f; return;
        }
        cy+=34.f;
        // Footer закрыть
        if(pos.y>=cy+6.f&&pos.y<=cy+30.f){popup.visible=false;addDrop.visible=false;}
        return;
    }

    // ── Результат ──
    if(state.phase==GamePhase::RESULT){
        float bw=760.f,bh=106.f+(float)state.currentMission->directions.size()*74.f+140.f;
        float bx=(W-bw)*0.5f,by=(H-bh)*0.5f; if(by<4.f)by=4.f;
        float px=bx+30.f,hw=(bw-60.f)*0.5f-8.f;
        float py=by+22.f+22.f+46.f+36.f+(float)state.currentMission->directions.size()*68.f+8.f;
        if(pos.y>=py&&pos.y<=py+44.f){
            if(pos.x>=px&&pos.x<=px+hw){state.reset(state.currentMission);buildMap(state.currentMission);}
            else if(pos.x>=px+hw+14.f) wantsNextMission=true;
        }
        return;
    }

    // ── Клик по узлу ──
    for(int i=0;i<(int)nodes.size();i++){
        if(nodeHit(i,pos)){
            if(popup.visible&&popup.dirIdx==i){popup.visible=false;}
            else{
                float pw=POPUP_W,ph=300.f;
                float ppx=nodes[i].pos.x-pw*0.5f,ppy=nodes[i].pos.y+nodes[i].r+12.f;
                if(ppy+ph>TOP_H+MAP_H-4.f) ppy=nodes[i].pos.y-nodes[i].r-ph-12.f;
                if(ppx<4.f)ppx=4.f; if(ppx+pw>W-4.f)ppx=W-pw-4.f;
                const auto& d=state.currentMission->directions[i];
                popup.openFor(i,ppx,ppy,(int)d.squads.size(),(int)d.platoons.size());
                addDrop.visible=false;
            }
            return;
        }
    }

    // ── Кнопка ВЫПОЛНИТЬ в col3 ──
    float col3x=COL1_W+COL2_W;
    if(pos.y>=PAN_Y&&pos.x>=col3x){
        float px=col3x+18.f,py=PAN_Y+12.f+20.f+18.f+18.f+34.f;
        if(pos.y>=py&&pos.y<=py+48.f) state.confirmAndCalculate();
    }
}

// ── Примитивы ───────────────────────────────────────────────
void Renderer::rect(float x,float y,float w,float h,sf::Color fill,sf::Color border,float thick){
    sf::RectangleShape s({w,h}); s.setPosition({x,y}); s.setFillColor(fill);
    if(border.a>0){s.setOutlineColor(border);s.setOutlineThickness(thick);}
    win.draw(s);
}
void Renderer::circle(float cx,float cy,float r,sf::Color fill,sf::Color border,float thick){
    sf::CircleShape s(r); s.setPosition({cx-r,cy-r}); s.setFillColor(fill);
    s.setOutlineColor(border); s.setOutlineThickness(thick); win.draw(s);
}
void Renderer::line(sf::Vector2f a,sf::Vector2f b,sf::Color col,float thick){
    sf::Vector2f d=b-a; float len=std::sqrt(d.x*d.x+d.y*d.y);
    if(len<0.01f)return;
    sf::RectangleShape s({len,thick}); s.setPosition(a);
    s.setRotation(sf::radians(std::atan2(d.y,d.x))); s.setFillColor(col); win.draw(s);
}
void Renderer::dashedLine(sf::Vector2f a,sf::Vector2f b,sf::Color col,float dash,float gap){
    sf::Vector2f d=b-a; float len=std::sqrt(d.x*d.x+d.y*d.y);
    if(len<0.01f)return; sf::Vector2f u=d/len; float t=0.f; bool dr=true;
    while(t<len){float seg=dr?dash:gap,end=std::min(t+seg,len);
        if(dr)line(a+u*t,a+u*end,col,2.5f);t=end;dr=!dr;}
}
void Renderer::arrowHead(sf::Vector2f tip,sf::Vector2f ud,sf::Color col,float sz){
    sf::Vector2f perp={-ud.y,ud.x},base=tip-ud*sz;
    sf::VertexArray tri(sf::PrimitiveType::Triangles,3);
    tri[0].position=tip; tri[1].position=base+perp*(sz*0.5f); tri[2].position=base-perp*(sz*0.5f);
    tri[0].color=tri[1].color=tri[2].color=col; win.draw(tri);
}
void Renderer::drawTxt(const std::string& s,unsigned sz,sf::Color col,float x,float y){
    sf::Text t(font,sf::String::fromUtf8(s.begin(),s.end()),sz);
    t.setFillColor(col); t.setPosition({x,y}); win.draw(t);
}
bool Renderer::txtVisible(float y,unsigned sz,float clipY,float clipH) const{
    return y+(float)sz>clipY&&y<clipY+clipH;
}
void Renderer::drawStars(int n,float x,float y,float sz){
    for(int i=0;i<3;i++)
        drawTxt("*",(unsigned)sz,(i<n)?C::STAR_ON:C::STAR_OFF,x+i*(sz+8.f),y);
}
sf::Color Renderer::nodeColor(int idx) const{
    if(!state.currentMission||idx>=(int)state.currentMission->directions.size()) return C::BORDER;
    int ep=state.currentMission->directions[idx].enemy_power;
    return (ep>=40)?C::RED:(ep>=25)?C::AMBER:C::GREEN_DIM;
}
bool Renderer::nodeHit(int idx,sf::Vector2f p) const{
    const auto& nd=nodes[idx]; float dx=p.x-nd.pos.x,dy=p.y-nd.pos.y;
    return std::sqrt(dx*dx+dy*dy)<=nd.r+10.f;
}
