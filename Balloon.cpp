#include "Balloon.h" // Sau "Balloon.h"
#include "Player.h"        // Pentru a putea folosi Player* și metodele sale (ex: getPosition)
#include <cmath>           // Pentru std::sqrt, std::pow

// Inițializarea membrilor statici
int AttackBalloon::damage=10;
float AttackBalloon::lifespan = 5.0f; // Durata de viață a balonului (ex: 5 secunde)
float AttackBalloon::radius = 8.0f;   // Raza balonului (ex: 8 pixeli)

// Constructor implementation
AttackBalloon::AttackBalloon(const sf::Color& color, sf::Vector2f startCoordinates, int ownerID)
    : sf::CircleShape(radius),
      m_ownerID(ownerID)
{
    // Folosește metodele moștenite pentru a configura shape-ul
    this->setFillColor(color);
    this->setOrigin({AttackBalloon::radius, AttackBalloon::radius}); // Setează originea în centru
    this->setPosition(startCoordinates);
    // m_currentVelocity și m_initialLaunchVelocity sunt inițializate la {0,0} implicit
}

void AttackBalloon::setTarget(const Player* target) {
    m_target = target;
}

const Player* AttackBalloon::getTarget() const {
    return m_target;
}

int AttackBalloon::getOwnerID() const {
    return m_ownerID;
}

void AttackBalloon::launch(sf::Vector2f initialVel) {
    m_initialLaunchVelocity = initialVel;
    m_currentVelocity = initialVel; // Viteza curentă începe cu cea de lansare
    lifetimeClock.restart();        // Pornește cronometrul de viață la lansare
}

void AttackBalloon::update(float deltaTime) {
    sf::Vector2f trackingForce = {0.0f, 0.0f};

    if (m_target != nullptr && !m_target->isEliminatedPlayer()) { // Urmărește ținta doar dacă există și nu e eliminată
        sf::Vector2f targetPosition = m_target->getPosition();
        sf::Vector2f balloonPosition = this->getPosition(); // Folosește poziția moștenită

        sf::Vector2f directionToTarget = targetPosition - balloonPosition;
        float distanceToTarget = std::sqrt(directionToTarget.x * directionToTarget.x + directionToTarget.y * directionToTarget.y);

        if (distanceToTarget > 0.01f) { // O mică toleranță pentru a evita împărțirea la zero
            trackingForce = (directionToTarget / distanceToTarget) * m_trackingSpeed;
        }
    }

    // Atenuarea vitezei inițiale
    float elapsed = lifetimeClock.getElapsedTime().asSeconds();
    float attenuationFactor = 0.0f;
    if (elapsed < initialLaunchDuration) {
        float t = elapsed / initialLaunchDuration;
        attenuationFactor = std::pow(1.0f - t, 1.5f); // Exponent ajustat pentru o atenuare diferită
    }

    // Combină viteza inițială atenuată cu forța de urmărire
    m_currentVelocity = (m_initialLaunchVelocity * attenuationFactor) + trackingForce;

    // Limitează viteza maximă
    float currentSpeedMagnitude = std::sqrt(m_currentVelocity.x * m_currentVelocity.x + m_currentVelocity.y * m_currentVelocity.y);
    if (currentSpeedMagnitude > m_maxSpeed) {
        m_currentVelocity = (m_currentVelocity / currentSpeedMagnitude) * m_maxSpeed;
    } else if (attenuationFactor == 0.0f && m_target == nullptr && currentSpeedMagnitude < 1.0f) {
        // Oprește mișcarea dacă nu mai are impuls inițial, nu are țintă și e aproape static
        m_currentVelocity = {0.0f, 0.0f};
    }

    // Mișcă balonul folosind metoda moștenită
    this->move(m_currentVelocity * deltaTime);
}

bool AttackBalloon::isExpired() const {
    return lifetimeClock.getElapsedTime().asSeconds() >= AttackBalloon::lifespan;
}

// Metoda getRadius() este moștenită de la sf::CircleShape.
// Dacă vrei un getter care returnează specific membrul static, ai putea adăuga:
// float AttackBalloon::getStaticRadius() const { return AttackBalloon::radius; }
// Dar în Game.cpp, balloon.getRadius() va funcționa corect datorită moștenirii.