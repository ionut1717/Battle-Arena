#include "UI_Elements.h"
#include "Player.h"
#include "Arena/Arena.h"
#include <algorithm>

UI_Elements::UI_Elements(const sf::Font& font) :
    m_font(font),
    m_player1HealthText( m_font,"P1 HEALTH", UIConstants::HEALTH_TEXT_SIZE),
    m_player2HealthText( m_font,"P2 HEALTH", UIConstants::HEALTH_TEXT_SIZE),
    m_player1PercentageText(m_font,"",  UIConstants::OWNERSHIP_TEXT_SIZE),
    m_player2PercentageText(m_font,"",  UIConstants::OWNERSHIP_TEXT_SIZE)
{
    m_player1HealthBarBackground.setSize({UIConstants::HEALTH_BAR_WIDTH, UIConstants::HEALTH_BAR_HEIGHT});
    m_player1HealthBarBackground.setFillColor(sf::Color(100, 100, 100, 200));
    m_player1HealthBarBackground.setOutlineColor(sf::Color::Black);
    m_player1HealthBarBackground.setOutlineThickness(2.0f);
    m_player1HealthBarGreen.setFillColor(sf::Color::Green);
    m_player1HealthBarRed.setFillColor(sf::Color::Red);
    m_player1HealthText.setFillColor(sf::Color::White);
    m_player2HealthBarBackground.setSize({UIConstants::HEALTH_BAR_WIDTH, UIConstants::HEALTH_BAR_HEIGHT});
    m_player2HealthBarBackground.setFillColor(sf::Color(100, 100, 100, 200));
    m_player2HealthBarBackground.setOutlineColor(sf::Color::Black);
    m_player2HealthBarBackground.setOutlineThickness(2.0f);
    m_player2HealthBarGreen.setFillColor(sf::Color::Green);
    m_player2HealthBarRed.setFillColor(sf::Color::Red);
    m_player2HealthText.setFillColor(sf::Color::White);
    m_ownershipBarBackground.setSize({UIConstants::OWNERSHIP_BAR_WIDTH, UIConstants::OWNERSHIP_BAR_HEIGHT});
    m_ownershipBarBackground.setFillColor(sf::Color(100, 100, 100, 200));
    m_ownershipBarBackground.setOutlineColor(sf::Color::Black);
    m_ownershipBarBackground.setOutlineThickness(2.0f);

    m_player1OwnershipBar.setFillColor(sf::Color::Blue);
    m_player2OwnershipBar.setFillColor(sf::Color::Red);
    m_player1OwnershipBar.setSize({0, UIConstants::OWNERSHIP_BAR_HEIGHT});
    m_player2OwnershipBar.setSize({0, UIConstants::OWNERSHIP_BAR_HEIGHT});

    m_player1PercentageText.setFillColor(sf::Color::White);
    m_player2PercentageText.setFillColor(sf::Color::White);

}

