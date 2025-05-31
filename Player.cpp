#include "Player.h"
#include "Tiles.h" // Include Tiles for Tile::getSize() and TileSpecialType logic

// Helper method to calculate sf::IntRect
sf::IntRect Player::getFrameRect(int row, int col) {
    return sf::IntRect({col * FRAME_WIDTH, row * FRAME_HEIGHT}, {FRAME_WIDTH, FRAME_HEIGHT});
}

// Helper method to populate animation frames
void Player::setupAnimationFrames() {
    // Down: Row 1, columns 1-4.
    animationFrames[AnimDown] = {
        getFrameRect(0, 0), getFrameRect(0, 1), getFrameRect(0, 2), getFrameRect(0, 3)
    };
    // Down-Right: Row 1, columns 5-8.
    animationFrames[AnimDownRight] = {
        getFrameRect(0, 4), getFrameRect(0, 5), getFrameRect(0, 6), getFrameRect(0, 7)
    };
    // Right: Row 1, column 9 + Row 2, columns 1-3.
    animationFrames[AnimRight] = {
        getFrameRect(0, 8), getFrameRect(1, 0), getFrameRect(1, 1), getFrameRect(1, 2)
    };
    // Up: Row 2, columns 4-7.
    animationFrames[AnimUp] = {
        getFrameRect(1, 3), getFrameRect(1, 4), getFrameRect(1, 5), getFrameRect(1, 6)
    };
    // Up-Right: Row 2, columns 8-9 + Row 3, columns 1-2.
    animationFrames[AnimUpRight] = {
        getFrameRect(1, 7), getFrameRect(1, 8), getFrameRect(2, 0), getFrameRect(2, 1)
    };

    // For left-facing directions, we use the right-facing frames and flip the sprite.
    animationFrames[AnimDownLeft] = animationFrames[AnimDownRight];
    animationFrames[AnimLeft] = animationFrames[AnimRight];
    animationFrames[AnimUpLeft] = animationFrames[AnimUpRight];
}

// Method to determine animation direction based on velocity vector
void Player::determineAnimationDirection(sf::Vector2f currentVelocity) {
    if (currentVelocity.x == 0 && currentVelocity.y == 0) {
        isMoving = false;
        return;
    }
    isMoving = true;

    // Use currentVelocity parameter
    if (currentVelocity.x > 0 && currentVelocity.y == 0) { // 0 degrees: Right
        currentAnimationDirection = AnimRight;
        facingLeft = false;
    } else if (currentVelocity.x > 0 && currentVelocity.y < 0) { // Up-Right
        currentAnimationDirection = AnimUpRight;
        facingLeft = false;
    } else if (currentVelocity.x == 0 && currentVelocity.y < 0) { // 90 degrees: Up
        currentAnimationDirection = AnimUp;
        facingLeft = false;
    } else if (currentVelocity.x < 0 && currentVelocity.y < 0 ) { // Up-Left
        currentAnimationDirection = AnimUpLeft;
        facingLeft = true;
    } else if (currentVelocity.x < 0 && currentVelocity.y == 0) { // 180 degrees: Left
        currentAnimationDirection = AnimLeft;
        facingLeft = true;
    } else if (currentVelocity.x < 0  && currentVelocity.y > 0) { // Down-Left
        currentAnimationDirection = AnimDownLeft;
        facingLeft = true;
    } else if (currentVelocity.x == 0 && currentVelocity.y > 0) { // 270 degrees: Down
        currentAnimationDirection = AnimDown;
        facingLeft = false;
    } else if (currentVelocity.x >= 0 && currentVelocity.y > 0) { // Down-Right
        currentAnimationDirection = AnimDownRight;
        facingLeft = false;
    }
}

