#ifndef TILES_H
#define TILES_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>

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
    static constexpr float TileSize = 20.0f;
    sf::RectangleShape tileShape;
    sf::Vector2f coordinates;
    std::vector<sf::Sprite> tileSprites; // Used for actual textures if loaded
    bool has_texture;

    int ownerPlayerID = -1; // ID-ul jucătorului care a colorat tile-ul (-1 dacă nu e colorat)
    sf::Color ownerColor;   // Culoarea cu care a fost colorat de un jucător
    TileSpecialType initialSpecialType; // Tipul special original al tile-ului

public:
    virtual ~Tile() = default;

    // Metodă helper pentru a obține culoarea asociată cu un TileSpecialType
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

    // Constructor pentru non-textured tiles
    Tile(const float x, const float y, TileSpecialType type = NONE)
        : coordinates(x, y), has_texture(false), initialSpecialType(type) {
        tileShape.setPosition(coordinates);
        tileShape.setSize({TileSize, TileSize});
        tileShape.setOutlineColor(sf::Color::Black);
        tileShape.setOutlineThickness(1.f);
        tileShape.setFillColor(getTileTypeColor(initialSpecialType)); // Setează direct culoarea
        ownerColor = getTileTypeColor(initialSpecialType); // Inițial, ownerColor e culoarea inițială a tipului
    }

    // Constructor pentru textured tiles
    Tile(sf::Texture const& texture, const float x, const float y, TileSpecialType type = NONE)
        : coordinates(x, y), has_texture(true), initialSpecialType(type) {
        tileShape.setPosition(coordinates);
        tileShape.setSize({TileSize, TileSize});
        tileShape.setOutlineColor(sf::Color::Black);
        tileShape.setOutlineThickness(1.f);
        tileShape.setFillColor(sf::Color::White); // Culoarea de bază pentru a desena conturul/umbra

        sf::Sprite sprite(texture);
        float scaleX = TileSize / static_cast<float>(texture.getSize().x);
        float scaleY = TileSize / static_cast<float>(texture.getSize().y);
        sprite.setScale({scaleX, scaleY});
        sprite.setPosition(coordinates);
        tileSprites.push_back(sprite);

        ownerColor = sf::Color::White; // OwnerColor inițial pentru tile-uri texturate (va fi setată la ownerColor când e colorat)
    }

    Tile(const Tile& other)
        : tileShape(other.tileShape),
          coordinates(other.coordinates),
          tileSprites(other.tileSprites),
          has_texture(other.has_texture),
          ownerPlayerID(other.ownerPlayerID),
          ownerColor(other.ownerColor),
          initialSpecialType(other.initialSpecialType) {}


    void setTexture(const sf::Texture& texture) {
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

    virtual void draw(sf::RenderWindow& window, int viewingPlayerID) {
        // Obținem culoarea inițială a tipului de tile
        sf::Color initialTypeColor = getTileTypeColor(initialSpecialType);

        // Culoarea de desenare finală sau tint pentru sprite
        sf::Color drawColor = sf::Color::White; // Implicit alb pentru texturi sau pentru tile-uri normale fără textură

        // Logica principală de desenare
        if (ownerPlayerID != -1) { // Dacă tile-ul este deținut de oricine (incl. viewingPlayerID)
            if (viewingPlayerID == ownerPlayerID) { // Jucătorul care privește este proprietarul
                if (initialSpecialType != NONE) {
                    // Dacă tile-ul este special, proprietarul vede aspectul special (textură cu tint special)
                    drawColor = initialTypeColor;
                    if (has_texture) {
                        drawColor.a = 128; // Aplica semi-transparență pentru a vedea textura
                    }
                } else {
                    // Dacă tile-ul NU este special, proprietarul vede culoarea lui de proprietate
                    drawColor = ownerColor;
                }
            } else { // Jucătorul care privește NU este proprietarul
                // Alți jucători văd întotdeauna culoarea proprietarului
                drawColor = ownerColor;
            }
        } else { // Tile-ul nu este deținut de nimeni
            // Toți jucătorii văd aspectul inițial, non-colorat
            drawColor = initialTypeColor;
            if (initialSpecialType != NONE && has_texture) {
                drawColor.a = 128; // Aplica semi-transparență pentru tile-urile speciale texturate
            }
        }

        // Aplicați culoarea determinată la desenare
        if (has_texture && !tileSprites.empty()) {
            tileSprites.front().setColor(drawColor);
            window.draw(tileSprites.front());
        } else {
            tileShape.setFillColor(drawColor);
            window.draw(tileShape);
        }
    }

    void claimTile(int playerID, sf::Color pColor) {
        ownerPlayerID = playerID;
        ownerColor = pColor;
    }

    void setColor(const sf::Color& color) {
        // Aceasta metodă este mai puțin relevantă acum, deoarece draw gestionează culorile
        // dar o păstrăm pentru consistență sau alte scopuri potențiale.
        if (has_texture && !tileSprites.empty()) {
            tileSprites.front().setColor(color);
        } else {
            tileShape.setFillColor(color);
        }
    }

    sf::Vector2f getPosition() const { return coordinates; }
    static float getSize() { return TileSize; }

    sf::FloatRect getBounds() const {
        if (has_texture && !tileSprites.empty()) {
            return tileSprites.front().getGlobalBounds();
        }
        return tileShape.getGlobalBounds();
    }

    int getOwner() const { return ownerPlayerID; }
    TileSpecialType getSpecialType() const { return initialSpecialType; }

    void resetTileOwnership() {
        ownerPlayerID = -1;
        // La resetare, ownerColor trebuie să fie culoarea inițială de tip, nu alb.
        ownerColor = getTileTypeColor(initialSpecialType);
    }
};

// --- Derived Tile Classes ---
class StickyTile : public Tile {
public:
    StickyTile(float x, float y) : Tile(x, y, STICKY) {}
    StickyTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, STICKY) {}
};

class DamageTile : public Tile {
public:
    DamageTile(float x, float y) : Tile(x, y, DAMAGE) {}
    DamageTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, DAMAGE) {}
};

class HealingTile : public Tile {
public:
    HealingTile(float x, float y) : Tile(x, y, HEALING) {}
    HealingTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, HEALING) {}
};

class TeleporterTile : public Tile {
public:
    TeleporterTile(float x, float y) : Tile(x, y, TELEPORTER) {}
    TeleporterTile(sf::Texture const& texture, float x, float y) : Tile(texture, x, y, TELEPORTER) {}
};

#endif //TILES_H