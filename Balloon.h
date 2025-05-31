#ifndef BALLOON_H
#define BALLOON_H

#include <SFML/Graphics.hpp>
#include <cmath>
// Nu mai include Player.h aici direct pentru a evita includerile circulare.
// Folosim o declarație forward.
class Player; // Declarație forward

class AttackBalloon : public sf::CircleShape {
public:
    // Constructor
    AttackBalloon(float radius, const sf::Color& color, sf::Vector2f startCoordinates)
        : sf::CircleShape(radius), coordinates(startCoordinates) {
        setFillColor(color);
        setOrigin({radius, radius});
        setPosition(coordinates);
        m_radius = radius;
    }

    // setTarget ar trebui să primească un pointer constant sau o referință constantă dacă nu modifică ținta
    void setTarget(const Player* target) { // Adăugat const
        m_target = target;
    }

    void launch(sf::Vector2f initialVel) {
        m_initialLaunchVelocity = initialVel;
        m_currentVelocity = initialVel;
        lifetimeClock.restart();
    }

    void update(float deltaTime) {
        // Balonul are o viteză inițială de lansare, apoi începe să urmărească ținta
        if (m_target != nullptr) {
            sf::Vector2f targetPosition = m_target->getPosition();
            sf::Vector2f balloonPosition = getPosition();

            sf::Vector2f directionToTarget = targetPosition - balloonPosition;
            float distanceToTarget = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);

            sf::Vector2f desiredTrackingVelocity = {0.0f, 0.0f};
            if (distanceToTarget > 0.f) {
                desiredTrackingVelocity = (directionToTarget / distanceToTarget) * m_trackingSpeed;
            }

            // --- NOU: Calcul non-liniar al factorului de atenuare ---
            float elapsed = lifetimeClock.getElapsedTime().asSeconds();
            float attenuationFactor = 0.0f;
            if (elapsed < initialLaunchDuration) {
                // Folosim o funcție de atenuare bazată pe o putere (ex: cubic)
                // 1.0 = complet, 0.0 = atenuat complet
                float t = elapsed / initialLaunchDuration; // t de la 0 la 1
                attenuationFactor = std::pow(1.0f - t, 2.0f); // Putere 2.0f pentru o atenuare mai lină inițial
            } else {
                attenuationFactor = 0.0f; // Complet atenuat după durata inițială
            }

            // Combină viteza inițială atenuată cu viteza de urmărire
            m_currentVelocity = (m_initialLaunchVelocity * attenuationFactor) + desiredTrackingVelocity;

            // Limitează viteza maximă pentru a preveni valori extreme
            float currentSpeedMagnitude = std::sqrt(m_currentVelocity.x * m_currentVelocity.x + m_currentVelocity.y * m_currentVelocity.y);
            if (currentSpeedMagnitude > m_maxSpeed) {
                m_currentVelocity = (m_currentVelocity / currentSpeedMagnitude) * m_maxSpeed;
            } else if (currentSpeedMagnitude < 0.01f && attenuationFactor == 0.0f) {
                m_currentVelocity = {0.0f, 0.0f};
            }

            move(m_currentVelocity * deltaTime);
            coordinates = getPosition();
        } else {
            // Dacă nu există țintă, continuă mișcarea bazată pe impulsul inițial atenuat.
            float elapsed = lifetimeClock.getElapsedTime().asSeconds();
            float attenuationFactor = 0.0f;
            if (elapsed < initialLaunchDuration) {
                float t = elapsed / initialLaunchDuration;
                attenuationFactor = std::pow(1.0f - t, 2.0f);
            } else {
                attenuationFactor = 0.0f;
            }

            m_currentVelocity = m_initialLaunchVelocity * attenuationFactor;

            move(m_currentVelocity * deltaTime);
            coordinates = getPosition();
        }
    }

    bool isExpired() const {
        return lifetimeClock.getElapsedTime().asSeconds() >= lifespan;
    }

    float getRadius() const { return m_radius; }

private:
    const Player* m_target = nullptr; // Acum este un pointer constant
    sf::Vector2f coordinates;
    float m_trackingSpeed = 250.f; // Ajustat la o viteză de urmărire puțin mai mică
    float m_maxSpeed = 1000.f; // Viteza maximă absolută a balonului
    float m_radius;

    sf::Vector2f m_initialLaunchVelocity = {0.0f, 0.0f};
    sf::Vector2f m_currentVelocity = {0.0f, 0.0f};
    float initialLaunchDuration = 0.6f; // Mărită la 0.6s pentru o curbură mai lungă

    sf::Clock lifetimeClock;
    float lifespan = 15.0f;
};

#endif // BALLOON_H