void Player::updateAnimation() {
    if (!isMoving) {
        // If not moving, display the first frame of the current direction (or AnimDown if empty)
        if (!animationFrames[currentAnimationDirection].empty()) {
            playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
        }
        currentFrame = 0; // Reset frame when stopping
        animationClock.restart(); // Restart clock to begin animation from start when movement resumes
    } else { // Only if moving, advance the animation
        if (animationClock.getElapsedTime().asSeconds() >= animationFrameTime) {
            currentFrame = (currentFrame + 1) % animationFrames[currentAnimationDirection].size();
            playerSprite.setTextureRect(animationFrames[currentAnimationDirection][currentFrame]);
            animationClock.restart();
        }
    }

    // Apply scaling and mirroring if necessary
    if (facingLeft) {
        playerSprite.setScale({-currentScaleFactor, currentScaleFactor}); // Use currentScaleFactor
        // Adjust origin for mirroring to keep position correct
        playerSprite.setOrigin({FRAME_WIDTH - (FRAME_WIDTH / 2.0f), FRAME_HEIGHT / 2.0f});
    } else {
        playerSprite.setScale({currentScaleFactor, currentScaleFactor}); // Use currentScaleFactor
        playerSprite.setOrigin({FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f});
    }
}


// Constructor
Player::Player(const int playerID, const sf::Color color, const sf::Vector2f startPosition, const bool wasd, sf::Texture PlayerTexture)
    : playerID(playerID), color(color), coordinates(startPosition), wasd(wasd), playerSprite(PlayerTexture) {

    currentScaleFactor = 0.5f; // Set desired scale factor (0.5 = half size)

    // Load specific player texture based on ID
    std::string texturePath;
    if (playerID == 1) {
        texturePath = "hero.png"; // Assuming "hero.png" is in the same directory as the executable, or relative path
    } else {
        texturePath = "base_character.png"; // Assuming "base_character.png" is in the same directory as the executable
    }

    if (!playerTexture.loadFromFile(texturePath)) {
        std::cerr << "Error: Could not load player texture for player " << playerID << " from " << texturePath << std::endl;
    }
    playerSprite.setTexture(playerTexture);
    playerSprite.setOrigin({FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f}); // Set origin to center
    playerSprite.setPosition(coordinates); // Initial sprite position

    playerSprite.setScale({currentScaleFactor, currentScaleFactor});
    playerRadius = (FRAME_WIDTH * currentScaleFactor) / 2.0f; // Adjust radius for collision

    setupAnimationFrames(); // Populate animation frames map
    // Set initial sprite frame
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


int Player::GetPlayerID() { return playerID; }

void Player::GetDamageBalloon() {
    if (PlayerHealth <= 10) {
        isEliminated = true;
        PlayerHealth = 0;
        std::cout << "Player " << playerID << " a fost eliminat de un balon!" << std::endl;
    } else {
        PlayerHealth -= 10;
    }
}

void Player::set_key_pressed(const sf::Keyboard::Key key, const bool pressed) {
    keysPressed[key] = pressed;
}

float Player::getActualPlayerSpeed() const {
    return isStuck ? PlayerSpeed * stuckSpeedFactor : PlayerSpeed;
}

void Player::update_position(float deltaTime) {
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
    // Normalize movement
    float length = std::sqrt(currentInputDirection.x * currentInputDirection.x + currentInputDirection.y * currentInputDirection.y);
    if (length > 0.0f) {
        velocity = (currentInputDirection / length) * getActualPlayerSpeed();
        determineAnimationDirection(velocity); // Determine animation direction
    } else {
        velocity = {0.0f, 0.0f};
        isMoving = false; // Not moving, stop animation
    }

    coordinates += velocity * deltaTime;
    playerSprite.setPosition(coordinates); // Update sprite position

    int grid_size = Arena::GetGridSize();
    float tile_size = Tile::getSize();

    // Adjust collision with borders based on scaled sprite size
    sf::FloatRect bounds = playerSprite.getGlobalBounds();
    float halfWidth = bounds.size.x / 2.0f;
    float halfHeight = bounds.size.y / 2.0f;

    // Left limit
    if (coordinates.x < halfWidth) {
        coordinates.x = halfWidth;
    }
    // Right limit
    else if (coordinates.x > static_cast<float>(grid_size) * tile_size - halfWidth) {
        coordinates.x = static_cast<float>(grid_size) * tile_size - halfWidth;
    }

    // Top limit
    if (coordinates.y < halfHeight) {
        coordinates.y = halfHeight;
    }
    // Bottom limit
    else if (coordinates.y > static_cast<float>(grid_size) * tile_size - halfHeight) {
        coordinates.y = static_cast<float>(grid_size) * tile_size - halfHeight;
    }

    updateAnimation(); // Update animation
}

void Player::draw(sf::RenderWindow& window) {
    if (!isEliminated) {
        window.draw(playerSprite); // Draw sprite instead of circle
    }
}

sf::FloatRect Player::getBounds() const {
    // Now return sprite bounds for more precise collision
    return playerSprite.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const { return coordinates; }
unsigned int Player::getHealth() const { return PlayerHealth; }
bool Player::isEliminatedPlayer() const { return isEliminated; }
float Player::getRadius() const { return playerRadius; }
sf::Vector2f Player::getVelocity() const { return velocity; }

void Player::paintNearbyTiles(std::vector<std::vector<std::unique_ptr<Tile>>>& grid, const float radius) {
    float tileSize = Tile::getSize();
    sf::Vector2f playerCenter = coordinates;
    // Ensure grid is not empty before accessing grid[0]
    int gridWidth = grid.empty() ? 0 : grid[0].size();
    int gridHeight = grid.size();

    float worldRadius = radius * tileSize;

    for (int i = 0; i < gridHeight; ++i) {
        for (int j = 0; j < gridWidth; ++j) {
            // Access the Tile by dereferencing unique_ptr
            Tile& tile = *grid[i][j];
            sf::Vector2f tile_position = tile.getPosition();
            sf::Vector2f tileCenter = tile_position + sf::Vector2f(tileSize / 2.0f, tileSize / 2.0f);

            float dx = tileCenter.x - playerCenter.x;
            float dy = tileCenter.y - playerCenter.y;
            if (dx * dx + dy * dy <= worldRadius * worldRadius) {
                // Call the claimTile method from Tile.h
                tile.claimTile(playerID, color);
            }
        }
    }
}

Player* Player::getPlayer() {
    return this;
}
sf::Color Player::getColor() {
    return color;
}
void Player::setStuck(bool stuck) {
    isStuck = stuck;
}
bool Player::getIsStuck() const {
    return isStuck;
}

void Player::setPosition(sf::Vector2f newPos) {
    coordinates = newPos;
    playerSprite.setPosition(coordinates); // Update the sprite position as well
    setOnDamageTile(false);
    setStuck(false);
    tookInitialDamage = false;
    lastGridPosition = {-1, -1};
}

void Player::setOnDamageTile(bool onTile) {
    isOnDamageTile = onTile;
    if (!onTile) {
        tookInitialDamage = false;
    }
}
bool Player::getIsOnDamageTile() const {
    return isOnDamageTile;
}

void Player::takeInitialDamage() {
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

void Player::takeContinuousDamage() {
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

float Player::getDamageTickRate() const {
    return damageTickRate;
}
sf::Time Player::getDamageElapsedTime() const {
    return damageTickClock.getElapsedTime();
}
void Player::restartDamageTickClock() {
    damageTickClock.restart();
}

sf::Vector2i Player::getLastGridPosition() const {
    return lastGridPosition;
}
void Player::setLastGridPosition(sf::Vector2i pos) {
    lastGridPosition = pos;
}
void Player::resetHealthAndState() {
    PlayerHealth = 100;
    isEliminated = false;
    isStuck = false;
    isOnDamageTile = false;
    tookInitialDamage = false;

    lastGridPosition = {-1, -1};
    velocity = {0.0f, 0.0f}; // Reset velocity
    currentFrame = 0; // Reset animation frame
    currentAnimationDirection = AnimDown; // Reset animation direction
    isMoving = false;
    facingLeft = false;
    playerSprite.setScale({currentScaleFactor, currentScaleFactor}); // Apply scale factor on reset
}