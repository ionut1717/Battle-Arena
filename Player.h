#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <SFML/System.hpp>
#include <iostream>
#include "Arena/Arena.h" // Necesar pentru Arena::GetGridSize() și Tile::getSize()

// Enum pentru a mapa mai ușor direcțiile de animație la un index
enum AnimationDirection {
    AnimDown = 0,
    AnimDownRight,
    AnimRight,
    AnimUpRight,
    AnimUp,
    AnimUpLeft,
    AnimLeft,
    AnimDownLeft,
    AnimCount
};

class Player {
private:
    int playerID;
    int playerHealth = 100;
    sf::Color color;
    sf::Vector2f coordinates;
    const float playerSpeed = 250.0f;
    sf::Vector2f velocity;
    std::map<sf::Keyboard::Key, bool> keysPressed;
    bool wasd;
    bool isEliminated = false;
    float playerRadius;
    bool isStuck = false;
    const float stuckSpeedFactor = 0.2f;
    bool isOnDamageTile = false;
    sf::Clock damageTickClock;
    const float damageTickRate = 1.0f;
    const int damageAmountPerTick = 1;
    bool tookInitialDamage = false;

    sf::Vector2i lastGridPosition = {-1, -1};

    sf::Texture& playerTexture; // Referință la textură
    sf::Sprite playerSprite;
    sf::Clock animationClock;
    float animationFrameTime = 1.0f / 10.0f; // 10 frame-uri pe secundă
    int currentFrame = 0;
    std::map<AnimationDirection, std::vector<sf::IntRect>> animationFrames;
    AnimationDirection currentAnimationDirection = AnimDown;
    bool isMoving = false;
    bool facingLeft = false;

    // Dimensiunea unui frame în fișierul textură (celula spritesheet-ului)
    static constexpr int m_frameSize = 128;
    // Dimensiunea țintă a playerului pe ecran (m_tileSize va fi 4 * Tile::getSize() = 200)
    float m_tileSize; // Inițializat în constructor

    // NOU: Dimensiunea vizuală efectivă a caracterului pe ecran (fără spațiul transparent)
    float m_characterVisualSize; // Ex: va fi 100.0f dacă sprite-ul e de 2 ori mai mic decât hitbox-ul de 200

    sf::Clock paintCooldownClock;
    const float paintCooldownTime = 3.0f;

    sf::IntRect getFrameRect(int row, int col);
    void setupAnimationFrames();
    void determineAnimationDirection(sf::Vector2f currentVelocity);
    void updateAnimation();

public:
    Player(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture& playerTextureRef);

    int getPlayerID() const;
    void getDamageBalloon();
    void setKeyPressed(sf::Keyboard::Key key, bool pressed);
    float getActualPlayerSpeed() const;
    void updatePosition(float deltaTime);
    void draw(sf::RenderWindow& window) const; // Am asumat că ai adăugat const
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    unsigned int getHealth() const;
    bool isEliminatedPlayer() const;
    float getRadius() const;
    sf::Vector2f getVelocity() const;
    sf::Color getColor() const;

    sf::Vector2f getLaunchDirection() const;
    bool isFacingLeft() const;
    AnimationDirection getCurrentAnimationDirection() const;

    void paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, float radius);

    void heal(float value);
    void setStuck(bool stuck);
    bool getIsStuck() const;
    void setPosition(sf::Vector2f newPos);
    void setOnDamageTile(bool onTile);
    bool getIsOnDamageTile() const;
    void takeInitialDamage();
    void takeContinuousDamage();
    float getDamageTickRate() const;
    sf::Time getDamageElapsedTime() const;
    void restartDamageTickClock();
    sf::Vector2i getLastGridPosition() const;
    void setLastGridPosition(sf::Vector2i pos);
    void resetHealthAndState();
};

#endif // PLAYER_H