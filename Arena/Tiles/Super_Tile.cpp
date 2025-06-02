//
// Created by ionut on 6/2/2025.
//

#include "Super_Tile.h"
SuperTile::SuperTile(float x, float y)
    : Tile(x, y, TileSpecialType::SUPER), // Initialize the virtual base Tile
      StickyTile(x, y),  // Initialize StickyTile part
      DamageTile(x, y) {
    initialSpecialType = TileSpecialType::SUPER;
}

// SuperTile constructor for textured tiles
SuperTile::SuperTile(sf::Texture const& texture, float x, float y)
    : Tile(texture, x, y, TileSpecialType::SUPER), // Initialize the virtual base Tile
      StickyTile(texture, x, y),  // Initialize StickyTile part
      DamageTile(texture, x, y) { // Initialize DamageTile part
    initialSpecialType = TileSpecialType::SUPER;
    // Culoarea va fi setată de Tile::getTileTypeColor(SUPER)
}
void static getData(float& out_slowDown, float& out_anotherValue) {
    out_slowDown = StickyTile::get_slow_down();
    out_anotherValue = DamageTile::getDamageAmount();
}