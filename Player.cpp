#include "Player.h"
#include <algorithm>
#include <iostream>
#include "Arena/Tiles/Tiles.h"
#include "Resource_Manager.h"
#include <cmath>
#include "Balloon.h"

std::vector<std::unique_ptr<Player>> Player::s_playerPool;
std::vector<Player*> Player::s_activePlayers;

sf::IntRect Player::getFrameRect(int row, int col) {
    return sf::IntRect({static_cast<int>(col * m_frameSize), static_cast<int>(row * m_frameSize)}, {static_cast<int>(m_frameSize), static_cast<int>(m_frameSize)});
}

void Player::setupAnimationFrames() {
    animationFrames[AnimDown]      = { getFrameRect(0,0), getFrameRect(0,1), getFrameRect(0,2), getFrameRect(0,3) };
    animationFrames[AnimDownRight] = { getFrameRect(0,4), getFrameRect(0,5), getFrameRect(0,6), getFrameRect(0,7) };
    animationFrames[AnimRight]     = { getFrameRect(0,8), getFrameRect(1,0), getFrameRect(1,1), getFrameRect(1,2) };
    animationFrames[AnimUpRight]   = { getFrameRect(1,7), getFrameRect(1,8), getFrameRect(2,0), getFrameRect(2,1) };
    animationFrames[AnimUp]        = { getFrameRect(1,3), getFrameRect(1,4), getFrameRect(1,5), getFrameRect(1,6) };
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
    float scaleFactor = m_tileSize / static_cast<float>(m_frameSize);

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
        currentFrame = 0;
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

Player::Player(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture* playerTexturePtr)
    : playerID(playerID), color(color), coordinates(startPosition), wasd(wasd),
      playerTexture(playerTexturePtr),
      playerSprite(*this->playerTexture),
      playerSpeed(250.0f),
      isEliminated(false),
      isStuck(false),
      stuckSpeedFactor(0.2f),
      isOnDamageTile(false),
      damageTickRate(1.0f),
      damageAmountPerTick(1),
      tookInitialDamage(false),
      lastGridPosition({-1, -1}),
      animationFrameTime(1.0f / 10.0f),
      currentFrame(0),
      currentAnimationDirection(AnimDown),
      isMoving(false),
      facingLeft(false),
      paintCooldownTime(3.0f),
      m_inUse(false)
{
    m_tileSize = 4 * Tile::getSize();
    m_characterVisualSize = m_tileSize / 2.0f;

    setupAnimationFrames();

    if (!animationFrames[currentAnimationDirection].empty()) {
        playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
    }
    updateAnimation();
    playerSprite.setPosition(coordinates);
    playerRadius = m_characterVisualSize / 2.0f;
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
    playerHealth = 100;
}

Player* Player::acquirePlayer(int playerID, const sf::Color& color, sf::Vector2f startPosition, bool wasd, sf::Texture* playerTexturePtr) {
    if (s_activePlayers.size() >= MAX_PLAYERS) {
        return nullptr;
    }

    Player* player = nullptr;
    if (!s_playerPool.empty()) {
        player = s_playerPool.back().release();
        s_playerPool.pop_back();
    } else {
        player = new Player(playerID, color, startPosition, wasd, playerTexturePtr);
    }

    if (player) {
        player->playerID = playerID;
        player->color = color;
        player->coordinates = startPosition;
        player->wasd = wasd;
        player->playerTexture = playerTexturePtr;
        player->playerSprite.setTexture(*playerTexturePtr);
        player->m_inUse = true;
        player->resetForReuse();
        s_activePlayers.push_back(player);
    }
    return player;
}

void Player::releasePlayer(Player* player) {
    if (player && player->m_inUse) {
        auto it = std::remove(s_activePlayers.begin(), s_activePlayers.end(), player);
        s_activePlayers.erase(it, s_activePlayers.end());
        player->m_inUse = false;
        player->resetForReuse();
        s_playerPool.emplace_back(player);
    }
}

void Player::resetForReuse() {
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
    if (!animationFrames[currentAnimationDirection].empty()) {
        playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
    }
    updateAnimation();
    playerSprite.setPosition(coordinates);
    for (auto const& [key, val] : keysPressed) {
        keysPressed[key] = false;
    }
    paintCooldownClock.restart();
    damageTickClock.restart();
}

int Player::getPlayerID() const { return playerID; }

void Player::getDamageBalloon() {
    int damage = AttackBalloon::getDamage();
    if (playerHealth <= damage) {
        playerHealth = 0;
        isEliminated = true;
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
    float halfEffectiveSize = m_characterVisualSize / 2.0f;

    coordinates.x = std::clamp(coordinates.x, halfEffectiveSize, static_cast<float>(gridSize) * Tile::getSize() - halfEffectiveSize);
    coordinates.y = std::clamp(coordinates.y, halfEffectiveSize, static_cast<float>(gridSize) * Tile::getSize() - halfEffectiveSize);

    playerSprite.setPosition(coordinates);
    updateAnimation();
}

void Player::draw(sf::RenderWindow& window) const {
    if (!isEliminated) {
        window.draw(playerSprite);
    }
}

sf::FloatRect Player::getBounds() const {
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

    if (speed > 0.1f) {
        direction = velocity / speed;
        return direction;
    }

    switch (currentAnimationDirection) {
        case AnimDown:      direction = {0.0f, 1.0f}; break;
        case AnimDownRight: direction = {1.0f, 1.0f}; break;
        case AnimRight:     direction = {1.0f, 0.0f}; break;
        case AnimUpRight:   direction = {1.0f, -1.0f}; break;
        case AnimUp:        direction = {0.0f, -1.0f}; break;
        case AnimUpLeft:    direction = {-1.0f, -1.0f}; break;
        case AnimLeft:      direction = {-1.0f, 0.0f}; break;
        case AnimDownLeft:  direction = {-1.0f, 1.0f}; break;
        case AnimCount:
        default:
            if (facingLeft) { direction = {-1.0f, 0.0f}; }
            else { direction = {1.0f, 0.0f}; }
            if (direction.x == 0.0f && direction.y == 0.0f) {
                 direction = {0.0f, 1.0f};
            }
            break;
    }

    float dirLength = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (dirLength > 0.001f) {
        direction /= dirLength;
    } else {
        direction = {0.0f, 1.0f};
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
        return;
    }

    sf::Vector2f playerCenter = coordinates;

    float worldRadius = radius_factor * m_characterVisualSize;

    int minTileX = std::max(0, static_cast<int>((playerCenter.x - worldRadius) / Tile::getSize()));
    int maxTileX = std::min(static_cast<int>(grid[0].size() -1) , static_cast<int>((playerCenter.x + worldRadius) / Tile::getSize()));
    int minTileY = std::max(0, static_cast<int>((playerCenter.y - worldRadius) / Tile::getSize()));
    int maxTileY = std::min(static_cast<int>(grid.size() - 1), static_cast<int>((playerCenter.y + worldRadius) / Tile::getSize()));

    for (int y = minTileY; y <= maxTileY; ++y) {
        for (int x = minTileX; x <= maxTileX; ++x) {
            if (y >= 0 && static_cast<size_t>(y) < grid.size() && x >= 0 && static_cast<size_t>(x) < grid[y].size()) {
                Tile& tile = *grid[y][x];
                sf::Vector2f tileCenter = tile.getPosition() + sf::Vector2f(Tile::getSize() / 2.0f, Tile::getSize() / 2.0f);

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

void Player::setStuck(bool stuck,float value) { isStuck = stuck; stuckSpeedFactor=value;}
bool Player::getIsStuck() const { return isStuck; }
void Player::setPosition(sf::Vector2f newPos) {
    coordinates = newPos;
    playerSprite.setPosition(coordinates);
    setOnDamageTile(false);
    setStuck(false,0.2f);
    tookInitialDamage = false;
    lastGridPosition = {-1, -1};
}
void Player::setOnDamageTile(bool onTile) {
    isOnDamageTile = onTile;
    if (!onTile) {
        tookInitialDamage = false;
    }
}
bool Player::getIsOnDamageTile() const { return isOnDamageTile; }
void Player::getDamageTile(float value) {
    if (value>playerHealth) {
        playerHealth = 0;
        isEliminated = true;
    }
    else {
        playerHealth -= value;
    }
}

void Player::takeInitialDamage() {
    if (!tookInitialDamage && playerHealth > 0) {
        if (playerHealth <= static_cast<unsigned int>(damageAmountPerTick)) {
            playerHealth = 0;
            isEliminated = true;
        } else {
            playerHealth -= damageAmountPerTick;
        }
        tookInitialDamage = true;
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

    if (!animationFrames[currentAnimationDirection].empty()) {
        playerSprite.setTextureRect(animationFrames[currentAnimationDirection][0]);
    }
    updateAnimation();
    playerSprite.setPosition(coordinates);
}