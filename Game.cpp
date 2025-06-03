#include "Game.h"
#include "Arena/Tiles/Tiles.h" // Rămâne necesar pentru Tile::getSize etc.
#include "Player.h"
#include "Balloon.h"
#include "Resource_Manager.h"
// UI_Elements.h este deja inclus din Game.h (corect)

namespace {
    constexpr float INITIAL_LAUNCH_FORCE = 1500.0f;
    constexpr float SPAWN_OFFSET_DISTANCE = 30.0f;
    constexpr float SPAWN_COOLDOWN = 1.0f;
    constexpr float DIVIDING_LINE_WIDTH = 4.0f;
    constexpr int PAINT_RADIUS = 3;
    constexpr float ZOOM_SPEED = 0.9f;
    constexpr float MIN_ZOOM_FACTOR = 0.5;
    constexpr float MAX_ZOOM_FACTOR = 1;
}

Game_Engine& Game_Engine::Instance() {
    static Game_Engine instance;
    return instance;
}

// Constructor Corectat:
Game_Engine::Game_Engine() :
    rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
    coordDist(0, 0), // Va fi actualizat în GameLoop
    gameFont(Resource_Manager::Instance().getFont("../Assets/Font.ttf")), // Încarcă fontul aici
    uiHandler(gameFont), // Pasează fontul la constructorul uiHandler
    gameEnded(false),
    currentWinMessage("") {
}

void Game_Engine::processPlayerTileInteraction(Player& player, float dt, std::vector<std::vector<std::unique_ptr<Tile>>>& grid) {
    sf::FloatRect playerBounds = player.getBounds();
    int gridSize = Arena::GetGridSize();
    float tileSize = Tile::getSize();

    int minTileX = static_cast<int>(playerBounds.position.x / tileSize);
    int maxTileX = static_cast<int>((playerBounds.position.x + playerBounds.size.x) / tileSize);
    int minTileY = static_cast<int>(playerBounds.position.y / tileSize);
    int maxTileY = static_cast<int>((playerBounds.position.y + playerBounds.size.y) / tileSize);

    minTileX = std::max(0, minTileX);
    maxTileX = std::min(gridSize - 1, maxTileX);
    minTileY = std::max(0, minTileY);
    maxTileY = std::min(gridSize - 1, maxTileY);

    player.setStuck(false);
    player.setOnDamageTile(false);

    Tile* currentIntersectedTile = nullptr;

    for (int y = minTileY; y <= maxTileY; ++y) {
        for (int x = minTileX; x <= maxTileX; ++x) {

            if (y >= 0 && static_cast<size_t>(y) < grid.size() && x >= 0 && static_cast<size_t>(x) < grid[y].size()) {
                Tile& tile = *grid[y][x];
                sf::FloatRect tileBounds = tile.getBounds();
                bool manual_intersects =
                    playerBounds.position.x < tileBounds.position.x + tileBounds.size.x &&
                    playerBounds.position.x + playerBounds.size.x > tileBounds.position.x &&
                    playerBounds.position.y < tileBounds.position.y + tileBounds.size.y &&
                    playerBounds.position.y + playerBounds.size.y > tileBounds.position.y;

                if (manual_intersects) { // SFML 3.0 intersects
                    sf::Vector2i currentTileIdx = {x, y};
                    bool enteredThisTile = (player.getLastGridPosition() != currentTileIdx);

                    switch (tile.getSpecialType()) {
                        case TileSpecialType::STICKY:
                            player.setStuck(true);
                            currentIntersectedTile = &tile;
                            break;
                        case TileSpecialType::DAMAGE:
                            player.setOnDamageTile(true);
                            currentIntersectedTile = &tile;
                            if (enteredThisTile) {
                                player.takeInitialDamage();
                                player.restartDamageTickClock();
                            } else {
                                if (player.getDamageElapsedTime().asSeconds() >= player.getDamageTickRate()) {
                                    player.takeContinuousDamage();
                                    player.restartDamageTickClock();
                                }
                            }
                            break;
                        case TileSpecialType::TELEPORTER:
                            currentIntersectedTile = &tile;
                            if (enteredThisTile) {
                                    int newX = coordDist(rng);
                                    int newY = coordDist(rng);
                                    sf::Vector2f newPos = {static_cast<float>(newX * tileSize + tileSize / 2.0f),
                                                           static_cast<float>(newY * tileSize + tileSize / 2.0f)};
                                    player.setPosition(newPos);
                                    player.setLastGridPosition({-1,-1});
                            }
                            break;
                        case TileSpecialType::HEALING:
                             currentIntersectedTile = &tile;
                            if (tile.getOwner() == player.getPlayerID() && enteredThisTile) {
                                player.heal(2);
                            }
                            break;
                        case TileSpecialType::SUPER:
                            currentIntersectedTile = &tile;
                            player.setStuck(true);
                            player.setOnDamageTile(true);
                            currentIntersectedTile = &tile;
                            if (enteredThisTile) {
                                player.takeInitialDamage();
                                player.restartDamageTickClock();
                            } else {
                                if (player.getDamageElapsedTime().asSeconds() >= player.getDamageTickRate()) {
                                    player.takeContinuousDamage();
                                    player.restartDamageTickClock();
                                }
                            }
                            break;
                        case TileSpecialType::NONE:
                        default:
                            // Nimic special
                            break;
                    }
                     if (currentIntersectedTile) player.setLastGridPosition(currentTileIdx);
                }
            }
        }
        if (currentIntersectedTile) {
            break;
        }
    }
     if (!currentIntersectedTile && player.getIsOnDamageTile()) {
        player.setOnDamageTile(false);
    }
}


