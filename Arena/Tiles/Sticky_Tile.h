//
// Created by ionut on 6/2/2025.
//

#ifndef STICKY_TILE_H
#define STICKY_TILE_H
#include "Tiles.h"


class StickyTile : public virtual Tile {
protected:
    static float slow_down;
public:
    static float get_slow_down() { return slow_down; }
    StickyTile(float x, float y);
    StickyTile(sf::Texture const& texture, float x, float y);
    float getslowfactor();
};



#endif //STICKY_TILE_H
