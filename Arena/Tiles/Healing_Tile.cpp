//
// Created by ionut on 6/2/2025.
//

#include "Healing_Tile.h"
float HealingTile::healAmount=5;
HealingTile::HealingTile(float x, float y) : Tile(x, y, TileSpecialType::HEALING) {}
HealingTile::HealingTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, TileSpecialType::HEALING) {}
bool HealingTile::tryApplyHeal() {
    if (healingClock.getElapsedTime() >= healingCooldown) {
        healingClock.restart();
        return true;
    }
    return false;
}
