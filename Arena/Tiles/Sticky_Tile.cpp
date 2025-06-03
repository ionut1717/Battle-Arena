//
// Created by ionut on 6/2/2025.
//

#include "Sticky_Tile.h"
float StickyTile::slow_down=0.4;
StickyTile::StickyTile(float x, float y)
    : Tile(x, y, TileSpecialType::STICKY) {
    initialSpecialType = TileSpecialType::STICKY;
}
StickyTile::StickyTile(sf::Texture const& texture, float x, float y)
    : Tile(texture, x, y, TileSpecialType::STICKY) {
    initialSpecialType = TileSpecialType::STICKY;
}
float StickyTile::getslowfactor() {
    return slow_down;
}