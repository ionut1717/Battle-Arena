#include "Arena.h"

#include <random>

#include "Tiles/Damage_Tile.h"
#include "Tiles/Healing_Tile.h"
#include "Tiles/Sticky_Tile.h"
#include "Tiles/Super_Tile.h"
#include "Tiles/Teleporter_Tile.h"

const int Arena::GridSize = 100;
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
    teleporterTileProb(0.005f),
    damageTileProb(0.02f),
    stickyTileProb(0.02f),
    healingTileProb(0.005f),
    superTileProb(0.002)
{
    float tileSize = Tile::getSize();
    Grid.resize(GridSize);

    for (int i = 0; i < GridSize; ++i) {
        Grid[i].reserve(GridSize);
        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = TileSpecialType::NONE;
            std::unique_ptr<Tile> currentTile;

            if (roll < teleporterTileProb) {
                type = TileSpecialType::TELEPORTER;
            } else if (roll < teleporterTileProb + damageTileProb) {
                type = TileSpecialType::DAMAGE;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb) {
                type = TileSpecialType::STICKY;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb) {
                type = TileSpecialType::HEALING;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb + superTileProb) {
                type = TileSpecialType::SUPER;
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
                case TileSpecialType::SUPER:
                    currentTile = std::make_unique<SuperTile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::NONE:
                default:
                    currentTile = std::make_unique<Tile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }
            Grid[i].push_back(std::move(currentTile));
        }
    }
}

Arena::Arena() :
    teleporterTileProb(0.005f),
    damageTileProb(0.02f),
    stickyTileProb(0.02f),
    healingTileProb(0.005f),
    superTileProb(0.01f)
{
    float tileSize = Tile::getSize();
    Grid.resize(GridSize);

    for (int i = 0; i < GridSize; ++i) {
        Grid[i].reserve(GridSize);
        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = TileSpecialType::NONE;
            std::unique_ptr<Tile> currentTile;

            if (roll < teleporterTileProb) {
                type = TileSpecialType::TELEPORTER;
            } else if (roll < teleporterTileProb + damageTileProb) {
                type = TileSpecialType::DAMAGE;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb) {
                type = TileSpecialType::STICKY;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb) {
                type = TileSpecialType::HEALING;
            } else if (roll < teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb + superTileProb) {
                type = TileSpecialType::SUPER;
            }

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
                case TileSpecialType::SUPER:
                    currentTile = std::make_unique<SuperTile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
                case TileSpecialType::NONE:
                default:
                    currentTile = std::make_unique<Tile>(static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }
            Grid[i].push_back(std::move(currentTile));
        }
    }
}

int Arena::GetGridSize() {
    return GridSize;
}


std::vector<std::vector<std::unique_ptr<Tile>>>& Arena::GetGrid() {
    return Grid;
}

std::map<int, int> Arena::getPlayerTileCounts() const {
    std::map<int, int> counts;
    for (int i = 0; i < GridSize; ++i) {
        for (int j = 0; j < GridSize; ++j) {
            if (Grid[i][j]) {
                int ownerID = Grid[i][j]->getOwner();
                if (ownerID != -1) {
                    counts[ownerID]++;
                }
            }
        }
    }
    return counts;
}

void Arena::draw(sf::RenderWindow& window, int perspectivaPlayerID) {
    if (Grid.empty()) {
        return;
    }
    for (const auto& row : Grid) {
        if (row.empty()) {
            continue;
        }
        for (const auto& tile_ptr : row) {
            if (tile_ptr) {
                tile_ptr->draw(window, perspectivaPlayerID);
            }
        }
    }
}

