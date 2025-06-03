#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include "Tiles/Tiles.h"

class Arena {
private:
    static std::unique_ptr<Arena> instance;
    explicit Arena(sf::Texture& texture);
    Arena();
    static const int GridSize;
    const float teleporterTileProb;
    const float damageTileProb;
    const float stickyTileProb;
    const float healingTileProb;
    const float superTileProb;

    std::vector<std::vector<std::unique_ptr<Tile>>> Grid;

    float Probability();

public:
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;
    Arena(const Arena&&) = delete;
    Arena& operator=(const Arena&&) = delete;
    static Arena& getInstance();
    static Arena& getInstance(sf::Texture& texture);

    static int GetGridSize();
    std::vector<std::vector<std::unique_ptr<Tile>>>& GetGrid();

    std::map<int, int> getPlayerTileCounts() const;

    void draw(sf::RenderWindow& window, int perspectivaPlayerID);
};

#endif // ARENA_H