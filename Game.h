#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <optional>
#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <string>

#include "Player.h"
#include "Balloon.h"
#include "Resource_Manager.h"
#include "Arena/Arena.h" // Includem direct pentru Arena& în updateUI
#include "UI_Elements.h"   // Includem noua clasă UI

// class Arena; // Alternativ, forward declaration dacă nu e nevoie de definiția completă aici

class Game_Engine {
protected:
    Game_Engine();

public:
    static Game_Engine& Instance();
    Game_Engine(const Game_Engine&) = delete;
    Game_Engine(Game_Engine&&) = delete;
    Game_Engine& operator=(const Game_Engine&) = delete;
    Game_Engine& operator=(Game_Engine&&) = delete;

private:
    sf::Clock gameClock;
    sf::Clock spawnClock;
    std::vector<AttackBalloon> activeBalloons;

    std::mt19937 rng;
    std::uniform_int_distribution<int> coordDist;

    sf::Font gameFont;        // Fontul principal, încărcat aici și pasat la UI_Elements
    UI_Elements uiHandler;    // Instanța clasei UI_Elements

    bool gameEnded = false;
    std::string currentWinMessage; // Mesajul de câștig stabilit de checkWinCondition

public:
    void GameLoop();

private:
    void processPlayerTileInteraction(Player& player, float dt, std::vector<std::vector<std::unique_ptr<Tile>>>& grid);
    // updateUI și drawUI vor delega către uiHandler
    void updateUIDelegator(unsigned int windowWidth, unsigned int windowHeight, Player& player1, Player& player2, const Arena& arena);
    void drawUIDelegator(sf::RenderWindow& window);
    int checkWinCondition(Player& player1, Player& player2, const Arena& arena); // Am scos const din fata Arena&
};

#endif // GAME_H