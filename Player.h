#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <map>
#include <SFML/System.hpp>
#include "Arena/Arena.h"
#include <memory>
#include "Arena/Tiles/Tiles.h"

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
    float stuckSpeedFactor;
    bool isOnDamageTile = false;
    sf::Clock damageTickClock;
    const float damageTickRate = 1.0f;
    const int damageAmountPerTick = 1;
    bool tookInitialDamage = false;

    sf::Vector2i lastGridPosition = {-1, -1};

    // MODIFIED: Changed from reference to pointer
    sf::Texture* playerTexture;
    sf::Sprite playerSprite;
    sf::Clock animationClock;
    float animationFrameTime = 1.0f /60.0f;
    int currentFrame = 0;
    std::map<AnimationDirection, std::vector<sf::IntRect>> animationFrames;
    AnimationDirection currentAnimationDirection = AnimDown;
    bool isMoving = false;
    bool facingLeft = false;

    static constexpr int m_frameSize = 128;
    float m_tileSize;

    float m_characterVisualSize;

    sf::Clock paintCooldownClock;
    const float paintCooldownTime = 3.0f;

    bool m_inUse = false;

    sf::IntRect getFrameRect(int row, int col);
    void setupAnimationFrames();
    void determineAnimationDirection(sf::Vector2f currentVelocity);
    void updateAnimation();
    void resetForReuse();

public:
    // MODIFIED: Constructor now takes a pointer
    Player(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture* playerTexturePtr);

    int getPlayerID() const;
    void getDamageBalloon();
    void getDamageTile(float value);
    void setKeyPressed(sf::Keyboard::Key key, bool pressed);
    float getActualPlayerSpeed() const;
    void updatePosition(float deltaTime);
    void draw(sf::RenderWindow& window) const;
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

    void paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, float radius_factor);

    void heal(float value);
    void setStuck(bool stuck,float value);
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

    static const int MAX_PLAYERS = 2;
    static std::vector<std::unique_ptr<Player>> s_playerPool;
    static std::vector<Player*> s_activePlayers;
    // MODIFIED: acquirePlayer now takes a pointer
    static Player* acquirePlayer(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture* playerTexturePtr);
    static void releasePlayer(Player* player);
};

#endif // PLAYER_H