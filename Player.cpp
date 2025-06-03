#include "Player.h"

#include <algorithm>
#include <iostream>
#include "Arena/Tiles/Tiles.h" // Necesar pentru Tile::getSize()
#include "Resource_Manager.h"
#include <cmath>

#include "Balloon.h" // Necesar pentru AttackBalloon::getDamage()

// Helper method to calculate sf::IntRect
sf::IntRect Player::getFrameRect(int row, int col) {
    return sf::IntRect({static_cast<int>(col * m_frameSize), static_cast<int>(row * m_frameSize)}, {static_cast<int>(m_frameSize), static_cast<int>(m_frameSize)});
}

// Helper method to populate animation frames
void Player::setupAnimationFrames() {
    animationFrames[AnimDown]      = { getFrameRect(0,0), getFrameRect(0,1), getFrameRect(0,2), getFrameRect(0,3) };
    animationFrames[AnimDownRight] = { getFrameRect(0,4), getFrameRect(0,5), getFrameRect(0,6), getFrameRect(0,7) };
    animationFrames[AnimRight]     = { getFrameRect(0,8), getFrameRect(1,0), getFrameRect(1,1), getFrameRect(1,2) };
    animationFrames[AnimUpRight]   = { getFrameRect(1,7), getFrameRect(1,8), getFrameRect(2,0), getFrameRect(2,1) };
    animationFrames[AnimUp]        = { getFrameRect(1,3), getFrameRect(1,4), getFrameRect(1,5), getFrameRect(1,6) };

    // Using existing animations for mirrored directions
    animationFrames[AnimDownLeft]  = animationFrames[AnimDownRight];
    animationFrames[AnimLeft]      = animationFrames[AnimRight];
    animationFrames[AnimUpLeft]    = animationFrames[AnimUpRight];
}


void Player::determineAnimationDirection(sf::Vector2f currentVelocity) {
    if (currentVelocity.x == 0 && currentVelocity.y == 0) {
        isMoving = false;
        return;
    }
    isMoving = true;

    if (currentVelocity.x > 0 && currentVelocity.y == 0)      { currentAnimationDirection = AnimRight;     facingLeft = false; }
    else if (currentVelocity.x > 0 && currentVelocity.y < 0)  { currentAnimationDirection = AnimUpRight;   facingLeft = false; }
    else if (currentVelocity.x == 0 && currentVelocity.y < 0) { currentAnimationDirection = AnimUp;        facingLeft = false; }
    else if (currentVelocity.x < 0 && currentVelocity.y < 0 ) { currentAnimationDirection = AnimUpLeft;    facingLeft = true;  }
    else if (currentVelocity.x < 0 && currentVelocity.y == 0) { currentAnimationDirection = AnimLeft;      facingLeft = true;  }
    else if (currentVelocity.x < 0  && currentVelocity.y > 0){ currentAnimationDirection = AnimDownLeft;  facingLeft = true;  }
    else if (currentVelocity.x == 0 && currentVelocity.y > 0) { currentAnimationDirection = AnimDown;      facingLeft = false; }
    else if (currentVelocity.x > 0 && currentVelocity.y > 0)  { currentAnimationDirection = AnimDownRight; facingLeft = false; }
}


void Player::updateAnimation() {
    // Factorul de scalare pe baza dimensiunii țintă (m_tileSize) și a dimensiunii frame-ului (m_frameSize)
    float scaleFactor = m_tileSize / static_cast<float>(m_frameSize);

    // Setăm scara și originea. Originea trebuie să fie mereu la jumătatea dimensiunii _originale_ a frame-ului.
    // Flip-ul se realizează prin scalarea negativă pe X.
    if (facingLeft) {
        playerSprite.setOrigin({static_cast<float>(m_frameSize) / 2.0f, static_cast<float>(m_frameSize) / 2.0f});
        playerSprite.setScale({-scaleFactor, scaleFactor});
    } else {
        playerSprite.setOrigin({static_cast<float>(m_frameSize) / 2.0f, static_cast<float>(m_frameSize) / 2.0f});
        playerSprite.setScale({scaleFactor, scaleFactor});
    }


    if (!isMoving) {
        if (!animationFrames[currentAnimationDirection].empty()) {
            playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
        }
        currentFrame = 0; // Resetează la primul frame când nu se mișcă
        animationClock.restart();
    } else {
        if (animationClock.getElapsedTime().asSeconds() >= animationFrameTime) {
            if (!animationFrames[currentAnimationDirection].empty()) {
                 currentFrame = (currentFrame + 1) % animationFrames[currentAnimationDirection].size();
                 playerSprite.setTextureRect(animationFrames[currentAnimationDirection][currentFrame]);
            }
            animationClock.restart();
        }
    }
}



