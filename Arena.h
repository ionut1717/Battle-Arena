#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <memory>   // Pentru std::unique_ptr și std::make_unique
#include <random>   // Pentru std::mt19937 și std::uniform_real_distribution (needed for private member)
#include <map>      // For getPlayerTileCounts method
#include <SFML/Graphics.hpp> // Pentru sf::Texture
#include "Tiles.h" // Ensure this path is correct - acum include noul enum TileSpecialType

// Forward declarations if needed, but Tiles.h is already included.

class Arena {
private:
    static const int GridSize=50; // Declared here, defined in .cpp or as inline static constexpr
                               // Keeping it as static const int GridSize = 50; in header is also common
                               // but for strict separation, define in .cpp.
                               // However, if it's used in other headers (like Player.h for Tile::getSize()),
                               // it needs to be defined in the header. Let's keep it in header for now.
    // Defined here as it's a compile-time constant.

    // Membri pentru probabilități, declarați corect și const
    const float teleporterTileProb = 0.01f; // 1%
    const float damageTileProb = 0.02f;     // 2%
    const float stickyTileProb = 0.02f;     // 2%
    const float healingTileProb = 0.005f;   // 0.5%
    const float totalSpecialProb = teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb;


    // Grila va stoca pointeri inteligenți la Tile-uri
    std::vector<std::vector<std::unique_ptr<Tile>>> Grid;

    // Funcție membră privată pentru generarea probabilităților
    float Probability();

public:
    // Constructors
    Arena(sf::Texture& texture); // Constructor for textured tiles
    Arena(); // Constructor for Arena with default tiles (no texture)

    // Static methods
    static int GetGridSize();

    // Member functions
    std::vector<std::vector<std::unique_ptr<Tile>>>& GetGrid();

    // New: Method to reset ownership of all tiles (useful for a new round)
    void resetAllTileOwnership();

    // New methods for tile ownership counts
    int getTotalTiles() const;
    std::map<int, int> getPlayerTileCounts() const;
};

#endif //ARENA_H