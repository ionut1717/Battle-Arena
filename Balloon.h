#ifndef BALLOON_H
#define BALLOON_H

#include <SFML/Graphics.hpp>

class Player;

class AttackBalloon : public sf::CircleShape {
    const Player* target = nullptr;
    int ownerID;
    static int damage;
    float m_trackingSpeed = 250.f;
    float m_maxSpeed = 700.f;
    sf::Vector2f m_initialLaunchVelocity = {0.0f, 0.0f};
    sf::Vector2f m_currentVelocity = {0.0f, 0.0f};
    float initialLaunchDuration = 0.5f;
    static float lifespan;
    static float radius;
    sf::Clock lifetimeClock;
public:


    AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates, int ownerID);

    void setTarget(const Player* target);
    const Player* getTarget() const;
    int getOwnerID() const;
    static int getDamage(){return damage;};
    void launch(sf::Vector2f initialVel);
    void update(float deltaTime);
    bool isExpired() const;

};

#endif