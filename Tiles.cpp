#include "Tiles.h"
Tile::~Tile() = default;

// Constructor for non-textured tiles
Tile::Tile(const float x, const float y, TileSpecialType type)
    : coordinates(x, y), has_texture(false), initialSpecialType(type) {
    tileShape.setPosition(coordinates);
    tileShape.setSize({TileSize, TileSize});
    tileShape.setOutlineColor(sf::Color::Black);
    tileShape.setOutlineThickness(1.f);
    tileShape.setFillColor(getTileTypeColor(initialSpecialType));
    ownerColor = getTileTypeColor(initialSpecialType);
}

// Constructor for textured tiles
Tile::Tile(sf::Texture const& texture, const float x, const float y, TileSpecialType type)
    : coordinates(x, y), has_texture(true), initialSpecialType(type) {
    tileShape.setPosition(coordinates);
    tileShape.setSize({TileSize, TileSize});
    tileShape.setOutlineColor(sf::Color::Black);
    tileShape.setOutlineThickness(1.f);
    tileShape.setFillColor(sf::Color::White); // Base color for outline/shadow

    sf::Sprite sprite(texture);
    float scaleX = TileSize / static_cast<float>(texture.getSize().x);
    float scaleY = TileSize / static_cast<float>(texture.getSize().y);
    sprite.setScale({scaleX, scaleY});
    sprite.setPosition(coordinates);
    tileSprites.push_back(sprite);

    ownerColor = sf::Color::White; // Initial ownerColor for textured tiles
}

// Copy constructor implementation
Tile::Tile(const Tile& other)
    : tileShape(other.tileShape),
      coordinates(other.coordinates),
      tileSprites(other.tileSprites),
      has_texture(other.has_texture),
      ownerPlayerID(other.ownerPlayerID),
      ownerColor(other.ownerColor),
      initialSpecialType(other.initialSpecialType) {
        if (has_texture && !tileSprites.empty() && other.tileSprites.empty() == false) {
            // If texture was set, ensure the sprite points to the same texture
            // This is a shallow copy for the texture itself, which is fine as textures are shared.
            tileSprites.front().setTexture(other.tileSprites.front().getTexture());
        }
    }


void Tile::setTexture(const sf::Texture& texture) {
    tileSprites.clear();
    sf::Sprite sprite(texture);
    float scaleX = TileSize / static_cast<float>(texture.getSize().x);
    float scaleY = TileSize / static_cast<float>(texture.getSize().y);
    sprite.setScale({scaleX, scaleY});
    sprite.setPosition(coordinates);
    tileSprites.push_back(sprite);
    has_texture = true;
    tileShape.setFillColor(sf::Color::White);
    ownerColor = sf::Color::White;
}

void Tile::draw(sf::RenderWindow& window, int viewingPlayerID) {
    sf::Color finalDrawColor = sf::Color::White; // Default for normal, unowned textured tiles

    if (ownerPlayerID != -1) { // Tile is owned
        if (viewingPlayerID == ownerPlayerID) { // Viewer is the owner
            if (initialSpecialType != NONE) {
                // Owner of a special tile sees its special property with a tint
                finalDrawColor = getTileTypeColor(initialSpecialType);
                // Add player's color tint if textured, making it a blend
                if (has_texture) {
                    finalDrawColor = sf::Color(
                        ((finalDrawColor.r * 0.7 + ownerColor.r * 0.3)),
                        ((finalDrawColor.g * 0.7 + ownerColor.g * 0.3)),
                        ((finalDrawColor.b * 0.7 + ownerColor.b * 0.3)),
                        255 // Full opacity
                    );
                }
            } else {
                // Owner of a normal tile sees their claim color
                finalDrawColor = ownerColor;
            }
        } else { // Viewer is NOT the owner (it's owned by an opponent)
            // Opponent always sees the owner's color, regardless of special type
            finalDrawColor = ownerColor;
        }
    } else { // Tile is unowned
        if (initialSpecialType != NONE) {
            // Unowned special tiles are shown with their special color
            finalDrawColor = getTileTypeColor(initialSpecialType);
        } else {
            // Unowned normal tiles retain their initial pattern/texture
            finalDrawColor = getTileTypeColor(initialSpecialType); // This gets white/black
        }
    }

    // Apply the determined color/tint
    if (has_texture && !tileSprites.empty()) {
        tileSprites.front().setColor(finalDrawColor);
        window.draw(tileSprites.front());
    } else {
        tileShape.setFillColor(finalDrawColor);
        window.draw(tileShape);
    }
}

void Tile::claimTile(int playerID, sf::Color pColor) {
    ownerPlayerID = playerID;
    ownerColor = pColor;
}

void Tile::setColor(const sf::Color& color) {
    if (has_texture && !tileSprites.empty()) {
        tileSprites.front().setColor(color);
    } else {
        tileShape.setFillColor(color);
    }
}

sf::Vector2f Tile::getPosition() const { return coordinates; }
float Tile::getSize() { return TileSize; }

sf::FloatRect Tile::getBounds() const {
    if (has_texture && !tileSprites.empty()) {
        return tileSprites.front().getGlobalBounds();
    }
    return tileShape.getGlobalBounds();
}

int Tile::getOwner() const { return ownerPlayerID; }
TileSpecialType Tile::getSpecialType() const { return initialSpecialType; }

void Tile::resetTileOwnership() {
    ownerPlayerID = -1;
    ownerColor = has_texture ? sf::Color::White : getTileTypeColor(initialSpecialType);
}

// --- Derived Tile Classes Implementations ---
StickyTile::StickyTile(float x, float y) : Tile(x, y, STICKY) {}
StickyTile::StickyTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, STICKY) {}

DamageTile::DamageTile(float x, float y) : Tile(x, y, DAMAGE) {}
DamageTile::DamageTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, DAMAGE) {}

HealingTile::HealingTile(float x, float y) : Tile(x, y, HEALING) {}
HealingTile::HealingTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, HEALING) {}

TeleporterTile::TeleporterTile(float x, float y) : Tile(x, y, TELEPORTER) {}
TeleporterTile::TeleporterTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, TELEPORTER) {}