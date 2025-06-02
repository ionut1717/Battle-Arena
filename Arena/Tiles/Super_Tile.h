//
// Created by ionut on 6/2/2025.
//

#ifndef SUPER_TILE_H
#define SUPER_TILE_H
#include "Damage_Tile.h"
#include "Sticky_Tile.h"


class SuperTile : virtual public StickyTile, public DamageTile {
public:
    SuperTile(float x, float y);
    SuperTile(sf::Texture const& texture, float x, float y);
    void static getData(float& out_slowDown, float& out_anotherValue);
};


#endif //SUPER_TILE_H
