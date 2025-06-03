//
// Created by ionut on 6/2/2025.
//

#include "Super_Tile.h"
float SuperTile::slow_down_factor=0.2;
SuperTile::SuperTile(float x, float y)
    : Tile(x, y, TileSpecialType::SUPER),
      StickyTile(x, y),
      DamageTile(x, y) {
    initialSpecialType = TileSpecialType::SUPER;
}
SuperTile::SuperTile(sf::Texture const& texture, float x, float y)
    : Tile(texture, x, y, TileSpecialType::SUPER),
      StickyTile(texture, x, y),
      DamageTile(texture, x, y) {
    initialSpecialType = TileSpecialType::SUPER;
}
// for future updates
void SuperTile::getData(float& out_slowDown, float& out_anotherValue) {
    out_slowDown = slow_down_factor;
    out_anotherValue = DamageTile::getDamageAmount();
}