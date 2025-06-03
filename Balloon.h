#ifndef BALLOON_H // Păstrăm garda BALLOON_H dacă Game.cpp include "Balloon.h"
#define BALLOON_H

#include <SFML/Graphics.hpp>

class Player; // Forward declaration

class AttackBalloon : public sf::CircleShape {
private:
    const Player* m_target = nullptr;
    int m_ownerID;
    static int damage;
    float m_trackingSpeed = 250.f; // Viteză de urmărire
    float m_maxSpeed = 700.f;      // Viteză maximă (ajustată față de 1000.f pentru un efect poate mai bun)
    sf::Vector2f m_initialLaunchVelocity = {0.0f, 0.0f};
    sf::Vector2f m_currentVelocity = {0.0f, 0.0f};
    float initialLaunchDuration = 0.5f; // Durata atenuării vitezei inițiale (ajustată)

    sf::Clock lifetimeClock; // Cronometru pentru durata de viață

public:
    static float lifespan; // Durata totală de viață în secunde
    static float radius;   // Raza implicită pentru baloane

    // Constructor actualizat pentru a include ownerID
    AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates, int ownerID);

    void setTarget(const Player* target);
    const Player* getTarget() const;
    int getOwnerID() const;
    static int getDamage(){return damage;};
    void launch(sf::Vector2f initialVel);
    void update(float deltaTime);
    bool isExpired() const;

    // getRadius() este moștenit și va returna raza actuală a shape-ului.
    // Nu este nevoie de o implementare separată dacă folosim setRadius în constructor.
};

#endif // BALLOON_H