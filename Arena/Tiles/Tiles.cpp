#include "Tiles.h"

///consts
float Tile::TileSize = 20.0f;



Tile::~Tile() = default;

sf::Color Tile::getTileTypeColor(TileSpecialType type) const {
    switch (type) {
        case TileSpecialType::NONE: return sf::Color::White;
        case TileSpecialType::STICKY: return sf::Color::Yellow;
        case TileSpecialType::DAMAGE: return sf::Color::Red;
        case TileSpecialType::HEALING: return sf::Color::Green;
        case TileSpecialType::TELEPORTER: return sf::Color::Magenta;
        case TileSpecialType::SUPER: return sf::Color(128, 0, 128); // Un fel de mov închis/purpuriu pentru SuperTile
        default: throw std::invalid_argument("Invalid special type");
    }
}
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
    tileShape.setFillColor(sf::Color::White);

    sf::Sprite sprite(texture);
    float scaleX = TileSize / static_cast<float>(texture.getSize().x);
    float scaleY = TileSize / static_cast<float>(texture.getSize().y);
    sprite.setScale({scaleX, scaleY});
    sprite.setPosition(coordinates);
    tileSprites.push_back(sprite);
}

// Copy constructor implementation
Tile::Tile(const Tile& other)
    : tileShape(other.tileShape),
      coordinates(other.coordinates),
      tileSprites(other.tileSprites),
      has_texture(other.has_texture),
      ownerPlayerID(other.ownerPlayerID),
      ownerColor(other.ownerColor),
      initialSpecialType(other.initialSpecialType)
{
        if (has_texture && !tileSprites.empty() && other.tileSprites.empty() == false) {
            tileSprites.front().setTexture(other.tileSprites.front().getTexture());
        }
}



void Tile::draw(sf::RenderWindow& window, int viewingPlayerID) {
    sf::Color finalDrawColor;

    if (ownerPlayerID != -1) { 
        if (viewingPlayerID == ownerPlayerID) {
            if (initialSpecialType != TileSpecialType::NONE) {
                finalDrawColor = getTileTypeColor(initialSpecialType);
            } else {
                finalDrawColor = ownerColor;
            }
        } else {
            finalDrawColor = ownerColor;
        }
    } else {
            finalDrawColor = getTileTypeColor(initialSpecialType);
    }

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