Player::Player(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture& playerTextureRef)
    : playerID(playerID), color(color), coordinates(startPosition), wasd(wasd),
      playerTexture(playerTextureRef),
      playerSprite(this->playerTexture)
{
    m_tileSize = 4 * Tile::getSize(); // Playerul vizual este de 200x200 (4 * 50)

    // Calculăm m_characterVisualSize pe baza observației că sprite-ul vizual este de 2 ori mai mic
    // decât dimensiunea la care este scalat m_tileSize (200x200), deci 100x100.
    m_characterVisualSize = m_tileSize / 2.0f; // 200 / 2 = 100

    setupAnimationFrames();

    if (!animationFrames[currentAnimationDirection].empty()) {
        playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
    }

    updateAnimation(); // Aplică scalarea și originea

    playerSprite.setPosition(coordinates);

    // Raza playerului se calculează direct din dimensiunea vizuală efectivă a caracterului
    playerRadius = m_characterVisualSize / 2.0f; // 100 / 2 = 50

    // Inițializare keysPressed
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

    // --- ADAUGĂ ACESTE LINII PENTRU DEBUGGING (OPȚIONAL) ---
    // Pot fi utile pentru a verifica valorile după modificări
    sf::FloatRect debug_localBounds = playerSprite.getLocalBounds();
    sf::FloatRect debug_globalBounds = playerSprite.getGlobalBounds();

    std::cout << "Player " << playerID << " Initialization Debugging:" << std::endl;
    std::cout << "  m_frameSize (from Player.h): " << m_frameSize << std::endl;
    std::cout << "  Tile::getSize(): " << Tile::getSize() << std::endl;
    std::cout << "  m_tileSize (target visual size for full sprite): " << m_tileSize << std::endl;
    std::cout << "  m_characterVisualSize (effective character size): " << m_characterVisualSize << std::endl;
    std::cout << "  playerSprite.getLocalBounds() (texture rect size): Width=" << debug_localBounds.size.x << ", Height=" << debug_localBounds.size.y << std::endl;
    std::cout << "  playerSprite.getGlobalBounds() (scaled sprite size with padding): Width=" << debug_globalBounds.size.x << ", Height=" << debug_globalBounds.size.y << std::endl;
    std::cout << "  Calculated playerRadius: " << playerRadius << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    // ---------------------------------------------
}

int Player::getPlayerID() const { return playerID; }

void Player::getDamageBalloon() {
    int damage = AttackBalloon::getDamage();
    if (playerHealth <= damage) {
        playerHealth = 0;
        isEliminated = true;
        std::cout << "Player " << playerID << " a fost eliminat de un balon!" << std::endl;
    } else {
        playerHealth -= damage;
    }
}

void Player::setKeyPressed(const sf::Keyboard::Key key, const bool pressed) {
    keysPressed[key] = pressed;
}

float Player::getActualPlayerSpeed() const {
    return isStuck ? playerSpeed * stuckSpeedFactor : playerSpeed;
}

void Player::updatePosition(float deltaTime) {
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

    float length = std::sqrt(currentInputDirection.x * currentInputDirection.x + currentInputDirection.y * currentInputDirection.y);
    if (length > 0.0f) {
        velocity = (currentInputDirection / length) * getActualPlayerSpeed();
    } else {
        velocity = {0.0f, 0.0f};
    }

    determineAnimationDirection(length > 0.0f ? velocity : currentInputDirection);


    coordinates += velocity * deltaTime;


    int gridSize = Arena::GetGridSize();
    // Dimensiunea efectiva a playerului pe ecran este m_characterVisualSize
    float halfEffectiveSize = m_characterVisualSize / 2.0f;

    // Ajustează limitele de coliziune cu marginile ecranului bazate pe m_characterVisualSize
    coordinates.x = std::clamp(coordinates.x, halfEffectiveSize, static_cast<float>(gridSize) * Tile::getSize() - halfEffectiveSize);
    coordinates.y = std::clamp(coordinates.y, halfEffectiveSize, static_cast<float>(gridSize) * Tile::getSize() - halfEffectiveSize);

    playerSprite.setPosition(coordinates);
    updateAnimation(); // Always call updateAnimation after setting position
}

