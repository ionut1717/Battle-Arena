#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <SFML/System.hpp>
#include <iostream> // For std::cout/cerr, can be moved if desired
#include "Arena.h" // Arena needs Tiles.h, so Player can include Arena

// Enum pentru a mapa mai ușor direcțiile de animație la un index
enum AnimationDirection {
    AnimDown = 0,
    AnimDownRight,
    AnimRight,
    AnimUpRight,
    AnimUp,
    AnimUpLeft,   // Acestea vor fi oglindite
    AnimLeft,     // Acestea vor fi oglindite
    AnimDownLeft  // Acestea vor fi oglindite
};

class Player {
private: // Moved private members here for clarity
    int playerID;
    unsigned int PlayerHealth = 100;
    sf::Color color; // Culoarea inițială, acum mai puțin relevantă pentru sprite
    sf::Vector2f coordinates;
    const float PlayerSpeed = 250.0f;
    sf::Vector2f velocity;
    std::map<sf::Keyboard::Key, bool> keysPressed;
    bool wasd;
    bool isEliminated = false;
    float playerRadius = 10.0f; // Radiusul e încă util pentru coliziuni, chiar și cu sprite
    bool isStuck = false;
    const float stuckSpeedFactor = 0.1f;
    bool isOnDamageTile = false;
    sf::Clock damageTickClock;
    const float damageTickRate = 1.0f;
    const int damageAmountPerTick = 1;
    bool tookInitialDamage = false;

    sf::Vector2i lastGridPosition = {-1, -1};

    // --- Noi membri pentru animație ---
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    sf::Clock animationClock;
    float animationFrameTime = 1.0f / 10.0f; // 10 FPS
    int currentFrame = 0;
    // Mapa pentru a stoca cadrele de animație pentru fiecare direcție
    std::map<AnimationDirection, std::vector<sf::IntRect>> animationFrames;
    AnimationDirection currentAnimationDirection = AnimDown; // Direcția implicită
    bool isMoving = false;
    bool facingLeft = false; // Pentru a ști dacă trebuie să oglindim sprite-ul

    // Dimensiunea unui singur cadru din sprite sheet
    static constexpr int FRAME_WIDTH = 128;
    static constexpr int FRAME_HEIGHT = 128;

    // Adaugă o variabilă membru pentru factorul de scalare
    float currentScaleFactor; // Va stoca factorul de scalare aplicat


    // Metodă helper pentru a calcula sf::IntRect
    sf::IntRect getFrameRect(int row, int col);

    // Metodă helper pentru a popula cadrele de animație
    void setupAnimationFrames();

    // Metodă pentru a determina direcția de animație pe baza vectorului de viteză
    void determineAnimationDirection(sf::Vector2f currentVelocity);

    void updateAnimation();

public:
    // Constructor
    Player(const int playerID, const sf::Color color, const sf::Vector2f startPosition, const bool wasd, sf::Texture PlayerTexture);

    // Destructor (if needed, otherwise default is fine)
    ~Player() = default;

    // Core Gameplay methods
    int GetPlayerID();
    void GetDamageBalloon();
    void set_key_pressed(const sf::Keyboard::Key key, const bool pressed);
    float getActualPlayerSpeed() const;
    void update_position(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    unsigned int getHealth() const;
    bool isEliminatedPlayer() const;
    float getRadius() const;
    sf::Vector2f getVelocity() const;

    // Tile Interaction methods
    void paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, const float radius);
    Player* getPlayer(); // Consider if this is truly necessary, often 'this' pointer is enough

    // Status/State methods
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
    sf::Color getColor();
};

#endif //PLAYER_H