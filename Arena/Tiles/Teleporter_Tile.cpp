//
// Created by ionut on 6/2/2025.
//

#include "Teleporter_Tile.h"
TeleporterTile::TeleporterTile(float x, float y) : Tile(x, y, TileSpecialType::TELEPORTER) {}
TeleporterTile::TeleporterTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, TileSpecialType::TELEPORTER) {}
