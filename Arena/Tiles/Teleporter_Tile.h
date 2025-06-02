//
// Created by ionut on 6/2/2025.
//

#ifndef TELEPORTER_TILE_H
#define TELEPORTER_TILE_H
#include "Tiles.h"


class TeleporterTile : public Tile {
public:
    TeleporterTile(float x, float y);
    TeleporterTile(sf::Texture const& texture, float x, float y);

};



#endif //TELEPORTER_TILE_H