void Game_Engine::updateUIDelegator(unsigned int windowWidth, unsigned int windowHeight, Player& player1, Player& player2, const Arena& arena) {
    uiHandler.update(windowWidth, windowHeight, player1, player2, arena, gameEnded, currentWinMessage);
}

void Game_Engine::drawUIDelegator(sf::RenderWindow& window) {
    uiHandler.draw(window);
}

int Game_Engine::checkWinCondition(Player& player1, Player& player2, const Arena& arena) {

    std::map<int, int> tileCounts = arena.getPlayerTileCounts();
    int player1Owned = tileCounts.count(player1.getPlayerID()) ? tileCounts.at(player1.getPlayerID()) : 0;
    int player2Owned = tileCounts.count(player2.getPlayerID()) ? tileCounts.at(player2.getPlayerID()) : 0;
    int totalPlayableTiles = arena.GetGridSize()* arena.GetGridSize();

    std::string winnerTitle;

    if (totalPlayableTiles > 0) {
        int winTileThreshold = (totalPlayableTiles / 2) + 1;
        if (player1Owned >= winTileThreshold) {
            return 1;
        } else if (player2Owned >= winTileThreshold) {
            return 2;
        }
    }

    if (player1.getHealth() == 0 && player2.getHealth() > 0) {
        return 2;
    } else if (player2.getHealth() == 0 && player1.getHealth() > 0) {
        return 1;
    } else if (player1.getHealth() == 0 && player2.getHealth() == 0) {
        return 0;
    }
    return -1;
}

