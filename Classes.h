#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <map>
#include<cmath>
#include <math.h>

class Game_Engine {

public:
};





// class Balloon {
//     sf::Color color;
//     Player target;
//     float speed;
//     sf::Vector2f coordinates;
//     sf::Vector2f target_coordinates;
// public:
//     Balloon(sf::Vector2f startPosition, sf::Color color, const Player* target):speed(100.0f),coordinates(startPosition),target=target
//     {
//         target_coordinates=target.getPosition();
//     }
//     void track(const sf::Vector2f& targetPosition)
//     {
//         sf::Clock timer;
//         sf::Vector2f currentVelocity{0.f, 0.f};
//         bool lockedOnDirection = false;
//
//         while (timer.getElapsedTime().asSeconds() < 10.0f)
//         {
//             // Compute vector from balloon to target
//             sf::Vector2f toTarget = targetPosition - coordinates;
//             float distance = std::sqrt(toTarget.x*toTarget.x + toTarget.y*toTarget.y);
//
//             if (!lockedOnDirection)
//             {
//                 if (distance > 20.0f)
//                 {
//                     // normalize toTarget and multiply by speed
//                     float inv = 1.f / distance;
//                     currentVelocity = { toTarget.x * inv * speed, toTarget.y * inv * speed };
//                 }
//                 else
//                 {
//                     // Too close—lock in the current velocity direction
//                     lockedOnDirection = true;
//                 }
//             }
//
//             // Move the balloon by the chosen velocity, scaled by frame‐time if you have it
//             coordinates += currentVelocity;
//
//             // Sleep or yield so this loop doesn’t hog CPU
//             sf::sleep(sf::milliseconds(10));
//         }
//     }
//
//
//     void draw(sf::RenderWindow& window) {
//         sf::CircleShape balloon(10.0f); // Example:  Draw as a circle
//         balloon.setFillColor(color);
//         balloon.setPosition(coordinates);
//         window.draw(balloon);
//     }
//
// };

