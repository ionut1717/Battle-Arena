#include "Game.h"
#include "Arena/Tiles/Tiles.h"
#include "Player.h"
#include "Balloon.h"
#include "Resource_Manager.h"
#include "Arena/Tiles/Super_Tile.h"

namespace  {
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

Game_Engine::Game_Engine() :
    rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
    coordDist(0, 0),
    gameFont(Resource_Manager<sf::Font>::Instance().getResource("../Assets/Font.ttf")),
    uiHandler(gameFont){
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

    player.setStuck(false,1);
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

                if (manual_intersects) {
                    sf::Vector2i currentTileIdx = {x, y};
                    bool enteredThisTile = (player.getLastGridPosition() != currentTileIdx);

                    switch (tile.getSpecialType()) {
                        case TileSpecialType::STICKY: {
                            std::unique_ptr<StickyTile> stickyTile=std::make_unique<StickyTile>(dynamic_cast<StickyTile&>(tile));
                            float slow_factor=stickyTile->getslowfactor();
                            player.setStuck(true,slow_factor);
                            currentIntersectedTile = &tile;
                            break;
                        }
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
                        case TileSpecialType::SUPER: {
                            std::unique_ptr<SuperTile> supertile=std::make_unique<SuperTile>(dynamic_cast<SuperTile&>(tile));
                            float slowfactor,damage_factor;
                            supertile->getData(slowfactor,damage_factor);
                            player.setStuck(true,slowfactor);
                            player.setOnDamageTile(true);
                            currentIntersectedTile = &tile;
                            if (enteredThisTile) {
                                player.getDamageTile(damage_factor);
                                player.restartDamageTickClock();
                            } else {
                                if (player.getDamageElapsedTime().asSeconds() >= player.getDamageTickRate()) {
                                    player.takeContinuousDamage();
                                    player.restartDamageTickClock();
                                }
                            }
                            break;
                        }
                        case TileSpecialType::NONE:
                        default:
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
    uiHandler.update(windowWidth, windowHeight, player1, player2, arena);
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
        }
        if (player2Owned >= winTileThreshold) {
            return 2;
        }
    }

    if (player1.getHealth() == 0 && player2.getHealth() > 0) {
        return 2;
    }
    if (player2.getHealth() == 0 && player1.getHealth() > 0) {
        return 1;
    }
    if (player1.getHealth() == 0 && player2.getHealth() == 0) {
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

    sf::Texture* defaultTileTexture_try = nullptr;
    sf::Texture* playerTexture1_try = nullptr;
    sf::Texture* playerTexture2_try = nullptr;
    Arena* arena_try = nullptr;
    try {
        sf::Texture& tempDefaultTileTexture = Resource_Manager<sf::Texture>::Instance().getResource("../Assets/sprite_tile2.jpg");
        sf::Texture& tempPlayerTexture1 = Resource_Manager<sf::Texture>::Instance().getResource("../Assets/hero.png");
        sf::Texture& tempPlayerTexture2 = Resource_Manager<sf::Texture>::Instance().getResource("../Assets/base_character.png");

        defaultTileTexture_try = &tempDefaultTileTexture;
        playerTexture1_try = &tempPlayerTexture1;
        playerTexture2_try = &tempPlayerTexture2;

        Arena& tempArena = Arena::getInstance(*defaultTileTexture_try);
        arena_try = &tempArena;
    }catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;

    }
    catch (...) {
        std::cerr<<"Unknown error!"<<std::endl;
    }

    sf::Texture& playerTexture1 = *playerTexture1_try;
    sf::Texture& playerTexture2 = *playerTexture2_try;
    Arena& arena = *arena_try;
    std::vector<std::vector<std::unique_ptr<Tile>>>& grid = arena.GetGrid();
    float p1StartX = tileSize * 2.0f;
    float p1StartY = tileSize * 2.0f;
    float p2StartX = static_cast<float>(gridSize * tileSize) - (tileSize * 2.0f);
    float p2StartY = static_cast<float>(gridSize * tileSize) - (tileSize * 2.0f);

    Player* player1 = Player::acquirePlayer(1, sf::Color::Blue, {p1StartX, p1StartY}, true, &playerTexture1);
    Player* player2 = Player::acquirePlayer(2, sf::Color::Cyan, {p2StartX, p2StartY}, false, &playerTexture2);

    auto resetGameState = [&]() {
        if (player1) Player::releasePlayer(player1);
        if (player2) Player::releasePlayer(player2);
        player1 = Player::acquirePlayer(1, sf::Color::Blue, {p1StartX, p1StartY}, true, &playerTexture1);
        player2 = Player::acquirePlayer(2, sf::Color::Cyan, {p2StartX, p2StartY}, false, &playerTexture2);

        activeBalloons.clear();
        gameEnded = false;
        currentWinMessage.clear();
        gameClock.restart();
        spawnClock.restart();
    };

    resetGameState();

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
                    if (player1) player1->setKeyPressed(keyPressed->code, true);
                    if (player2) player2->setKeyPressed(keyPressed->code, true);

                    if (keyPressed->code == sf::Keyboard::Key::Z) { if (player1ZoomFactor > MIN_ZOOM_FACTOR) { player1ZoomFactor *= ZOOM_SPEED; view1.zoom(ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::X) { if (player1ZoomFactor < MAX_ZOOM_FACTOR) { player1ZoomFactor /= ZOOM_SPEED; view1.zoom(1.0f / ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::O) { if (player2ZoomFactor > MIN_ZOOM_FACTOR) { player2ZoomFactor *= ZOOM_SPEED; view2.zoom(ZOOM_SPEED); } }
                    if (keyPressed->code == sf::Keyboard::Key::P) { if (player2ZoomFactor < MAX_ZOOM_FACTOR) { player2ZoomFactor /= ZOOM_SPEED; view2.zoom(1.0f / ZOOM_SPEED); } }

                    if (keyPressed->code == sf::Keyboard::Key::E) {
                        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_COOLDOWN && player1 && !player1->isEliminatedPlayer()) {
                            sf::Vector2f launchDirection = player1->getLaunchDirection();
                            sf::Vector2f spawnPosition = player1->getPosition() + (launchDirection * SPAWN_OFFSET_DISTANCE);
                            activeBalloons.emplace_back(player1->getColor(), spawnPosition,player1->getPlayerID());
                            if (!activeBalloons.empty()) {
                                activeBalloons.back().setTarget(player2);
                                activeBalloons.back().launch(launchDirection * INITIAL_LAUNCH_FORCE);
                            }
                            spawnClock.restart();
                        }
                    }
                    if (keyPressed->code == sf::Keyboard::Key::RShift) {
                        if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_COOLDOWN && player2 && !player2->isEliminatedPlayer()) {
                            sf::Vector2f launchDirection = player2->getLaunchDirection();
                            sf::Vector2f spawnPosition = player2->getPosition() + (launchDirection * SPAWN_OFFSET_DISTANCE);
                            activeBalloons.emplace_back(player2->getColor(), spawnPosition, player2->getPlayerID());
                            if (!activeBalloons.empty()) {
                                activeBalloons.back().setTarget(player1);
                                activeBalloons.back().launch(launchDirection * INITIAL_LAUNCH_FORCE);
                            }
                            spawnClock.restart();
                        }
                    }
                    if (keyPressed->code == sf::Keyboard::Key::Space) { if(player1 && !player1->isEliminatedPlayer()) player1->paintNearbyTiles(grid, PAINT_RADIUS); }
                    if (keyPressed->code == sf::Keyboard::Key::RControl) { if(player2 && !player2->isEliminatedPlayer()) player2->paintNearbyTiles(grid, PAINT_RADIUS); }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) window.close();
                if (gameEnded && keyPressed->code == sf::Keyboard::Key::Enter) {
                    resetGameState();
                    player1ZoomFactor = 1.0f;
                    player2ZoomFactor = 1.0f;
                    if (player1) view1.setSize({viewHalfWidth, viewHeight}); view1.setCenter(player1->getPosition());
                    if (player2) view2.setSize({viewHalfWidth, viewHeight}); view2.setCenter(player2->getPosition());
                }
            }
            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (!gameEnded) {
                    if (player1) player1->setKeyPressed(keyReleased->code, false);
                    if (player2) player2->setKeyPressed(keyReleased->code, false);
                }
            }
        }

        if (!gameEnded) {
            if(player1 && !player1->isEliminatedPlayer()) player1->updatePosition(dtSeconds);
            if(player2 && !player2->isEliminatedPlayer()) player2->updatePosition(dtSeconds);
            for (auto& balloon : activeBalloons) { balloon.update(dtSeconds); }
            if(player1 && !player1->isEliminatedPlayer()) processPlayerTileInteraction(*player1, dtSeconds, grid);
            if(player2 && !player2->isEliminatedPlayer()) processPlayerTileInteraction(*player2, dtSeconds, grid);
            activeBalloons.erase(std::remove_if(activeBalloons.begin(), activeBalloons.end(),
                                               [&](AttackBalloon& balloon) {
                                                   bool hit = false;
                                                   Player* targetPlayer = const_cast<Player*>(balloon.getTarget());

                                                   if (targetPlayer && !targetPlayer->isEliminatedPlayer()){
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
            checkWinCondition(*player1, *player2, arena);
        }


        if (player1 && !player1->isEliminatedPlayer()) view1.setCenter(player1->getPosition());
        else view1.setCenter({Arena::GetGridSize() * Tile::getSize() / 4.f, Arena::GetGridSize() * Tile::getSize() / 2.f});

        if (player2 && !player2->isEliminatedPlayer()) view2.setCenter(player2->getPosition());
        else view2.setCenter({Arena::GetGridSize() * Tile::getSize() * 3.f / 4.f,
                             Arena::GetGridSize() * Tile::getSize() / 2.f
        });

        window.clear(sf::Color::White);
        int caz=checkWinCondition(*player1,*player2,arena);
        if (caz==1) {
            gameEnded = true;
            currentWinMessage = "Player 1 Wins!";
        }
        if (caz==2) {
            gameEnded = true;
            currentWinMessage = "Player 2 Wins!";
        }
        if (caz==0) {
            gameEnded = true;
            currentWinMessage = "Draw - Both Eliminated";
        }
        if (caz==-1) {
            window.setView(view1);
            arena.draw(window, player1->getPlayerID());

            if(player1) player1->draw(window);
            if(player2) player2->draw(window);
            for (const auto& balloon : activeBalloons) { window.draw(balloon); }

            window.setView(view2);
            arena.draw(window, player2->getPlayerID());
            if(player1) player1->draw(window);
            if(player2) player2->draw(window);
            for (const auto& balloon : activeBalloons) { window.draw(balloon); }


            window.setView(window.getDefaultView());
            window.draw(dividingLine);
            updateUIDelegator(windowWidth, windowHeight, *player1, *player2, arena);
            drawUIDelegator(window);

        } else {
            std::cout<<currentWinMessage<<std::endl;
            window.close();
        }
        window.display();
    }
    if (player1) Player::releasePlayer(player1);
    if (player2) Player::releasePlayer(player2);
}