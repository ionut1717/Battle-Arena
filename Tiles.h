#ifndef TILES_H
#define TILES_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream> // For std::cerr if needed in constructors, though usually moved to .cpp

// Enum pentru tipurile speciale de tile-uri
enum TileSpecialType {
    NONE = 0,         // Tile normal (alb/negru)
    STICKY,           // Galben
    DAMAGE,           // Roșu
    HEALING,          // Verde
    TELEPORTER        // Mov
};

class Tile {
protected:
    static constexpr float TileSize = 20.0f; // Keep static constexpr in header
    sf::RectangleShape tileShape;
    sf::Vector2f coordinates;
    std::vector<sf::Sprite> tileSprites; // Used for actual textures if loaded
    bool has_texture;

    int ownerPlayerID = -1; // ID-ul jucătorului care a colorat tile-ul (-1 dacă nu e colorat)
    sf::Color ownerColor;   // Culoarea cu care a fost colorat de un jucător
    TileSpecialType initialSpecialType; // Tipul special original al tile-ului

public:
    // Virtual destructor
    virtual ~Tile();

    // Metodă helper pentru a obține culoarea asociată cu un TileSpecialType (can be inline)
    sf::Color getTileTypeColor(TileSpecialType type) const {
        switch (type) {
            case NONE: return sf::Color::White;
            case STICKY: return sf::Color::Yellow;
            case DAMAGE: return sf::Color::Red;
            case HEALING: return sf::Color::Green;
            case TELEPORTER: return sf::Color::Magenta;
            default: return sf::Color::White;
        }
    }

    // Constructors
    Tile(const float x, const float y, TileSpecialType type = NONE);
    Tile(sf::Texture const& texture, const float x, const float y, TileSpecialType type = NONE);

    // Copy constructor
    Tile(const Tile& other);

    // Member functions
    void setTexture(const sf::Texture& texture);
    virtual void draw(sf::RenderWindow& window, int viewingPlayerID);
    void claimTile(int playerID, sf::Color pColor);
    void setColor(const sf::Color& color); // This method is less relevant now, but kept for consistency

    // Getters
    sf::Vector2f getPosition() const;
    static float getSize(); // Static getter for TileSize
    sf::FloatRect getBounds() const;
    int getOwner() const;
    TileSpecialType getSpecialType() const;

    void resetTileOwnership();
};

// --- Derived Tile Classes ---
class StickyTile : public Tile {
public:
    StickyTile(float x, float y);
    StickyTile(sf::Texture const& texture, float x, float y);
};

class DamageTile : public Tile {
public:
    DamageTile(float x, float y);
    DamageTile(sf::Texture const& texture, float x, float y);
};

class HealingTile : public Tile {
public:
    HealingTile(float x, float y);
    HealingTile(sf::Texture const& texture, float x, float y);
};

class TeleporterTile : public Tile {
public:
    TeleporterTile(float x, float y);
    TeleporterTile(sf::Texture const& texture, float x, float y);
};

#endif //TILES_H