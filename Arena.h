#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <memory>   // Pentru std::unique_ptr și std::make_unique
#include <random>   // Pentru std::mt19937 și std::uniform_real_distribution
#include <iostream> // Pentru std::cout

#include <SFML/Graphics.hpp> // Pentru sf::Texture
#include "Tiles.h" // Ensure this path is correct - acum include noul enum TileSpecialType

class Arena {
    static const int GridSize = 50; // Constant for the grid size, can be made larger

    // Membri pentru probabilități, declarați corect și const
    // Am ajustat probabilitățile pentru a fi mai ușor de citit și de înțeles
    const float teleporterTileProb = 0.01f; // 1%
    const float damageTileProb = 0.02f;     // 2%
    const float stickyTileProb = 0.02f;     // 2%
    const float healingTileProb = 0.005f;   // 0.5%
    const float totalSpecialProb = teleporterTileProb + damageTileProb + stickyTileProb + healingTileProb;


    // Grila va stoca pointeri inteligenți la Tile-uri
    std::vector<std::vector<std::unique_ptr<Tile>>> Grid;

    // Funcție membră pentru generarea probabilităților
    float Probability() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> random(0.0f, 1.0f); // Asigură un float între 0.0f și 1.0f
        return random(rng);
    };

public:
    // Constructor pentru tile-uri texturate
    Arena(sf::Texture& texture) {
        float tileSize = Tile::getSize(); // Presupunem ca Tile::getSize() este metoda statica
        Grid.reserve(GridSize); // Rezerva memorie pentru vectorul exterior (randuri)

        for (int i = 0; i < GridSize; ++i) {
            // Declaram si construim un vector pentru randul curent, care va contine unique_ptr<Tile>
            std::vector<std::unique_ptr<Tile>> currentRow;
            currentRow.reserve(GridSize); // Rezerva memorie pentru elementele din randul curent

            for (int j = 0; j < GridSize; ++j) {
                float roll = Probability();
                TileSpecialType type = NONE;
                std::unique_ptr<Tile> currentTile;

                // Determină tipul special al tile-ului
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

                // Acum, instanțiem tile-ul corect, pasând tipul special constructorului de bază
                // și folosind textura
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

                currentRow.push_back(std::move(currentTile)); // Folosim std::move pentru unique_ptr
            }
            Grid.push_back(std::move(currentRow)); // Mutăm rândul complet în Grid
        }
    }

    // Constructor pentru Arena cu tile-uri implicite (fără textură)
    Arena() {
        float tileSize = Tile::getSize();
        Grid.reserve(GridSize);

        for (int i = 0; i < GridSize; ++i) {
            std::vector<std::unique_ptr<Tile>> currentRow; // Vectorul interior va conține unique_ptr
            currentRow.reserve(GridSize);

            for (int j = 0; j < GridSize; ++j) {
                float roll = Probability();
                TileSpecialType type = NONE;
                std::unique_ptr<Tile> currentTile; // Pointerul inteligent pentru tile-ul curent

                // Determină tipul special al tile-ului
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

                // Acum, instanțiem tile-ul corect, pasând tipul special constructorului de bază
                // fără textură
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

    static int GetGridSize() {
        return GridSize;
    }

    std::vector<std::vector<std::unique_ptr<Tile>>>& GetGrid() {
        return Grid;
    }

    // NOU: Metodă pentru a reseta proprietatea tuturor tile-urilor (utilă la o nouă rundă)
    void resetAllTileOwnership() {
        for (auto& row : Grid) {
            for (auto& tilePtr : row) {
                if (tilePtr) {
                    tilePtr->resetTileOwnership();
                }
            }
        }
    }
};

#endif //ARENA_H