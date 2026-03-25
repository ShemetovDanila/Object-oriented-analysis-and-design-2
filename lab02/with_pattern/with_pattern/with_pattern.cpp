#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "Mission.h"
#include "Renderer.h"
#include "Menu.h"

enum class AppState { MENU, GAME };

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Mission m1 = MissionFactory::createMission1();
    Mission m2 = MissionFactory::createMission2();
    std::vector<Mission*> missions = { &m1, &m2 };

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1920u, 900u)), "COMMANDER");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
            return -1;

    GameState state;
    Renderer  renderer(window, font, state);
    Menu      menu(window, font);
    AppState  appState = AppState::MENU;

    while (window.isOpen()) {
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* mm = ev->getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f pos = {(float)mm->position.x, (float)mm->position.y};
                if (appState == AppState::MENU) menu.handleMouseMove(pos);
                else                            renderer.handleMouseMove(pos);
            }
            if (const auto* mw = ev->getIf<sf::Event::MouseWheelScrolled>())
                if (appState == AppState::GAME)
                    renderer.handleMouseScroll(mw->delta,
                        {(float)mw->position.x, (float)mw->position.y});

            if (const auto* mc = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mc->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = {(float)mc->position.x, (float)mc->position.y};
                    if (appState == AppState::MENU) {
                        menu.handleMouseClick(pos);
                        int sel = menu.getSelectedLevel();
                        if (sel >= 0 && sel < (int)missions.size()) {
                            state.currentMissionIndex = sel;
                            state.reset(missions[sel]);
                            renderer.buildMap(missions[sel]);
                            menu.resetSelection();
                            appState = AppState::GAME;
                        }
                    } else {
                        renderer.handleMouseClick(pos);
                        if (renderer.wantsMenu) {
                            renderer.wantsMenu = false;
                            appState = AppState::MENU;
                        }
                        if (renderer.wantsNextMission) {
                            renderer.wantsNextMission = false;
                            int next = state.currentMissionIndex + 1;
                            if (next < (int)missions.size()) {
                                state.currentMissionIndex = next;
                                state.reset(missions[next]);
                                renderer.buildMap(missions[next]);
                            } else {
                                appState = AppState::MENU;
                            }
                        }
                    }
                }
            }
            if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    if (appState == AppState::GAME) appState = AppState::MENU;
                    else window.close();
                }
                if (kp->code == sf::Keyboard::Key::R && appState == AppState::GAME) {
                    state.reset(state.currentMission);
                    renderer.buildMap(state.currentMission);
                }
            }
        }
        if (appState == AppState::MENU) menu.draw();
        else                            renderer.draw();
        window.display();
    }
    return 0;
}
