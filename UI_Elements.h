#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <SFML/Graphics.hpp>
class Player;
class Arena;

namespace UIConstants {
    constexpr float HEALTH_BAR_WIDTH = 200.0f;
    constexpr float HEALTH_BAR_HEIGHT = 25.0f;
    constexpr float HEALTH_BAR_PADDING = 15.0f;
    constexpr unsigned int HEALTH_TEXT_SIZE = 18;
    constexpr float HEALTH_TEXT_OFFSET_Y = 10.0f;

    constexpr float OWNERSHIP_BAR_WIDTH = 350.0f;
    constexpr float OWNERSHIP_BAR_HEIGHT = 25.0f;
    constexpr float OWNERSHIP_BAR_PADDING_TOP = 15.0f;
    constexpr unsigned int OWNERSHIP_TEXT_SIZE = 16;
    constexpr unsigned int TOTAL_TILES_TEXT_SIZE = 12;
    constexpr unsigned int WIN_MESSAGE_TEXT_SIZE = 52;
}

class UI_Elements {
public:
    explicit UI_Elements(const sf::Font& font);

    void update(unsigned int windowWidth, unsigned int windowHeight,
                const Player& player1, const Player& player2,
                const Arena& arena);

    void draw(sf::RenderWindow& window);

private:
    sf::Font m_font;
    sf::RectangleShape m_player1HealthBarBackground;
    sf::RectangleShape m_player1HealthBarGreen;
    sf::RectangleShape m_player1HealthBarRed;
    sf::Text m_player1HealthText;

    sf::RectangleShape m_player2HealthBarBackground;
    sf::RectangleShape m_player2HealthBarGreen;
    sf::RectangleShape m_player2HealthBarRed;
    sf::Text m_player2HealthText;

    sf::RectangleShape m_ownershipBarBackground;
    sf::RectangleShape m_player1OwnershipBar;
    sf::RectangleShape m_player2OwnershipBar;
    sf::Text m_player1PercentageText;
    sf::Text m_player2PercentageText;

};

#endif // UI_ELEMENTS_H