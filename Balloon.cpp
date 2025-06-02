#include "Balloon.h"
#include "Player.h" // Include Player.h to access Player's methods (e.g., getPosition())

float AttackBalloon::lifespan=15;
float AttackBalloon::radius=10;

// Constructor implementation
AttackBalloon::AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates)
    : m_shape(radius), coordinates(startCoordinates) {
    m_shape.setFillColor(color);
    m_shape.setOrigin({radius, radius}); // Set origin to center for easier positioning
    m_shape.setPosition(coordinates);
}

// setTarget implementation
void AttackBalloon::setTarget(const Player* target) {
    m_target = target;
}

// launch implementation
void AttackBalloon::launch(sf::Vector2f initialVel) {
    m_initialLaunchVelocity = initialVel;
    m_currentVelocity = initialVel;
    lifetimeClock.restart(); // Restart the clock upon launch
}

// update implementation
void AttackBalloon::update(float deltaTime) {
    if (m_target != nullptr) {
        sf::Vector2f targetPosition = m_target->getPosition();
        sf::Vector2f balloonPosition = m_shape.getPosition();

        sf::Vector2f directionToTarget = targetPosition - balloonPosition;
        float distanceToTarget = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);

        sf::Vector2f desiredTrackingVelocity = {0.0f, 0.0f};
        if (distanceToTarget > 0.f) {
            desiredTrackingVelocity = (directionToTarget / distanceToTarget) * m_trackingSpeed;
        }

        // Non-linear attenuation calculation
        float elapsed = lifetimeClock.getElapsedTime().asSeconds();
        float attenuationFactor = 0.0f;
        if (elapsed < initialLaunchDuration) {
            // Use a power function for smoother initial attenuation
            float t = elapsed / initialLaunchDuration; // t from 0 to 1
            attenuationFactor = std::pow(1.0f - t, 2.0f); // Power of 2.0f for smoother initial attenuation
        } else {
            attenuationFactor = 0.0f; // Fully attenuated after initial duration
        }

        // Combine attenuated initial velocity with tracking velocity
        m_currentVelocity = (m_initialLaunchVelocity * attenuationFactor) + desiredTrackingVelocity;

        // Limit maximum speed to prevent extreme values
        float currentSpeedMagnitude = std::sqrt(m_currentVelocity.x * m_currentVelocity.x + m_currentVelocity.y * m_currentVelocity.y);
        if (currentSpeedMagnitude > m_maxSpeed) {
            m_currentVelocity = (m_currentVelocity / currentSpeedMagnitude) * m_maxSpeed;
        } else if (currentSpeedMagnitude < 0.01f && attenuationFactor == 0.0f) {
            // If movement is negligible and initial impulse is fully attenuated, stop movement
            m_currentVelocity = {0.0f, 0.0f};
        }

        std::move(m_currentVelocity * deltaTime);
        coordinates = m_shape.getPosition(); // Update internal coordinates
    } else {
        // If no target, continue movement based on attenuated initial impulse
        float elapsed = lifetimeClock.getElapsedTime().asSeconds();
        float attenuationFactor = 0.0f;
        if (elapsed < initialLaunchDuration) {
            float t = elapsed / initialLaunchDuration;
            attenuationFactor = std::pow(1.0f - t, 2.0f);
        } else {
            attenuationFactor = 0.0f;
        }

        m_currentVelocity = m_initialLaunchVelocity * attenuationFactor;

        std::move(m_currentVelocity * deltaTime);
        coordinates = m_shape.getPosition(); // Update internal coordinates
    }
}

// isExpired implementation
bool AttackBalloon::isExpired() const {
    return lifetimeClock.getElapsedTime().asSeconds() >= lifespan;
}

// getRadius implementation
float AttackBalloon::getRadius() const { return radius; }