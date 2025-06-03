//
// Created by ionut on 6/2/2025.
//

#ifndef SUPER_TILE_H
#define SUPER_TILE_H
#include "Damage_Tile.h"
#include "Sticky_Tile.h"


class SuperTile : public StickyTile, public DamageTile {
    static float slow_down_factor;
public:
    SuperTile(float x, float y);
    SuperTile(sf::Texture const& texture, float x, float y);
    void getData(float& out_slowDown, float& out_anotherValue);
};


#endif //SUPER_TILE_H