void Game_Engine::GameLoop() {
    int gridSize = Arena::GetGridSize();
    float tileSize = Tile::getSize();
    coordDist = std::uniform_int_distribution<int>(0, gridSize - 1);

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    if (!desktopMode.isValid()) {
        desktopMode = sf::VideoMode({1280, 720});
    }
    sf::RenderWindow window(desktopMode, "Battle Arena (Split Screen)");
    unsigned int windowWidth = window.getSize().x;
    unsigned int windowHeight = window.getSize().y;
    window.setVerticalSyncEnabled(true);

    sf::Texture& defaultTileTexture = Resource_Manager::Instance().getTexture("../Assets/sprite_tile.jpg");
    sf::Texture& playerTexture1 = Resource_Manager::Instance().getTexture("../Assets/hero.png");
    sf::Texture& playerTexture2 = Resource_Manager::Instance().getTexture("../Assets/base_character.png");
    Arena& arena = Arena::getInstance(defaultTileTexture);
    std::vector<std::vector<std::unique_ptr<Tile>>>& grid = arena.GetGrid();
    float p1StartX = tileSize * 2.0f;
    float p1StartY = tileSize * 2.0f;
    float p2StartX = static_cast<float>(gridSize * tileSize) - (tileSize * 2.0f);
    float p2StartY = static_cast<float>(gridSize * tileSize) - (tileSize * 2.0f);


    Player player1(1, sf::Color::Blue, {p1StartX, p1StartY}, true, playerTexture1);
    Player player2(2, sf::Color::Cyan, {p2StartX, p2StartY}, false, playerTexture2);

    // Starea inițială a jocului
    auto resetGameState = [&]() {
        player1.resetHealthAndState();
        player2.resetHealthAndState();
        player1.setPosition({p1StartX, p1StartY});
        player2.setPosition({p2StartX, p2StartY});
        activeBalloons.clear();
        gameEnded = false;
        currentWinMessage.clear();
        gameClock.restart();
        spawnClock.restart();
    };

    resetGameState(); // Setează starea inițială


    sf::View view1, view2;
    float viewHalfWidth = static_cast<float>(windowWidth) / 2.0f;
    float viewHeight = static_cast<float>(windowHeight);

    view1.setSize({viewHalfWidth, viewHeight});
    view1.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}));
    view2.setSize({viewHalfWidth, viewHeight});
    view2.setViewport(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}));

    float player1ZoomFactor = 1.0f;
    float player2ZoomFactor = 1.0f;

    sf::RectangleShape dividingLine;
    dividingLine.setSize({DIVIDING_LINE_WIDTH, static_cast<float>(windowHeight)});
    dividingLine.setFillColor(sf::Color::Black);
    dividingLine.setPosition({static_cast<float>(windowWidth / 2.0f - DIVIDING_LINE_WIDTH / 2.0f), 0.0f});

    while (window.isOpen()) {
        sf::Time deltaTime = gameClock.restart();
        float dtSeconds = deltaTime.asSeconds();

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (!gameEnded) {
                    player1.setKeyPressed(keyPressed->code, true);
                    player2.setKeyPressed(keyPressed->code, true);

                    if (keyPressed->code == sf::Keyboard::Key::Z) { if (player1ZoomFactor > MIN_ZOOM_FACTOR) { player1ZoomFactor *= ZOOM_SPEED; view1.zoom(ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::X) { if (player1ZoomFactor < MAX_ZOOM_FACTOR) { player1ZoomFactor /= ZOOM_SPEED; view1.zoom(1.0f / ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::O) { if (player2ZoomFactor > MIN_ZOOM_FACTOR) { player2ZoomFactor *= ZOOM_SPEED; view2.zoom(ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::P) { if (player2ZoomFactor < MAX_ZOOM_FACTOR) { player2ZoomFactor /= ZOOM_SPEED; view2.zoom(1.0f / ZOOM_SPEED); } }

                    if (keyPressed->code == sf::Keyboard::Key::E) {
                        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_COOLDOWN && !player1.isEliminatedPlayer()) {
                            sf::Vector2f launchDirection = player1.getLaunchDirection();
                            sf::Vector2f spawnPosition = player1.getPosition() + (launchDirection * SPAWN_OFFSET_DISTANCE);
                            activeBalloons.emplace_back(player1.getColor(), spawnPosition,player1.getPlayerID()); // Adaugă ownerID
                            if (!activeBalloons.empty()) {
                                activeBalloons.back().setTarget(&player2);
                                activeBalloons.back().launch(launchDirection * INITIAL_LAUNCH_FORCE);
                            }
                            spawnClock.restart();
                        }
                    }
                    if (keyPressed->code == sf::Keyboard::Key::RShift) {
                        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_COOLDOWN && !player2.isEliminatedPlayer()) {
                            sf::Vector2f launchDirection = player2.getLaunchDirection();
                            sf::Vector2f spawnPosition = player2.getPosition() + (launchDirection * SPAWN_OFFSET_DISTANCE);
                            activeBalloons.emplace_back(player2.getColor(), spawnPosition, player2.getPlayerID()); // Adaugă ownerID
                            if (!activeBalloons.empty()) {
                                activeBalloons.back().setTarget(&player1);
                                activeBalloons.back().launch(launchDirection * INITIAL_LAUNCH_FORCE);
                            }
                            spawnClock.restart();
                        }
                    }
                    if (keyPressed->code == sf::Keyboard::Key::Space) { if(!player1.isEliminatedPlayer()) player1.paintNearbyTiles(grid, PAINT_RADIUS); }
                    if (keyPressed->code == sf::Keyboard::Key::RControl) { if(!player2.isEliminatedPlayer()) player2.paintNearbyTiles(grid, PAINT_RADIUS); }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) window.close();
                if (gameEnded && keyPressed->code == sf::Keyboard::Key::Enter) {
                    resetGameState(); // Resetează starea jocului
                    // Reset view zoom factors and centers
                    player1ZoomFactor = 1.0f;
                    player2ZoomFactor = 1.0f;
                    view1.setSize({viewHalfWidth, viewHeight}); view1.setCenter(player1.getPosition());
                    view2.setSize({viewHalfWidth, viewHeight}); view2.setCenter(player2.getPosition());
                }
            }
            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (!gameEnded) {
                    player1.setKeyPressed(keyReleased->code, false);
                    player2.setKeyPressed(keyReleased->code, false);
                }
            }
        }

        if (!gameEnded) {
            if(!player1.isEliminatedPlayer()) player1.updatePosition(dtSeconds);
            if(!player2.isEliminatedPlayer()) player2.updatePosition(dtSeconds);
            for (auto& balloon : activeBalloons) { balloon.update(dtSeconds); }
            if(!player1.isEliminatedPlayer()) processPlayerTileInteraction(player1, dtSeconds, grid);
            if(!player2.isEliminatedPlayer()) processPlayerTileInteraction(player2, dtSeconds, grid);
            activeBalloons.erase(std::remove_if(activeBalloons.begin(), activeBalloons.end(),
                                               [&](AttackBalloon& balloon) {
                                                   bool hit = false;
                                                   Player* targetPlayer = const_cast<Player*>(balloon.getTarget());

                                                   if (targetPlayer && !targetPlayer->isEliminatedPlayer()){
                                                       // Verifică dacă balonul nu aparține țintei
                                                       if (balloon.getOwnerID() != targetPlayer->getPlayerID()) {
                                                           sf::Vector2f dist = balloon.getPosition() - targetPlayer->getPosition();
                                                           float collisionDistanceSq = (balloon.getRadius() + targetPlayer->getRadius()) * (balloon.getRadius() + targetPlayer->getRadius());
                                                           if ((dist.x * dist.x + dist.y * dist.y) < collisionDistanceSq) {
                                                                targetPlayer->getDamageBalloon();
                                                                hit = true;
                                                           }
                                                       }
                                                   }
                                                   return hit || balloon.isExpired();
                                               }), activeBalloons.end());
            checkWinCondition(player1, player2, arena);
        }




        if (!player1.isEliminatedPlayer()) view1.setCenter(player1.getPosition());
        else view1.setCenter({Arena::GetGridSize() * Tile::getSize() / 4.f, Arena::GetGridSize() * Tile::getSize() / 2.f}); // Centru aproximativ al jumătății stângi

        if (!player2.isEliminatedPlayer()) view2.setCenter(player2.getPosition());
        else view2.setCenter({Arena::GetGridSize() * Tile::getSize() * 3.f / 4.f,
                             Arena::GetGridSize() * Tile::getSize() / 2.f
        });

        // Desenare pentru View 1
        window.clear(sf::Color::White);
        int caz=Game_Engine::checkWinCondition(player1,player2,arena);
        if (caz==1) {
            std::cout<<"Player 1 wins!"<<std::endl;
            break;

        }
        else if (caz==2) {
            std::cout<<"Player 2 wins!"<<std::endl;
            break;
        }
        else if (caz==0) {
            std::cout<<"Draw"<<std::endl;
            break;

        }
        else if (caz==-1) {
            window.setView(view1);
            arena.draw(window, player1.getPlayerID());
            // Desenează ambii playeri în view-ul 1
            player1.draw(window);
            player2.draw(window);
            for (const auto& balloon : activeBalloons) { window.draw(balloon); }

            // Desenare pentru View 2
            window.setView(view2);
            arena.draw(window, player2.getPlayerID());
            player1.draw(window);
            player2.draw(window);
            for (const auto& balloon : activeBalloons) { window.draw(balloon); }


            window.setView(window.getDefaultView());// Revenim la view-ul implicit pentru UI și linia de divizare
            window.draw(dividingLine);
            updateUIDelegator(windowWidth, windowHeight, player1, player2, arena);
            drawUIDelegator(window);

        }
        window.display();
    }
}