void Player::draw(sf::RenderWindow& window) const { // Trebuie să fie const dacă e apelată pe referințe const
    if (!isEliminated) {
        window.draw(playerSprite);
    }
}

sf::FloatRect Player::getBounds() const {
    // NOU: Returnează un sf::FloatRect bazat pe dimensiunea vizuală efectivă a caracterului
    // Centrul sprite-ului este la `coordinates`.
    // Lățimea și înălțimea sunt `m_characterVisualSize`.
    return sf::FloatRect({coordinates.x - m_characterVisualSize / 2.0f,
                         coordinates.y - m_characterVisualSize / 2.0f},
                         {m_characterVisualSize, m_characterVisualSize});
}

sf::Vector2f Player::getPosition() const { return coordinates; }
unsigned int Player::getHealth() const { return playerHealth; }
bool Player::isEliminatedPlayer() const { return isEliminated; }
float Player::getRadius() const { return playerRadius; }
sf::Vector2f Player::getVelocity() const { return velocity; }
sf::Color Player::getColor() const { return color; }


sf::Vector2f Player::getLaunchDirection() const {
    sf::Vector2f direction = {0.0f, 0.0f};
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

    if (speed > 0.1f) { // If there's significant current movement, use that direction
        direction = velocity / speed;
        return direction;
    }

    // Otherwise, use the last animation direction for launching
    switch (currentAnimationDirection) {
        case AnimDown:      direction = {0.0f, 1.0f}; break;
        case AnimDownRight: direction = {1.0f, 1.0f}; break;
        case AnimRight:     direction = {1.0f, 0.0f}; break;
        case AnimUpRight:   direction = {1.0f, -1.0f}; break;
        case AnimUp:        direction = {0.0f, -1.0f}; break;
        case AnimUpLeft:    direction = {-1.0f, -1.0f}; break;
        case AnimLeft:      direction = {-1.0f, 0.0f}; break;
        case AnimDownLeft:  direction = {-1.0f, 1.0f}; break;
        case AnimCount:     // Fallthrough
        default:
            // Fallback for cases where animation direction might not be set or is default
            if (facingLeft) { direction = {-1.0f, 0.0f}; }
            else { direction = {1.0f, 0.0f}; } // Default to right

            // Further fallback if still (0,0) - e.g., if player was spawned without movement
            if (direction.x == 0.0f && direction.y == 0.0f) {
                 direction = {0.0f, 1.0f}; // Default to down
            }
            break;
    }

    // Normalize the direction vector
    float dirLength = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (dirLength > 0.001f) {
        direction /= dirLength;
    } else {
        direction = {0.0f, 1.0f}; // Fallback to down if still zero length
    }
    return direction;
}

bool Player::isFacingLeft() const {
    return facingLeft;
}

AnimationDirection Player::getCurrentAnimationDirection() const {
    return currentAnimationDirection;
}

