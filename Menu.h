// Menu.h
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Game_State.h" // Includem enum-ul GameState dintr-un fișier separat

// Enum pentru acțiunile specifice pe care le poate declanșa meniul.
enum class MenuAction {
    None,
    Play,
    Settings,
    Quit
};

class Menu {
private:
    sf::Font font;
    sf::RectangleShape backgroundRect;
    sf::Text gameTitle;

    struct Button1 {
        sf::Text text;
        sf::FloatRect bounds; // Limitele butonului (accesate cu position.x/y, size.x/y)
        sf::Color normalColor;
        sf::Color hoverColor;
        MenuAction action;
    };
    std::vector<Button1> buttons;

    GameState currentMenuState;

public:
    Menu(unsigned int windowWidth, unsigned int windowHeight) : currentMenuState(GameState::Menu),gameTitle(font) {
        if (!font.openFromFile("../Font.ttf")) {
            std::cerr << "Eroare critica: Nu s-a putut incarca fontul pentru meniu! Asigurati-va ca 'Font.ttf' este in directorul corect (sau adaptați calea). Meniul nu va functiona corect." << std::endl;
        }

        backgroundRect.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
        backgroundRect.setFillColor(sf::Color(50, 50, 50, 200));

        gameTitle.setFont(font);
        gameTitle.setString("Paint Battle");
        gameTitle.setCharacterSize(static_cast<unsigned int>(std::round(windowHeight * 0.12f)));
        gameTitle.setFillColor(sf::Color::White);

        sf::FloatRect titleRect = gameTitle.getLocalBounds();
        // ATENTIE: Modificat aici pentru a folosi .position.x/y și .size.x/y
        gameTitle.setOrigin({std::round(titleRect.position.x + titleRect.size.x / 2.0f),
                            std::round(titleRect.position.y + titleRect.size.y / 2.0f)});
        gameTitle.setPosition({std::round(windowWidth / 2.0f), std::round(windowHeight / 4.0f)});

        float buttonCharacterSize = static_cast<float>(std::round(windowHeight * 0.07f));
        float dynamicButtonSpacing = windowHeight * 0.03f;
        float startY = windowHeight / 2.0f;

        // auto createAndAddButton = [&](const std::string& textString, float yOffset, MenuAction action) {
        //     Button1 btn{
        //         btn.text.setFont(font),
        //         btn.text.setString(textString),
        //         btn.text.setCharacterSize(static_cast<unsigned int>(buttonCharacterSize))
        //     };
        //     btn.text.setFillColor(sf::Color::White);
        //     btn.normalColor = sf::Color::White;
        //     btn.hoverColor = sf::Color::Yellow;
        //
        //     sf::FloatRect textBounds = btn.text.getLocalBounds();
        //     // ATENTIE: Modificat aici pentru a folosi .position.x/y și .size.x/y
        //     btn.text.setOrigin({std::round(textBounds.position.x + textBounds.size.x / 2.0f),
        //                        std::round(textBounds.position.y + textBounds.size.y / 2.0f)});
        //     btn.text.setPosition({std::round(windowWidth / 2.0f), std::round(startY + yOffset)});
        //     btn.bounds = btn.text.getGlobalBounds();
        //     btn.action = action;
        //     buttons.push_back(btn);
        // };
        //
        // createAndAddButton("Play", 0.0f, MenuAction::Play);
        // // Când accesezi getGlobalBounds().height, presupunem că și acest sf::FloatRect
        // // returnat are membrii .position și .size.
        // float firstButtonHeight = buttons.empty() ? 0.0f : buttons[0].text.getGlobalBounds().size.y; // Modificat aici
        // createAndAddButton("Settings", firstButtonHeight + dynamicButtonSpacing, MenuAction::Settings);
        // createAndAddButton("Quit", 2 * (firstButtonHeight + dynamicButtonSpacing), MenuAction::Quit);
    }

    MenuAction handleEvent(sf::RenderWindow& window, const sf::Event& event) {
        if (const auto* mouseButtonReleasedEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseButtonReleasedEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(mouseButtonReleasedEvent->position);

                for (const auto& button : buttons) {
                    // Metoda contains() a sf::FloatRect ar trebui să funcționeze corect,
                    // indiferent de modul intern de stocare (left/top/width/height sau position/size).
                    if (button.bounds.contains(mousePos)) {
                        if (button.action == MenuAction::Play) {
                            currentMenuState = GameState::Playing;
                            std::cout << "Meniu: Butonul 'Play' a fost apasat." << std::endl;
                        } else if (button.action == MenuAction::Settings) {
                            currentMenuState = GameState::Settings;
                            std::cout << "Meniu: Butonul 'Settings' a fost apasat." << std::endl;
                        } else if (button.action == MenuAction::Quit) {
                            currentMenuState = GameState::Exiting;
                            std::cout << "Meniu: Butonul 'Quit' a fost apasat. Aplicatia se va inchide." << std::endl;
                        }
                        return button.action;
                    }
                }
            }
            return MenuAction::None;
        }
    };

    void update(sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        for (auto& button : buttons) {
            bool isHovered = button.bounds.contains(mousePos);
            button.text.setFillColor(isHovered ? button.hoverColor : button.normalColor);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(backgroundRect);
        window.draw(gameTitle);

        for (const auto& button : buttons) {
            window.draw(button.text);
        }
    }

    void resetMenuState() {
        currentMenuState = GameState::Menu;
        for (auto& button : buttons) {
            button.text.setFillColor(button.normalColor);
        }
    }

    GameState getCurrentMenuState() const {
        return currentMenuState;
    }
};

#endif // MENU_H