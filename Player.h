#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <SFML/System.hpp>
#include <iostream> // Pentru std::cout
#include "Arena.h" // Arena are nevoie de Tiles.h, deci Player poate include Arena

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
    sf::IntRect getFrameRect(int row, int col) {
        return sf::IntRect({col * FRAME_WIDTH, row * FRAME_HEIGHT}, {FRAME_WIDTH, FRAME_HEIGHT});
    }

    // Metodă helper pentru a popula cadrele de animație
    void setupAnimationFrames() {
        // Jos (Down): Rândul 1, coloanele 1-4.
        animationFrames[AnimDown] = {
            getFrameRect(0, 0), getFrameRect(0, 1), getFrameRect(0, 2), getFrameRect(0, 3)
        };
        // Jos-Dreapta (Down-Right): Rândul 1, coloanele 5-8.
        animationFrames[AnimDownRight] = {
            getFrameRect(0, 4), getFrameRect(0, 5), getFrameRect(0, 6), getFrameRect(0, 7)
        };
        // Dreapta (Right): Rândul 1, coloana 9 + Rândul 2, coloanele 1-3.
        animationFrames[AnimRight] = {
            getFrameRect(0, 8), getFrameRect(1, 0), getFrameRect(1, 1), getFrameRect(1, 2)
        };
        // Sus (Up): Rândul 2, coloanele 4-7.
        animationFrames[AnimUp] = {
            getFrameRect(1, 3), getFrameRect(1, 4), getFrameRect(1, 5), getFrameRect(1, 6)
        };
        // Sus-Dreapta (Up-Right): Rândul 2, coloanele 8-9 + Rândul 3, coloanele 1-2.
        animationFrames[AnimUpRight] = {
            getFrameRect(1, 7), getFrameRect(1, 8), getFrameRect(2, 0), getFrameRect(2, 1)
        };

        // Pentru direcțiile spre stânga, vom folosi cadrele corespunzătoare și vom oglindi sprite-ul.
        // Acestea sunt doar placeholdere; logica de oglindire va fi în draw/updateAnimation.
        animationFrames[AnimDownLeft] = animationFrames[AnimDownRight];
        animationFrames[AnimLeft] = animationFrames[AnimRight];
        animationFrames[AnimUpLeft] = animationFrames[AnimUpRight];
    }

    // Metodă pentru a determina direcția de animație pe baza vectorului de viteză
    void determineAnimationDirection(sf::Vector2f currentVelocity) {
        if (currentVelocity.x == 0 && currentVelocity.y == 0) {
            isMoving = false;
            return;
        }
        isMoving = true;

        // Modificat: Am schimbat velocity în currentVelocity pentru a folosi parametrul funcției
        if (currentVelocity.x > 0 && currentVelocity.y == 0) { // 0 grade: Right
            currentAnimationDirection = AnimRight;
            facingLeft = false;
        } else if (currentVelocity.x > 0 && currentVelocity.y < 0) {
            currentAnimationDirection = AnimUpRight;
            facingLeft = false;
        } else if (currentVelocity.x == 0 && currentVelocity.y < 0) { // 90 grade: Up
            currentAnimationDirection = AnimUp;
            facingLeft = false;
        } else if (currentVelocity.x < 0 && currentVelocity.y < 0 ) { // 135 grade: Up-Left
            currentAnimationDirection = AnimUpLeft;
            facingLeft = true;
        } else if (currentVelocity.x < 0 && currentVelocity.y == 0) { // 180 grade: Left
            currentAnimationDirection = AnimLeft;
            facingLeft = true;
        } else if (currentVelocity.x < 0  && currentVelocity.y > 0) { // 225 grade: Down-Left
            currentAnimationDirection = AnimDownLeft;
            facingLeft = true;
        } else if (currentVelocity.x == 0 && currentVelocity.y > 0) { // 270 grade: Down
            currentAnimationDirection = AnimDown;
            facingLeft = false;
        } else if (currentVelocity.x >= 0 && currentVelocity.y > 0) { // 315 grade: Down-Right
            currentAnimationDirection = AnimDownRight;
            facingLeft = false;
        }
    }

    void updateAnimation() {
        if (!isMoving) {
            // Dacă nu se mișcă, afișează primul cadru al direcției curente (sau down)
            if (!animationFrames[currentAnimationDirection].empty()) {
                playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
            }
            currentFrame = 0; // Resetăm cadrul la oprire
            animationClock.restart(); // Resetăm clock-ul pentru a porni animația de la început când începe mișcarea
        } else { // Doar dacă se mișcă, avansăm animația
            if (animationClock.getElapsedTime().asSeconds() >= animationFrameTime) {
                currentFrame = (currentFrame + 1) % animationFrames[currentAnimationDirection].size();
                playerSprite.setTextureRect(animationFrames[currentAnimationDirection][currentFrame]);
                animationClock.restart();
            }
        }


        // Aplicăm scalarea și oglindirea dacă este necesar
        if (facingLeft) {
            playerSprite.setScale({-currentScaleFactor, currentScaleFactor}); // Folosește currentScaleFactor
            playerSprite.setOrigin({FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f});
        } else {
            playerSprite.setScale({currentScaleFactor, currentScaleFactor}); // Folosește currentScaleFactor
            playerSprite.setOrigin({FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f});
        }
    }


