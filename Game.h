#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <string>

#include "Player.h"
#include "Balloon.h"
#include "Arena/Arena.h"
#include "UI_Elements.h"


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

    sf::Font gameFont;
    UI_Elements uiHandler;

    bool gameEnded = false;
    std::string currentWinMessage;

public:
    void GameLoop();

private:
    void processPlayerTileInteraction(Player& player, float dt, std::vector<std::vector<std::unique_ptr<Tile>>>& grid);
    void updateUIDelegator(unsigned int windowWidth, unsigned int windowHeight, Player& player1, Player& player2, const Arena& arena);
    void drawUIDelegator(sf::RenderWindow& window);
    int checkWinCondition(Player& player1, Player& player2, const Arena& arena);
};

#endif // GAME_H