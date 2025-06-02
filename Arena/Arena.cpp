#include "Arena.h"
#include <iostream>

#include "Tiles/Damage_Tile.h"
#include "Tiles/Healing_Tile.h"
#include "Tiles/Sticky_Tile.h"
#include "Tiles/Super_Tile.h"
#include "Tiles/Teleporter_Tile.h"


///consts
const int Arena::GridSize = 50;

std::unique_ptr<Arena> Arena::instance = nullptr;

Arena& Arena::getInstance() {
    if (instance == nullptr) {
        instance = std::unique_ptr<Arena>(new Arena());
    }
    return *instance;
}


Arena& Arena::getInstance(sf::Texture& texture) {
    if (instance == nullptr) {
        instance = std::unique_ptr<Arena>(new Arena(texture));
    }
    return *instance;
}


float Arena::Probability() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> random(0.0f, 1.0f);
    return random(rng);
}
Arena::Arena(sf::Texture& texture) :
teleporterTileProb(0.005),
damageTileProb(0.02),
stickyTileProb(0.02),
healingTileProb(0.005),
superTileProb(0.01)
{
    float tileSize = Tile::getSize();
    Grid.reserve(GridSize);

    for (int i = 0; i < GridSize; ++i) {
        std::vector<std::unique_ptr<Tile>> currentRow;
        currentRow.reserve(GridSize);
        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = TileSpecialType::NONE;
            std::unique_ptr<Tile> currentTile;

            if (roll < teleporterTileProb) {
                type = TileSpecialType::TELEPORTER;
            } else if (roll < (teleporterTileProb + damageTileProb)) {
                type = TileSpecialType::DAMAGE;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb)) {
                type = TileSpecialType::STICKY;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb)) {
                type = TileSpecialType::HEALING;
            }
            else if(roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb+superTileProb)){
                type= TileSpecialType::SUPER;
            }
            else {
                type = TileSpecialType::NONE;
            }

            switch(type) {
                case TileSpecialType::TELEPORTER:
                    currentTile = std::make_unique<TeleporterTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::DAMAGE:
                    currentTile = std::make_unique<DamageTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::STICKY:
                    currentTile = std::make_unique<StickyTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::HEALING:
                    currentTile = std::make_unique<HealingTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::SUPER: // Handle SuperTile instantiation
                    currentTile = std::make_unique<SuperTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                default:
                    currentTile = std::make_unique<Tile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }

            currentRow.push_back(std::move(currentTile)); // Use std::move for unique_ptr
        }
        Grid.push_back(std::move(currentRow)); // Move the complete row into Grid
    }
}

// Constructor for Arena with default tiles (no texture) (now private)
Arena::Arena()  :
teleporterTileProb(0.005),
damageTileProb(0.02),
stickyTileProb(0.02),
healingTileProb(0.005),
superTileProb(0.01){
    float tileSize = Tile::getSize();
    Grid.reserve(GridSize);

    for (int i = 0; i < GridSize; ++i) {
        std::vector<std::unique_ptr<Tile>> currentRow; // Inner vector will contain unique_ptr
        currentRow.reserve(GridSize);

        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = TileSpecialType::NONE;
            std::unique_ptr<Tile> currentTile; // Smart pointer for the current tile

            // Determine the special tile type
            // IMPORTANT: Add SuperTile probability here
            if (roll < teleporterTileProb) {
                type = TileSpecialType::TELEPORTER;
            } else if (roll < (teleporterTileProb + damageTileProb)) {
                type = TileSpecialType::DAMAGE;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb)) {
                type = TileSpecialType::STICKY;
            } else if (roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb)) {
                type = TileSpecialType::HEALING;
            }
            // Add SuperTile probability. You need to define superTileProb in Arena.h
            // and adjust totalSpecialProb if you want to include it in the random generation.
            // For now, I'll add it as a separate check or you can integrate it into the existing chain.
            // For example, if you want SuperTile to be 0.001f (0.1%)
            // const float superTileProb = 0.001f;
            // const float totalSpecialProb = teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb + superTileProb;
            // if (roll < (teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb + superTileProb)) {
            //     type = SUPER;
            // }
            else { // If no special tile, default to NONE
                type = TileSpecialType::NONE;
            }

            // Now, instantiate the correct tile, passing the special type to the base constructor
            // without texture
            switch(type) {
                case TileSpecialType::TELEPORTER:
                    currentTile = std::make_unique<TeleporterTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::DAMAGE:
                    currentTile = std::make_unique<DamageTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::STICKY:
                    currentTile = std::make_unique<StickyTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::HEALING:
                    currentTile = std::make_unique<HealingTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::SUPER: // Handle SuperTile instantiation
                    currentTile = std::make_unique<SuperTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::NONE:
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