#ifndef BALLOON_H
#define BALLOON_H

#include <SFML/Graphics.hpp>
#include <cmath> // For std::sqrt and std::pow

// Forward declaration of the Player class to avoid circular includes.
// The full definition of Player is only needed in Balloon.cpp where Player methods are called.
class Player;

class AttackBalloon : public sf::CircleShape {
public:
    // Constructor
    AttackBalloon(float radius, const sf::Color& color, sf::Vector2f startCoordinates);

    // Set target for the balloon (should be const Player* if it only observes)
    void setTarget(const Player* target);

    // Launch the balloon with an initial velocity
    void launch(sf::Vector2f initialVel);

    // Update balloon's position and behavior
    void update(float deltaTime);

    // Check if the balloon's lifespan has expired
    bool isExpired() const;

    // Get the radius of the balloon
    float getRadius() const;

private:
    const Player* m_target = nullptr; // Pointer to the target player (constant pointer)
    sf::Vector2f coordinates; // Current position of the balloon
    float m_trackingSpeed = 250.f; // Speed at which the balloon tracks its target
    float m_maxSpeed = 1000.f; // Maximum absolute speed of the balloon
    float m_radius; // Stored for consistent access

    sf::Vector2f m_initialLaunchVelocity = {0.0f, 0.0f}; // Initial velocity when launched
    sf::Vector2f m_currentVelocity = {0.0f, 0.0f}; // Current velocity after combining launch and tracking
    float initialLaunchDuration = 0.6f; // Duration over which initial launch velocity attenuates

    sf::Clock lifetimeClock; // Clock to track balloon's lifespan and initial launch duration
    float lifespan = 15.0f; // Total lifespan of the balloon in seconds
};

#endif // BALLOON_H