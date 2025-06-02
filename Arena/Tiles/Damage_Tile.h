//
// Created by ionut on 6/2/2025.
//

#ifndef DAMAGE_TILE_H
#define DAMAGE_TILE_H
#include "Tiles.h"


class DamageTile : public virtual Tile {
protected:
    static float damageAmount;
    sf::Clock damageClock;
    sf::Time damageCooldown = sf::seconds(1.0f);
public:
    static float getDamageAmount() { return damageAmount; }
    DamageTile(float x, float y);
    DamageTile(sf::Texture const& texture, float x, float y);
    virtual bool tryApplyDamage();
};


#endif
