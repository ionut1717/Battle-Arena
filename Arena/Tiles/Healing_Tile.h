//
// Created by ionut on 6/2/2025.
//

#ifndef HEALING_TILE_H
#define HEALING_TILE_H
#include "Tiles.h"


class HealingTile : public Tile {
    static float healAmount;
    sf::Clock healingClock;
    sf::Time healingCooldown = sf::seconds(0.5f);
public:
    static float gethealAmount(){return healAmount;};
    HealingTile(float x, float y);
    HealingTile(sf::Texture const& texture, float x, float y);
    bool tryApplyHeal();
};


#endif //HEALING_TILE_H
