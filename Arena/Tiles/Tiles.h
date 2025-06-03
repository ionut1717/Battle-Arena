#ifndef TILES_H
#define TILES_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>

enum class TileSpecialType {
    NONE = 0,
    STICKY,
    DAMAGE,
    HEALING,
    TELEPORTER,
    SUPER
};

class Tile {
private:
    static float TileSize;
    sf::RectangleShape tileShape;
    sf::Vector2f coordinates;
    std::vector<sf::Sprite> tileSprites;
    bool has_texture;
    int ownerPlayerID = -1;
    sf::Color ownerColor;
protected:
    TileSpecialType initialSpecialType;
public:
    virtual ~Tile();
    sf::Color getTileTypeColor(TileSpecialType type) const;

    Tile(const float x, const float y, TileSpecialType type = TileSpecialType::NONE);

    Tile(sf::Texture const& texture, const float x, const float y, TileSpecialType type = TileSpecialType::NONE);

    Tile(const Tile& other);

    virtual void draw(sf::RenderWindow& window, int viewingPlayerID);
    void claimTile(int playerID, sf::Color pColor);

    sf::Vector2f getPosition() const { return coordinates; }
    static float getSize() { return TileSize; }
    sf::FloatRect getBounds() const{return tileShape.getGlobalBounds();};
    int getOwner() const { return ownerPlayerID; }
    TileSpecialType getSpecialType() const { return initialSpecialType; }
};



#endif // TILES_H