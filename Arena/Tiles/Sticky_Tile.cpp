//
// Created by ionut on 6/2/2025.
//

#include "Sticky_Tile.h"
float StickyTile::slow_down=0.8;
StickyTile::StickyTile(float x, float y)
    : Tile(x, y, TileSpecialType::STICKY) { // Call base Tile constructor
    initialSpecialType = TileSpecialType::STICKY; // Ensure type is set correctly
}
StickyTile::StickyTile(sf::Texture const& texture, float x, float y)
    : Tile(texture, x, y, TileSpecialType::STICKY) { // Call base Tile constructor
    initialSpecialType = TileSpecialType::STICKY;
}