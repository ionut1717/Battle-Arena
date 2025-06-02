#ifndef BALLOON_H
#define BALLOON_H

#include <SFML/Graphics.hpp>
class Player;

class AttackBalloon : public sf::CircleShape{
    sf::CircleShape m_shape;
    const Player* m_target = nullptr;
    sf::Vector2f coordinates;
    float m_trackingSpeed = 250.f;
    float m_maxSpeed = 1000.f;
    sf::Vector2f m_initialLaunchVelocity = {0.0f, 0.0f}; // Initial velocity when launched
    sf::Vector2f m_currentVelocity = {0.0f, 0.0f}; // Current velocity after combining launch and tracking
    float initialLaunchDuration = 0.6f; // Duration over which initial launch velocity attenuates

    sf::Clock lifetimeClock; // Clock to track balloon's lifespan and initial launch duration
    static float lifespan; // Total lifespan of the balloon in seconds
    static float radius;
public:
    AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates);

    void setTarget(const Player* target);

    void launch(sf::Vector2f initialVel);

    void update(float deltaTime);

    bool isExpired() const;

    // Get the radius of the balloon
    float getRadius() const;
};

#endif // BALLOON_H