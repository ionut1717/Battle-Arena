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

class Game_Engine {
protected:
    Game_Engine()=default;

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
    float spawnCooldown = 2.0f;
    float player1ZoomFactor = 1.0f;
    float player2ZoomFactor = 1.0f;
    const float zoomSpeed = 0.9f;
    const float minZoom = 0.5f;
    const float maxZoom = 2.0f;
    const float initialLaunchForce = 1500.0f;
    const float spawnOffsetDistance = 30.0f;

    std::mt19937 rng;
    std::uniform_int_distribution<int> coordDist;

    // sf::RectangleShape ownershipBarBackground;
    // sf::RectangleShape player1OwnershipBar;
    // sf::RectangleShape player2OwnershipBar;
    // sf::Text player1PercentageText;
    // sf::Text player2PercentageText;
    // sf::Text totalTilesText;

public:
    void GameLoop();

private:
    void processPlayerTileInteraction(Player& player, float dt, std::vector<std::vector<std::unique_ptr<Tile>>>& grid);
};

#endif // GAME_H