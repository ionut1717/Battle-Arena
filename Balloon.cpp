#include "Balloon.h"
#include "Player.h"
#include <cmath>

int AttackBalloon::damage=10;
float AttackBalloon::lifespan = 12.0f;
float AttackBalloon::radius = 8.0f;

AttackBalloon::AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates, int ownerID)
    : sf::CircleShape(radius),
      ownerID(ownerID)
{
    this->setFillColor(color);
    this->setOrigin({radius, radius});
    this->setPosition(startCoordinates);
}

void AttackBalloon::setTarget(const Player* targe) {
    target = targe;
}

const Player* AttackBalloon::getTarget() const {
    return target;
}

int AttackBalloon::getOwnerID() const {
    return ownerID;
}

void AttackBalloon::launch(sf::Vector2f initialVel) {
    m_initialLaunchVelocity = initialVel;
    m_currentVelocity = initialVel;
    lifetimeClock.restart();
}

void AttackBalloon::update(float deltaTime) {
    sf::Vector2f trackingForce = {0.0f, 0.0f};

    if (target != nullptr && !target->isEliminatedPlayer()) {
        sf::Vector2f targetPosition = target->getPosition();
        sf::Vector2f balloonPosition = this->getPosition();

        sf::Vector2f directionToTarget = targetPosition - balloonPosition;
        float distanceToTarget = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);

        if (distanceToTarget !=0) {
            trackingForce = (directionToTarget / distanceToTarget) * m_trackingSpeed;
        }
    }

    float elapsed = lifetimeClock.getElapsedTime().asSeconds();
    float attenuationFactor = 0.0f;
    if (elapsed < initialLaunchDuration) {
        float t = elapsed / initialLaunchDuration;
        attenuationFactor = std::pow(1.0f - t, 1.5f);
    }

    m_currentVelocity = (m_initialLaunchVelocity * attenuationFactor) + trackingForce;

    float currentSpeedMagnitude = std::sqrt(m_currentVelocity.x * m_currentVelocity.x + m_currentVelocity.y * m_currentVelocity.y);
    if (currentSpeedMagnitude > m_maxSpeed) {
        m_currentVelocity = (m_currentVelocity / currentSpeedMagnitude) * m_maxSpeed;
    } else if ((attenuationFactor == 0.0f &&  currentSpeedMagnitude < 1.0f) || target == nullptr) {
        m_currentVelocity = {0.0f, 0.0f};
    }

    this->move(m_currentVelocity * deltaTime);
}

bool AttackBalloon::isExpired() const {
    return lifetimeClock.getElapsedTime().asSeconds() >= lifespan;
}

