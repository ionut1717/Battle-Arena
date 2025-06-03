#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <memory>   // For std::unique_ptr
#include <random>   // For std::random_device, std::mt19937, std::uniform_real_distribution
#include <map>      // For std::map
#include <SFML/Graphics.hpp>
#include "Tiles/Tiles.h" // Asigură-te că acest fișier definește Tile și TileSpecialType

class Arena {
private:
    static std::unique_ptr<Arena> instance;
    Arena(sf::Texture& texture);
    Arena();
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;
    Arena(const Arena&&) = delete; // Folosește Arena(Arena&&) pentru move constructor
    Arena& operator=(const Arena&&) = delete; // Folosește Arena& operator=(Arena&&) pentru move assignment

    static const int GridSize;
    const float teleporterTileProb;
    const float damageTileProb;
    const float stickyTileProb;
    const float healingTileProb;
    const float superTileProb;

    std::vector<std::vector<std::unique_ptr<Tile>>> Grid;

    float Probability();

public:
    static Arena& getInstance(); // Poate crea o arenă fără texturi dacă e apelat primul
    static Arena& getInstance(sf::Texture& texture);

    static int GetGridSize();
    std::vector<std::vector<std::unique_ptr<Tile>>>& GetGrid();

    std::map<int, int> getPlayerTileCounts() const;

    void draw(sf::RenderWindow& window, int perspectivaPlayerID);
};

#endif // ARENA_H