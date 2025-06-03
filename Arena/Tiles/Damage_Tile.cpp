//
// Created by ionut on 6/2/2025.
//

#include "Damage_Tile.h"
float DamageTile::damageAmount=5;


DamageTile::DamageTile(float x, float y): Tile(x, y, TileSpecialType::DAMAGE) {
    initialSpecialType = TileSpecialType::DAMAGE;
    damageClock.restart();
}
DamageTile::DamageTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, TileSpecialType::DAMAGE) {
    initialSpecialType = TileSpecialType::DAMAGE;
    damageClock.restart();
}

bool DamageTile::tryApplyDamage() {
    if (damageClock.getElapsedTime() >= damageCooldown) {
        damageClock.restart();
        return true;
    }
    return false;
}