public:
    int GetPlayerID() { return playerID; }
    void GetDamageBalloon() {
        if (PlayerHealth <= 10) {
            isEliminated = true;
            PlayerHealth = 0;
            std::cout << "Player " << playerID << " a fost eliminat de un balon!" << std::endl;
        } else {
            PlayerHealth -= 10;
        }
    }

    // Constructorul modificat
    Player(const int playerID, const sf::Color color, const sf::Vector2f startPosition, const bool wasd,sf::Texture PlayerTexture)
        : playerID(playerID), color(color), coordinates(startPosition), wasd(wasd), playerSprite(PlayerTexture) {

        // --- Inițializează currentScaleFactor aici, înainte de a seta scala pe sprite ---
        currentScaleFactor = 0.5f; // Setează factorul de scalare dorit (0.5 = jumătate din dimensiune)


        // Încărcarea texturii specifice fiecărui jucător
        std::string texturePath;
        if (playerID == 1) {
            texturePath = "../hero.png";
        } else {
            texturePath = "../base_character.png";
        }

        if (!playerTexture.loadFromFile(texturePath)) {
            std::cerr << "Eroare: Nu s-a putut incarca textura jucatorului " << playerID << " din " << texturePath <<
                    std::endl;
        }
        playerSprite.setTexture(playerTexture);
        playerSprite.setOrigin({FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f}); // Setează originea în centru
        playerSprite.setPosition(coordinates); // Poziția inițială a sprite-ului

        // --- Modificare AICI: Aplică scala folosind currentScaleFactor ---
        playerSprite.setScale({currentScaleFactor, currentScaleFactor});
        playerRadius = (FRAME_WIDTH * currentScaleFactor) / 2.0f; // Ajustează raza pentru coliziune

        setupAnimationFrames(); // Populează mapa cu cadre de animație
        // Setează cadrul inițial al sprite-ului
        if (!animationFrames[currentAnimationDirection].empty()) {
            playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
        }


        if (wasd) {
            keysPressed[sf::Keyboard::Key::A] = false;
            keysPressed[sf::Keyboard::Key::S] = false;
            keysPressed[sf::Keyboard::Key::D] = false;
            keysPressed[sf::Keyboard::Key::W] = false;
        } else {
            keysPressed[sf::Keyboard::Key::Left] = false;
            keysPressed[sf::Keyboard::Key::Right] = false;
            keysPressed[sf::Keyboard::Key::Up] = false;
            keysPressed[sf::Keyboard::Key::Down] = false;
        }
        velocity = {0.0f, 0.0f};
    }

    void set_key_pressed(const sf::Keyboard::Key key, const bool pressed) {
        keysPressed[key] = pressed;
    }

    float getActualPlayerSpeed() const {
        return isStuck ? PlayerSpeed * stuckSpeedFactor : PlayerSpeed;
    }

    void update_position(float deltaTime) {
        sf::Vector2f currentInputDirection = {0.0f, 0.0f};

        if (wasd) {
            if (keysPressed[sf::Keyboard::Key::A]) currentInputDirection.x -= 1.0f;
            if (keysPressed[sf::Keyboard::Key::D]) currentInputDirection.x += 1.0f;
            if (keysPressed[sf::Keyboard::Key::W]) currentInputDirection.y -= 1.0f;
            if (keysPressed[sf::Keyboard::Key::S]) currentInputDirection.y += 1.0f;
        } else {
            if (keysPressed[sf::Keyboard::Key::Left]) currentInputDirection.x -= 1.0f;
            if (keysPressed[sf::Keyboard::Key::Right]) currentInputDirection.x += 1.0f;
            if (keysPressed[sf::Keyboard::Key::Up]) currentInputDirection.y -= 1.0f;
            if (keysPressed[sf::Keyboard::Key::Down]) currentInputDirection.y += 1.0f;
        }
        // Normalise movement
        float length = std::sqrt(currentInputDirection.x * currentInputDirection.x + currentInputDirection.y * currentInputDirection.y);
        if (length > 0.0f) {
            velocity = (currentInputDirection / length) * getActualPlayerSpeed();
            determineAnimationDirection(velocity); // Determină direcția animației
        } else {
            velocity = {0.0f, 0.0f};
            isMoving = false; // Nu se mișcă, oprește animația
        }

        coordinates += velocity * deltaTime;
        playerSprite.setPosition(coordinates); // Actualizează poziția sprite-ului

        int grid_size = Arena::GetGridSize();
        float tile_size = Tile::getSize();

        // --- MODIFICARE AICI: Ajustarea coliziunii cu bordurile bazată pe dimensiunea sprite-ului scalat ---
        // Folosim playerSprite.getGlobalBounds() pentru a obține dimensiunile reale ale sprite-ului pe ecran.
        sf::FloatRect bounds = playerSprite.getGlobalBounds();
        float halfWidth = bounds.size.x / 2.0f;
        float halfHeight = bounds.size.y / 2.0f;

        // Limita stânga
        if (coordinates.x < halfWidth) {
            coordinates.x = halfWidth;
        }
        // Limita dreapta
        else if (coordinates.x > static_cast<float>(grid_size) * tile_size - halfWidth) {
            coordinates.x = static_cast<float>(grid_size) * tile_size - halfWidth;
        }

        // Limita sus
        if (coordinates.y < halfHeight) {
            coordinates.y = halfHeight;
        }
        // Limita jos
        else if (coordinates.y > static_cast<float>(grid_size) * tile_size - halfHeight) {
            coordinates.y = static_cast<float>(grid_size) * tile_size - halfHeight;
        }
        // --- SFÂRȘIT MODIFICARE ---


        updateAnimation(); // Actualizează animația
    }

    void draw(sf::RenderWindow& window) {
        if (!isEliminated) {
            window.draw(playerSprite); // Desenează sprite-ul în loc de cerc
        }
    }

    sf::FloatRect getBounds() const {
        // Acum returnăm limitele sprite-ului pentru o coliziune mai precisă
        return playerSprite.getGlobalBounds();
    }

    sf::Vector2f getPosition() const { return coordinates; }
    unsigned int getHealth() const { return PlayerHealth; }
    bool isEliminatedPlayer() const { return isEliminated; }
    // Radiusul este mai puțin relevant acum, dar îl păstrăm pentru consistență sau alte coliziuni simple.
    float getRadius() const { return playerRadius; }
    sf::Vector2f getVelocity() const { return velocity; }

    void paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, const float radius) {
        float tileSize = Tile::getSize();
        sf::Vector2f playerCenter = coordinates;
        // Asigură că grid-ul nu e gol înainte de a accesa grid[0]
        int gridWidth = grid.empty() ? 0 : grid[0].size();
        int gridHeight = grid.size();

        float worldRadius = radius * tileSize;

        for (int i = 0; i < gridHeight; ++i) {
            for (int j = 0; j < gridWidth; ++j) {
                // Accesează Tile-ul prin dereferențierea unique_ptr
                Tile& tile = *grid[i][j];
                sf::Vector2f tile_position = tile.getPosition();
                sf::Vector2f tileCenter = tile_position + sf::Vector2f(tileSize / 2.0f, tileSize / 2.0f);

                float dx = tileCenter.x - playerCenter.x;
                float dy = tileCenter.y - playerCenter.y;
                if (dx * dx + dy * dy <= worldRadius * worldRadius) {
                    // MODIFICARE AICI: Apelez noua metodă claimTile din Tile.h
                    tile.claimTile(playerID, color);
                }
            }
        }
    }

    Player* getPlayer() {
        return this;
    }

    void setStuck(bool stuck) {
        isStuck = stuck;
    }
    bool getIsStuck() const {
        return isStuck;
    }

    void setPosition(sf::Vector2f newPos) {
        coordinates = newPos;
        playerSprite.setPosition(coordinates); // Actualizează și sprite-ul
        setOnDamageTile(false);
        setStuck(false);
        tookInitialDamage = false;
        lastGridPosition = {-1, -1};
    }

    void setOnDamageTile(bool onTile) {
        isOnDamageTile = onTile;
        if (!onTile) {
            tookInitialDamage = false;
        }
    }
    bool getIsOnDamageTile() const {
        return isOnDamageTile;
    }

    void takeInitialDamage() {
        if (!tookInitialDamage && PlayerHealth > 0) {
            PlayerHealth -= damageAmountPerTick;
            tookInitialDamage = true;
            std::cout << "Player " << playerID << " a luat " << damageAmountPerTick << " damage INITIAL de la tile! Viata: " << PlayerHealth << std::endl;
            if (PlayerHealth <= 0) {
                isEliminated = true;
                PlayerHealth = 0;
                std::cout << "Player " << playerID << " a fost eliminat de un tile de damage!" << std::endl;
            }
        }
    }

    void takeContinuousDamage() {
        if (PlayerHealth > 0) {
            PlayerHealth -= damageAmountPerTick;
            std::cout << "Player " << playerID << " a luat " << damageAmountPerTick << " damage CONTINUU de la tile! Viata: " << PlayerHealth << std::endl;
            if (PlayerHealth <= 0) {
                isEliminated = true;
                PlayerHealth = 0;
                std::cout << "Player " << playerID << " a fost eliminat de un tile de damage!" << std::endl;
            }
        }
    }

    float getDamageTickRate() const {
        return damageTickRate;
    }
    sf::Time getDamageElapsedTime() const {
        return damageTickClock.getElapsedTime();
    }
    void restartDamageTickClock() {
        damageTickClock.restart();
    }

    sf::Vector2i getLastGridPosition() const {
        return lastGridPosition;
    }
    void setLastGridPosition(sf::Vector2i pos) {
        lastGridPosition = pos;
    }
    void resetHealthAndState() {
        PlayerHealth = 100;
        isEliminated = false;
        isStuck = false;
        isOnDamageTile = false;
        tookInitialDamage = false;

        lastGridPosition = {-1, -1};
        velocity = {0.0f, 0.0f}; // Resetează și viteza
        currentFrame = 0; // Resetează cadrul animației
        currentAnimationDirection = AnimDown; // Resetează direcția animației
        isMoving = false;
        facingLeft = false;
        playerSprite.setScale({currentScaleFactor, currentScaleFactor}); // Folosește factorul de scalare la reset
        // Nu resetăm setPosition aici, deoarece este apelată separat în initGame
    }
};

#endif //PLAYER_H