void Player::paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, const float radius_factor) {
    if (paintCooldownClock.getElapsedTime().asSeconds() < paintCooldownTime) {
        std::cout << "Player " << playerID << " cooldown activ pentru colorare. Mai asteapta "
                  << paintCooldownTime - paintCooldownClock.getElapsedTime().asSeconds() << " secunde."
                  << std::endl;
        return;
    }

    // Folosim m_characterVisualSize pentru a calcula raza de vopsire
    sf::Vector2f playerCenter = coordinates;

    // Radius in world units (pixels)
    float worldRadius = radius_factor * m_characterVisualSize; // Acum se bazează pe m_characterVisualSize

    // Calculate the tile range to iterate over
    int minTileX = std::max(0, static_cast<int>((playerCenter.x - worldRadius) / Tile::getSize()));
    int maxTileX = std::min(static_cast<int>(grid[0].size() -1) , static_cast<int>((playerCenter.x + worldRadius) / Tile::getSize()));
    int minTileY = std::max(0, static_cast<int>((playerCenter.y - worldRadius) / Tile::getSize()));
    int maxTileY = std::min(static_cast<int>(grid.size() - 1), static_cast<int>((playerCenter.y + worldRadius) / Tile::getSize()));


    for (int y = minTileY; y <= maxTileY; ++y) {
        for (int x = minTileX; x <= maxTileX; ++x) {
            // Check if tile is within grid boundaries
            if (y >= 0 && static_cast<size_t>(y) < grid.size() && x >= 0 && static_cast<size_t>(x) < grid[y].size()) {
                Tile& tile = *grid[y][x];
                sf::Vector2f tileCenter = tile.getPosition() + sf::Vector2f(Tile::getSize() / 2.0f, Tile::getSize() / 2.0f);

                // Check distance from player center to tile center
                float dx = tileCenter.x - playerCenter.x;
                float dy = tileCenter.y - playerCenter.y;
                if (dx * dx + dy * dy <= worldRadius * worldRadius) {
                    tile.claimTile(playerID, color);
                }
            }
        }
    }
    paintCooldownClock.restart();
}

void Player::heal(float value) {
    playerHealth += static_cast<unsigned int>(value);
    if (playerHealth > 100) {
        playerHealth = 100;
    }
}

void Player::setStuck(bool stuck) { isStuck = stuck; }
bool Player::getIsStuck() const { return isStuck; }
void Player::setPosition(sf::Vector2f newPos) {
    coordinates = newPos;
    playerSprite.setPosition(coordinates);
    // Reset relevant states on reposition
    setOnDamageTile(false);
    setStuck(false);
    tookInitialDamage = false;
    lastGridPosition = {-1, -1}; // Consider resetting lastGridPosition
}
void Player::setOnDamageTile(bool onTile) {
    isOnDamageTile = onTile;
    if (!onTile) {
        tookInitialDamage = false;
    }
}
bool Player::getIsOnDamageTile() const { return isOnDamageTile; }

void Player::takeInitialDamage() {
    if (!tookInitialDamage && playerHealth > 0) {
        if (playerHealth <= static_cast<unsigned int>(damageAmountPerTick)) {
            playerHealth = 0;
            isEliminated = true;
        } else {
            playerHealth -= damageAmountPerTick;
        }
        tookInitialDamage = true;
        std::cout << "Player " << playerID << " took " << damageAmountPerTick << " INITIAL damage from tile! Health: " << playerHealth << std::endl;
        if (isEliminated) {
            std::cout << "Player " << playerID << " was eliminated by a damage tile!" << std::endl;
        }
    }
}
void Player::takeContinuousDamage() {
    if (playerHealth > 0) {
         if (playerHealth <= static_cast<unsigned int>(damageAmountPerTick)) {
            playerHealth = 0;
            isEliminated = true;
        } else {
            playerHealth -= damageAmountPerTick;
        }
        std::cout << "Player " << playerID << " took " << damageAmountPerTick << " CONTINUOUS damage from tile! Health: " << playerHealth << std::endl;
        if (isEliminated) {
            std::cout << "Player " << playerID << " was eliminated by a damage tile!" << std::endl;
        }
    }
}

float Player::getDamageTickRate() const { return damageTickRate; }
sf::Time Player::getDamageElapsedTime() const { return damageTickClock.getElapsedTime(); }
void Player::restartDamageTickClock() { damageTickClock.restart(); }

sf::Vector2i Player::getLastGridPosition() const { return lastGridPosition; }
void Player::setLastGridPosition(sf::Vector2i pos) { lastGridPosition = pos; }

void Player::resetHealthAndState() {
    playerHealth = 100;
    isEliminated = false;
    isStuck = false;
    isOnDamageTile = false;
    tookInitialDamage = false;
    lastGridPosition = {-1, -1};
    velocity = {0.0f, 0.0f};
    currentFrame = 0;
    currentAnimationDirection = AnimDown;
    isMoving = false;
    facingLeft = false;

    // Ensure initial frame and properties are set after reset
    if (!animationFrames[currentAnimationDirection].empty()) {
        playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
    }
    updateAnimation(); // Apply scaling and origin based on current state and tile size
}