void UI_Elements::update(unsigned int windowWidth, unsigned int windowHeight,
                         const Player& player1, const Player& player2,
                         const Arena& arena) {
    m_player1HealthBarBackground.setPosition({UIConstants::HEALTH_BAR_PADDING, UIConstants::HEALTH_BAR_PADDING});
    m_player1HealthBarGreen.setPosition(m_player1HealthBarBackground.getPosition());
    m_player1HealthBarRed.setPosition(m_player1HealthBarBackground.getPosition());

    float p1HealthPercentage = static_cast<float>(player1.getHealth()) / 100.0f;
    p1HealthPercentage = std::clamp(p1HealthPercentage, 0.0f, 1.0f);

    m_player1HealthBarGreen.setSize({UIConstants::HEALTH_BAR_WIDTH * p1HealthPercentage, UIConstants::HEALTH_BAR_HEIGHT});
    m_player1HealthBarRed.setSize({UIConstants::HEALTH_BAR_WIDTH * (1.0f - p1HealthPercentage), UIConstants::HEALTH_BAR_HEIGHT});
    m_player1HealthBarRed.setPosition({
        m_player1HealthBarBackground.getPosition().x + m_player1HealthBarGreen.getSize().x,
        m_player1HealthBarBackground.getPosition().y
    });

    sf::FloatRect p1HealthTextBounds = m_player1HealthText.getLocalBounds();
    m_player1HealthText.setOrigin({p1HealthTextBounds.position.x + p1HealthTextBounds.size.x / 2.0f,
                                  p1HealthTextBounds.position.y + p1HealthTextBounds.size.y / 2.0f});
    m_player1HealthText.setPosition({
        m_player1HealthBarBackground.getPosition().x + UIConstants::HEALTH_BAR_WIDTH / 2.0f,
        m_player1HealthBarBackground.getPosition().y + UIConstants::HEALTH_BAR_HEIGHT + UIConstants::HEALTH_TEXT_OFFSET_Y + p1HealthTextBounds.size.y / 2.0f
    });
    m_player2HealthBarBackground.setPosition({windowWidth - UIConstants::HEALTH_BAR_WIDTH - UIConstants::HEALTH_BAR_PADDING, UIConstants::HEALTH_BAR_PADDING});
    m_player2HealthBarGreen.setPosition(m_player2HealthBarBackground.getPosition());
    m_player2HealthBarRed.setPosition(m_player2HealthBarBackground.getPosition());

    float p2HealthPercentage = static_cast<float>(player2.getHealth()) / 100.0f;
    p2HealthPercentage = std::clamp(p2HealthPercentage, 0.0f, 1.0f);

    m_player2HealthBarGreen.setSize({UIConstants::HEALTH_BAR_WIDTH * p2HealthPercentage, UIConstants::HEALTH_BAR_HEIGHT});
    m_player2HealthBarRed.setSize({UIConstants::HEALTH_BAR_WIDTH * (1.0f - p2HealthPercentage), UIConstants::HEALTH_BAR_HEIGHT});
    m_player2HealthBarRed.setPosition({
        m_player2HealthBarBackground.getPosition().x + m_player2HealthBarGreen.getSize().x,
        m_player2HealthBarBackground.getPosition().y
    });

    sf::FloatRect p2HealthTextBounds = m_player2HealthText.getLocalBounds();
    m_player2HealthText.setOrigin({p2HealthTextBounds.position.x + p2HealthTextBounds.size.x / 2.0f, p2HealthTextBounds.position.y + p2HealthTextBounds.size.y / 2.0f});
    m_player2HealthText.setPosition({
        m_player2HealthBarBackground.getPosition().x + UIConstants::HEALTH_BAR_WIDTH / 2.0f,
        m_player2HealthBarBackground.getPosition().y + UIConstants::HEALTH_BAR_HEIGHT + UIConstants::HEALTH_TEXT_OFFSET_Y + p2HealthTextBounds.size.y / 2.0f
    });

    m_ownershipBarBackground.setPosition({(static_cast<float>(windowWidth) / 2.0f) - (UIConstants::OWNERSHIP_BAR_WIDTH / 2.0f), UIConstants::OWNERSHIP_BAR_PADDING_TOP});

    std::map<int, int> tileCounts = arena.getPlayerTileCounts();
    int p1Owned = tileCounts.count(player1.getPlayerID()) ? tileCounts.at(player1.getPlayerID()) : 0;
    int p2Owned = tileCounts.count(player2.getPlayerID()) ? tileCounts.at(player2.getPlayerID()) : 0;
    int totalPlayableTiles = arena.GetGridSize()*arena.GetGridSize();

    float p1OwnPercent = (totalPlayableTiles > 0) ? (static_cast<float>(p1Owned) / totalPlayableTiles) : 0.0f;
    float p2OwnPercent = (totalPlayableTiles > 0) ? (static_cast<float>(p2Owned) / totalPlayableTiles) : 0.0f;
    p1OwnPercent = std::clamp(p1OwnPercent, 0.0f, 1.0f);
    p2OwnPercent = std::clamp(p2OwnPercent, 0.0f, 1.0f);

    float p1BarWidth = UIConstants::OWNERSHIP_BAR_WIDTH * p1OwnPercent;
    float p2BarWidth = UIConstants::OWNERSHIP_BAR_WIDTH * p2OwnPercent;

    m_player1OwnershipBar.setSize({p1BarWidth, UIConstants::OWNERSHIP_BAR_HEIGHT});
    m_player2OwnershipBar.setSize({p2BarWidth, UIConstants::OWNERSHIP_BAR_HEIGHT});

    m_player1OwnershipBar.setOrigin({0.0f, 0.0f});
    m_player1OwnershipBar.setPosition(m_ownershipBarBackground.getPosition());

    m_player2OwnershipBar.setOrigin({0.0f, 0.0f});
    m_player2OwnershipBar.setPosition({m_ownershipBarBackground.getPosition().x + UIConstants::OWNERSHIP_BAR_WIDTH-p2BarWidth,
                                       m_ownershipBarBackground.getPosition().y});

    m_player1PercentageText.setString(std::to_string(static_cast<int>(p1OwnPercent * 100)) + "%");
    m_player2PercentageText.setString(std::to_string(static_cast<int>(p2OwnPercent * 100)) + "%");

    sf::FloatRect p1PercentTextBounds = m_player1PercentageText.getLocalBounds();
    m_player1PercentageText.setOrigin({p1PercentTextBounds.position.x + p1PercentTextBounds.size.x / 2.0f, p1PercentTextBounds.position.y + p1PercentTextBounds.size.y / 2.0f});
    if (p1BarWidth > p1PercentTextBounds.size.x + 10.f) {
        m_player1PercentageText.setPosition({m_player1OwnershipBar.getPosition().x + p1BarWidth / 2.0f,
                                          m_player1OwnershipBar.getPosition().y + UIConstants::OWNERSHIP_BAR_HEIGHT / 2.0f});
    } else {
        m_player1PercentageText.setString("");
    }

    sf::FloatRect p2PercentTextBounds = m_player2PercentageText.getLocalBounds();
    m_player2PercentageText.setOrigin({p2PercentTextBounds.position.x + p2PercentTextBounds.size.x / 2.0f, p2PercentTextBounds.position.y + p2PercentTextBounds.size.y / 2.0f});
    if (p2BarWidth > p2PercentTextBounds.size.x + 10.f) {
        m_player2PercentageText.setPosition({m_player2OwnershipBar.getPosition().x - p2BarWidth / 2.0f,
                                          m_player2OwnershipBar.getPosition().y + UIConstants::OWNERSHIP_BAR_HEIGHT / 2.0f});
    } else {
        m_player2PercentageText.setString("");
    }


}
void UI_Elements::draw(sf::RenderWindow& window) {
    window.draw(m_player1HealthBarBackground);
    window.draw(m_player1HealthBarRed);
    window.draw(m_player1HealthBarGreen);
    window.draw(m_player1HealthText);

    window.draw(m_player2HealthBarBackground);
    window.draw(m_player2HealthBarRed);
    window.draw(m_player2HealthBarGreen);
    window.draw(m_player2HealthText);

    window.draw(m_ownershipBarBackground);
    window.draw(m_player1OwnershipBar);
    window.draw(m_player2OwnershipBar);
    if (!m_player1PercentageText.getString().isEmpty()) window.draw(m_player1PercentageText);
    if (!m_player2PercentageText.getString().isEmpty()) window.draw(m_player2PercentageText);

}