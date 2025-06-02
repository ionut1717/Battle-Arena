#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <memory>   // For std::unique_ptr
#include <random>   // For std::random_device, std::mt19937, std::uniform_real_distribution
#include <map>      // For std::map
#include <SFML/Graphics.hpp>
#include "Tiles/Tiles.h"

class Arena {
private:
    static std::unique_ptr<Arena> instance;
    Arena(sf::Texture& texture);
    Arena();
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;
    Arena(const Arena&&) = delete;
    Arena& operator=(const Arena&&) = delete;

    static const int GridSize;
    const float teleporterTileProb;
    const float damageTileProb;
    const float stickyTileProb;
    const float healingTileProb;
    const float superTileProb;


    std::vector<std::vector<std::unique_ptr<Tile>>> Grid;


    float Probability();

public:
    static Arena& getInstance();
    static Arena& getInstance(sf::Texture& texture);

    static int GetGridSize();

    std::vector<std::vector<std::unique_ptr<Tile>>>& GetGrid();

    // New methods for tile ownership counts
    int getTotalTiles() const;
    std::map<int, int> getPlayerTileCounts() const;
};

#endif // ARENA_H