#include "Arena.h"
#include <iostream> // Pentru std::cout

#include "Tiles/Damage_Tile.h"
#include "Tiles/Healing_Tile.h"
#include "Tiles/Sticky_Tile.h"
#include "Tiles/Super_Tile.h"
#include "Tiles/Teleporter_Tile.h"

const int Arena::GridSize = 100;
std::unique_ptr<Arena> Arena::instance = nullptr;


Arena& Arena::getInstance() {
    if (instance == nullptr) {
        std::cout << "Warning: Arena::getInstance() called without texture. Creating Arena with default (textureless) tiles if this is the first call." << std::endl;
        instance = std::unique_ptr<Arena>(new Arena()); // Apelează constructorul Arena()
    }
    return *instance;
}

// getInstance cu textură (existent)
Arena& Arena::getInstance(sf::Texture& texture) {
    if (instance == nullptr) {
        instance = std::unique_ptr<Arena>(new Arena(texture));
    }
    return *instance;
}

// Probability (existent)
float Arena::Probability() {
    static std::mt19937 rng(std::random_device{}()); // Seed RNG o singură dată
    static std::uniform_real_distribution<float> random(0.0f, 1.0f);
    return random(rng);
}

// Constructor cu textură (existent, mici ajustări pentru claritate)
Arena::Arena(sf::Texture& texture) :
     // Stochează referința la textură dacă e necesară ulterior pentru altceva
    teleporterTileProb(0.005f), // Adaugă 'f' pentru float literals
    damageTileProb(0.02f),
    stickyTileProb(0.02f),
    healingTileProb(0.005f),
    superTileProb(0.01f)     // Probabilitatea pentru SuperTile
{
    float tileSize = Tile::getSize(); // Presupunem că Tile::getSize() este statică sau accesibilă
    Grid.resize(GridSize); // Redimensionează vectorul exterior

    for (int i = 0; i < GridSize; ++i) {
        Grid[i].reserve(GridSize); // Pre-alocă pentru eficiență
        for (int j = 0; j < GridSize; ++j) {
            float roll = Probability();
            TileSpecialType type = TileSpecialType::NONE;
            std::unique_ptr<Tile> currentTile;

            // Ajustează logica de probabilități cumulate
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
            // else type rămâne TileSpecialType::NONE

            // Pasează textura la constructorii de tile-uri
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
                    // Constructorul de bază Tile ar trebui să primească și el textura
                    currentTile = std::make_unique<Tile>(texture, static_cast<float>(j * tileSize), static_cast<float>(i * tileSize));
                    break;
            }
            Grid[i].push_back(std::move(currentTile));
        }
    }
}

// Constructor fără textură (existent, mici ajustări pentru claritate)
Arena::Arena() :// Inițializează cu un sf::Texture gol dacă e nevoie să existe membrul
    teleporterTileProb(0.005f),
    damageTileProb(0.02f),
    stickyTileProb(0.02f),
    healingTileProb(0.005f),
    superTileProb(0.01f)
{
    std::cout << "Arena default constructor (no texture) called." << std::endl;
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

            // Constructorii de tile-uri apelați aici NU primesc textură
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

// GetGridSize (existent)
int Arena::GetGridSize() {
    return GridSize;
}


std::vector<std::vector<std::unique_ptr<Tile>>>& Arena::GetGrid() {
    return Grid;
}

// getPlayerTileCounts (existent)
std::map<int, int> Arena::getPlayerTileCounts() const {
    std::map<int, int> counts;
    for (int i = 0; i < GridSize; ++i) {
        for (int j = 0; j < GridSize; ++j) {
            if (Grid[i][j]) { // Verifică dacă tile_ptr este valid
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

