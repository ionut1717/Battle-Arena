#include "Arena.h"
#include "Tiles.h" // Needed for Tile::getSize() and derived Tile classes
#include <iostream> // For std::cout/cerr

// Define the static const member outside the class
// const int Arena::GridSize = 50; // Not needed if defined with constexpr in header

// Private helper function for probability generation
float Arena::Probability() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> random(0.0f, 1.0f); // Ensures float between 0.0f and 1.0f
    return random(rng);
}

// Constructor for textured tiles
Arena::Arena(sf::Texture& texture) {
    float tileSize = Tile::getSize(); // Assume Tile::getSize() is a static method
    Grid.reserve(GridSize); // Reserve memory for the outer vector (rows)

    for (int i = 0; i < GridSize; ++i) {
        // Declare and construct a vector for the current row, which will contain unique_ptr<Tile>
        std::vector<std::unique_ptr<Tile>> currentRow;
        currentRow.reserve(GridSize); // Reserve memory for elements in the current row

        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = NONE;
            std::unique_ptr<Tile> currentTile;

            // Determine the special tile type based on probabilities
            if (roll < teleporterTileProb) {
                type = TELEPORTER;
            } else if (roll < (teleporterTileProb + damageTileProb)) {
                type = DAMAGE;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb)) {
                type = STICKY;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb)) {
                type = HEALING;
            } else {
                type = NONE;
            }

            // Now, instantiate the correct tile, passing the special type to the base constructor
            // and using the texture
            switch(type) {
                case TELEPORTER:
                    currentTile = std::make_unique<TeleporterTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case DAMAGE:
                    currentTile = std::make_unique<DamageTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case STICKY:
                    currentTile = std::make_unique<StickyTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case HEALING:
                    currentTile = std::make_unique<HealingTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case NONE:
                default:
                    currentTile = std::make_unique<Tile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }

            currentRow.push_back(std::move(currentTile)); // Use std::move for unique_ptr
        }
        Grid.push_back(std::move(currentRow)); // Move the complete row into Grid
    }
}

// Constructor for Arena with default tiles (no texture)
Arena::Arena() {
    float tileSize = Tile::getSize();
    Grid.reserve(GridSize);

    for (int i = 0; i < GridSize; ++i) {
        std::vector<std::unique_ptr<Tile>> currentRow; // Inner vector will contain unique_ptr
        currentRow.reserve(GridSize);

        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = NONE;
            std::unique_ptr<Tile> currentTile; // Smart pointer for the current tile

            // Determine the special tile type
            if (roll < teleporterTileProb) {
                type = TELEPORTER;
            } else if (roll < (teleporterTileProb + damageTileProb)) {
                type = DAMAGE;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb)) {
                type = STICKY;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb)) {
                type = HEALING;
            } else {
                type = NONE;
            }

            // Now, instantiate the correct tile, passing the special type to the base constructor
            // without texture
            switch(type) {
                case TELEPORTER:
                    currentTile = std::make_unique<TeleporterTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case DAMAGE:
                    currentTile = std::make_unique<DamageTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case STICKY:
                    currentTile = std::make_unique<StickyTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case HEALING:
                    currentTile = std::make_unique<HealingTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case NONE:
                default:
                    currentTile = std::make_unique<Tile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }
            currentRow.push_back(std::move(currentTile));
        }
        Grid.push_back(std::move(currentRow));
    }
}

// Static method to get grid size
int Arena::GetGridSize() {
    return GridSize;
}

// Method to get the grid (access to the vector of tiles)
std::vector<std::vector<std::unique_ptr<Tile>>>& Arena::GetGrid() {
    return Grid;
}

// Method to reset ownership of all tiles (useful for a new round)
void Arena::resetAllTileOwnership() {
    for (auto& row : Grid) {
        for (auto& tilePtr : row) {
            if (tilePtr) {
                tilePtr->resetTileOwnership();
            }
        }
    }
}

// New method: Get total number of tiles
int Arena::getTotalTiles() const {
    return GridSize * GridSize;
}

// New method: Get player tile counts
std::map<int, int> Arena::getPlayerTileCounts() const {
    std::map<int, int> counts;
    for (int i = 0; i < GridSize; ++i) {
        for (int j = 0; j < GridSize; ++j) {
            int ownerID = Grid[i][j]->getOwner();
            if (ownerID != -1) { // Only count owned tiles
                counts[ownerID]++;
            }
        }
    }
    return counts;
}