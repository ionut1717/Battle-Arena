#include "Game.h"
#include "Tiles.h" // Needed for Tile::getSize() and TileSpecialType enum in the cpp file
#include "Arena.h" // Needed for Arena::GetGridSize() and Arena class in the cpp file
// Resource_Manager.h is included via Game.h, so no need for explicit include here.


// Definition of the static Instance() method for the singleton
Game_Engine& Game_Engine::Instance() {
    static Game_Engine instance; // The single instance is created here, once.
    return instance;
}

// Protected constructor for the singleton Game_Engine
// Initialize member sf::Text objects here.
Game_Engine::Game_Engine()
    : player1PercentageText(), player2PercentageText(), totalTilesText() // Explicitly call default constructors
{
    rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // --- Initialize UI Elements that need resources or specific setup ---
    try {
        // Load the font using the Resource_Manager
        // Assuming your font file is in 'assets/Font.ttf'
        sf::Font& loadedFont = Resource_Manager::Instance().load<sf::Font>("assets/Font.ttf");

        // Set the font for your sf::Text members
        player1PercentageText.setFont(loadedFont);
        player2PercentageText.setFont(loadedFont);
        totalTilesText.setFont(loadedFont);

        // Set initial character sizes and fill colors (can be modified later in GameLoop)
        player1PercentageText.setCharacterSize(14);
        player1PercentageText.setFillColor(sf::Color::White);

        player2PercentageText.setCharacterSize(14);
        player2PercentageText.setFillColor(sf::Color::White);

        totalTilesText.setCharacterSize(10);
        totalTilesText.setFillColor(sf::Color::Black);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error initializing Game_Engine: " << e.what() << std::endl;
        // Optionally, you could set a default font or handle this error more gracefully.
        // For now, it will print an error and the game might run without text.
    }

    // Initialize sf::RectangleShapes (their sizes/positions will be set in GameLoop based on window size)
    ownershipBarBackground = sf::RectangleShape();
    player1OwnershipBar = sf::RectangleShape();
    player2OwnershipBar = sf::RectangleShape();
}


// Helper function to process player-tile interactions
void Game_Engine::processPlayerTileInteraction(Player& player, float dt, std::vector<std::vector<std::unique_ptr<Tile>>>& grid) {
    sf::FloatRect playerBounds = player.getBounds();

    int GridSize = Arena::GetGridSize(); // Get GridSize from Arena
    float TileSize = Tile::getSize(); // Get TileSize from Tile

    // Calculate the range of tiles to check for collision
    int minTileX = static_cast<int>(playerBounds.position.x / TileSize);
    int maxTileX = static_cast<int>((playerBounds.position.x + playerBounds.size.x) / TileSize);
    int minTileY = static_cast<int>(playerBounds.position.y / TileSize);
    int maxTileY = static_cast<int>((playerBounds.position.y + playerBounds.size.y) / TileSize);

    minTileX = std::max(0, minTileX);
    maxTileX = std::min(GridSize - 1, maxTileX);
    minTileY = std::max(0, minTileY);
    maxTileY = std::min(GridSize - 1, maxTileY);

    player.setStuck(false); // Reset stuck status for this frame
    player.setOnDamageTile(false); // Reset damage status for this frame

    Tile* currentIntersectedTile = nullptr; // Pointer to the tile currently intersecting with the player

    for (int y = minTileY; y <= maxTileY; ++y) {
        for (int x = minTileX; x <= maxTileX; ++x) {
            // Access the Tile by dereferencing unique_ptr
            Tile& tile = *grid[y][x];
            sf::FloatRect tileBounds = tile.getBounds();

            // AABB collision check
            bool intersects =
                playerBounds.position.x < tileBounds.position.x + tileBounds.size.x &&
                playerBounds.position.x + playerBounds.size.x > tileBounds.position.x &&
                playerBounds.position.y < tileBounds.position.y + tileBounds.size.y &&
                playerBounds.position.y + playerBounds.size.y > tileBounds.position.y;

            if (intersects) {
                sf::Vector2i currentTileIdx = {x, y};
                bool enteredThisTile = (player.getLastGridPosition().x != currentTileIdx.x ||
                                        player.getLastGridPosition().y != currentTileIdx.y);

                switch (tile.getSpecialType()) {
                    case STICKY:
                        player.setStuck(true);
                        currentIntersectedTile = &tile;
                        break;
                    case DAMAGE:
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
                    case TELEPORTER:
                        currentIntersectedTile = &tile;
                        if (enteredThisTile) {
                            // Teleport only if owned by the current player OR unowned
                            if (tile.getOwner() == player.GetPlayerID() || tile.getOwner() == -1) {
                                int newX = coordDist(rng); // Use rng and coordDist from Game_Engine
                                int newY = coordDist(rng);
                                sf::Vector2f newPos = {static_cast<float>(newX * TileSize + TileSize / 2.0f),
                                                       static_cast<float>(newY * TileSize + TileSize / 2.0f)};
                                player.setPosition(newPos);
                                std::cout << "Player " << player.GetPlayerID() << " teleportat la (" << newX << ", " << newY << ")" << std::endl;
                                player.setLastGridPosition({-1,-1}); // Reset to allow re-teleportation
                            }
                        }
                        break;
                    case HEALING:
                        if (tile.getOwner() == player.GetPlayerID() && enteredThisTile) {
                            // Example healing logic:
                            player.heal(10); // Assuming you have a heal() method in Player
                            std::cout << "Player " << player.GetPlayerID() << " a calcat pe un Healing Tile! Viata: " << player.getHealth() << std::endl;
                        }
                        currentIntersectedTile = &tile;
                        break;
                    case NONE:
                    default:
                        // Nothing special to do for normal tiles
                        break;
                }
            }
        }
        // If a special tile was found that the player is interacting with, no need to check other tiles
        if (currentIntersectedTile) {
            break;
        }
    }

    // Update lastGridPosition if the player is not on a special tile or has moved off one
    if (!currentIntersectedTile && player.getLastGridPosition().x != -1) {
        // If the player is no longer on a special tile, update their normal grid position
        sf::Vector2f playerCenter = player.getPosition();
        player.setLastGridPosition({
            static_cast<int>(playerCenter.x / TileSize),
            static_cast<int>(playerCenter.y / TileSize)
        });
    }
    // Update lastGridPosition if player is currently on a special tile
    if (currentIntersectedTile) {
        sf::Vector2f playerCenter = player.getPosition();
        player.setLastGridPosition({
            static_cast<int>(playerCenter.x / TileSize),
            static_cast<int>(playerCenter.y / TileSize)
        });
    }
}


void Game_Engine::GameLoop() {
    int GridSize = Arena::GetGridSize();
    float TileSize = Tile::getSize();

    // Initialize random number distribution after GridSize is known
    coordDist = std::uniform_int_distribution<int>(0, GridSize - 1); // Distribution for grid coordinates

    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Battle Arena (Split Screen)");
    unsigned int windowWidth = window.getSize().x;
    unsigned int windowHeight = window.getSize().y;
    window.setVerticalSyncEnabled(true);

    // --- Load textures using Resource_Manager ---
    // Assuming your textures are in 'assets/' directory
    sf::Texture& defaultTexture = Resource_Manager::Instance().load<sf::Texture>("assets/sprite_tile2.jpg");
    sf::Texture& PlayerTexture = Resource_Manager::Instance().load<sf::Texture>("assets/hero.png");

    Arena arena(defaultTexture);
    std::vector<std::vector<std::unique_ptr<Tile>>>& grid = arena.GetGrid();

    Player player1(1, sf::Color::Blue, {50.0f, 50.0f}, true, PlayerTexture);
    Player player2(2, sf::Color::Red, {static_cast<float>(GridSize * TileSize - 50.0f), 50.0f}, false, PlayerTexture);

    sf::View view1;
    float viewHalfWidth = static_cast<float>(windowWidth / 2.0f);
    float viewHeight = static_cast<float>(windowHeight);
    view1.setSize({viewHalfWidth, viewHeight});
    view1.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}));

    sf::View view2;
    view2.setSize({viewHalfWidth, viewHeight});
    view2.setViewport(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}));

    sf::RectangleShape dividingLine;
    float lineWidth = 4.0f;
    dividingLine.setSize({lineWidth, static_cast<float>(windowHeight)});
    dividingLine.setFillColor(sf::Color::Black);
    dividingLine.setPosition({static_cast<float>(windowWidth / 2.0f - lineWidth / 2.0f), 0.0f});

    // --- Initialize Ownership Bar ---
    float ownershipBarWidth = 300.0f; // Width of the entire ownership bar
    float ownershipBarHeight = 20.0f; // Height of the ownership bar
    float ownershipBarPaddingTop = 10.0f; // Padding from top edge

    ownershipBarBackground.setSize({ownershipBarWidth, ownershipBarHeight});
    ownershipBarBackground.setFillColor(sf::Color(100, 100, 100, 180)); // Grey background, semi-transparent
    ownershipBarBackground.setOutlineColor(sf::Color::Black);
    ownershipBarBackground.setOutlineThickness(2.0f);
    ownershipBarBackground.setPosition({(windowWidth / 2.0f) - (ownershipBarWidth / 2.0f), ownershipBarPaddingTop});

    player1OwnershipBar.setFillColor(player1.getColor());
    player2OwnershipBar.setFillColor(player2.getColor());
    player1OwnershipBar.setSize({0, ownershipBarHeight}); // Start with 0 width
    player2OwnershipBar.setSize({0, ownershipBarHeight}); // Start with 0 width

    // The font is already set in the Game_Engine constructor.
    // player1PercentageText.setFont(font); // NO LONGER NEEDED HERE
    // player2PercentageText.setFont(font); // NO LONGER NEEDED HERE
    // totalTilesText.setFont(font); // NO LONGER NEEDED HERE

    totalTilesText.setString("Total Tiles: " + std::to_string(arena.getTotalTiles()));
    totalTilesText.setPosition({ownershipBarBackground.getPosition().x + ownershipBarWidth / 2.0f - totalTilesText.getGlobalBounds().size.x / 2.0f,
                               ownershipBarBackground.getPosition().y + ownershipBarHeight + 2}); // Below the bar

    while (window.isOpen()) {
        sf::Time deltaTime = gameClock.restart();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                player1.set_key_pressed(keyPressed->code, true);
                player2.set_key_pressed(keyPressed->code, true);

                // Zoom logic
                if (keyPressed->code == sf::Keyboard::Key::Z) {
                    if (player1ZoomFactor > minZoom) { player1ZoomFactor *= zoomSpeed; view1.zoom(zoomSpeed); }
                }
                if (keyPressed->code == sf::Keyboard::Key::X) {
                    if (player1ZoomFactor < maxZoom) { player1ZoomFactor /= zoomSpeed; view1.zoom(1.0f / zoomSpeed); }
                }
                if (keyPressed->code == sf::Keyboard::Key::O) {
                    if (player2ZoomFactor > minZoom) { player2ZoomFactor *= zoomSpeed; view2.zoom(zoomSpeed); }
                }
                if (keyPressed->code == sf::Keyboard::Key::P) {
                    if (player2ZoomFactor < maxZoom) { player2ZoomFactor /= zoomSpeed; view2.zoom(1.0f / zoomSpeed); }
                }

                if (keyPressed->code == sf::Keyboard::Key::E) {
                    if (spawnClock.getElapsedTime().asSeconds() >= spawnCooldown) {
                        sf::Vector2f playerVelocity = player1.getVelocity();
                        sf::Vector2f launchDirection = playerVelocity;
                        float length = std::sqrt(launchDirection.x * launchDirection.x + launchDirection.y * launchDirection.y);

                        if (length == 0.0f) {
                            launchDirection = {0.0f, 1.0f}; // Fallback: if not moving, launch downwards
                        } else {
                            launchDirection /= length;
                        }

                        sf::Vector2f spawnPosition = player1.getPosition() + (launchDirection * spawnOffsetDistance);

                        activeBalloons.emplace_back(10.0f, sf::Color::Black, spawnPosition);
                        if (!activeBalloons.empty()) {
                            AttackBalloon& newBalloon = activeBalloons.back();
                            newBalloon.setTarget(&player2); // Use direct address of player2 object
                            newBalloon.launch(launchDirection * initialLaunchForce);
                        }
                        spawnClock.restart();
                    } else {
                        std::cout << "Cooldown activ. Mai asteapta "
                                  << spawnCooldown - spawnClock.getElapsedTime().asSeconds() << " secunde."
                                  << std::endl;
                    }
                }
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    player1.paintNearbyTiles(grid, 10);
                }
                if (keyPressed->code == sf::Keyboard::Key::RControl) {
                    player2.paintNearbyTiles(grid, 10);
                }

                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }

            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                player1.set_key_pressed(keyReleased->code, false);
                player2.set_key_pressed(keyReleased->code, false);
            }
        }

        // Update player positions
        player1.update_position(deltaTime.asSeconds());
        player2.update_position(deltaTime.asSeconds());

        // Update balloons
        for (auto& balloon : activeBalloons) {
            balloon.update(deltaTime.asSeconds());
        }

        // Process player-tile interactions using the new helper
        processPlayerTileInteraction(player1, deltaTime.asSeconds(), grid);
        processPlayerTileInteraction(player2, deltaTime.asSeconds(), grid);

        window.clear(sf::Color::White);

        // --- Drawing for Player 1 (left half) ---
        view1.setCenter(player1.getPosition());
        window.setView(view1);

        // Draw grid for Player 1, showing Player 1's secrets
        for (int i = 0; i < GridSize; i++) {
            for (int j = 0; j < GridSize; j++) {
                grid[i][j]->draw(window, player1.GetPlayerID()); // Pass player 1's ID
            }
        }
        player1.draw(window);
        player2.draw(window);
        for (const auto& balloon : activeBalloons) {
            window.draw(balloon);
        }

        // --- Drawing for Player 2 (right half) ---
        view2.setCenter(player2.getPosition());
        window.setView(view2);

        // Draw grid for Player 2, showing Player 2's secrets
        for (int i = 0; i < GridSize; i++) {
            for (int j = 0; j < GridSize; j++) {
                grid[i][j]->draw(window, player2.GetPlayerID()); // Pass player 2's ID
            }
        }
        player1.draw(window);
        player2.draw(window);
        for (const auto& balloon : activeBalloons) {
            window.draw(balloon);
        }

        // Balloon logic and collisions (executed once)
        activeBalloons.erase(std::remove_if(activeBalloons.begin(), activeBalloons.end(),
                                           [&](AttackBalloon& balloon) {
                                               sf::Vector2f dist1 = balloon.getPosition() - player1.getPosition();
                                               float distanceSq1 = dist1.x * dist1.x + dist1.y * dist1.y;
                                               float minDistanceSq1 = (balloon.getRadius() + player1.getRadius()) * (balloon.getRadius() + player1.getRadius());

                                               if (!player1.isEliminatedPlayer() && distanceSq1 < minDistanceSq1) {
                                                   player1.GetDamageBalloon();
                                                   std::cout << "Balon a lovit player 1! Viata: " << player1.getHealth() << std::endl;
                                                   return true;
                                               }

                                               sf::Vector2f dist2 = balloon.getPosition() - player2.getPosition();
                                               float distanceSq2 = dist2.x * dist2.x + dist2.y * dist2.y;
                                               float minDistanceSq2 = (balloon.getRadius() + player2.getRadius()) * (balloon.getRadius() + player2.getRadius());

                                               if (!player2.isEliminatedPlayer() && distanceSq2 < minDistanceSq2) {
                                                   player2.GetDamageBalloon();
                                                   std::cout << "Balon a lovit player 2! Viata: " << player2.getHealth() << std::endl;
                                                   return true;
                                               }
                                               return balloon.isExpired();
                                           }),
                       activeBalloons.end());

        window.setView(window.getDefaultView());
        window.draw(dividingLine);

        // --- Update and Draw Ownership Bar ---
        std::map<int, int> tileCounts = arena.getPlayerTileCounts();
        int player1Owned = tileCounts[player1.GetPlayerID()];
        int player2Owned = tileCounts[player2.GetPlayerID()];
        int totalTiles = arena.getTotalTiles();

        float player1Percentage = (totalTiles > 0) ? (static_cast<float>(player1Owned) / totalTiles) : 0.0f;
        float player2Percentage = (totalTiles > 0) ? (static_cast<float>(player2Owned) / totalTiles) : 0.0f;

        // Adjust widths based on ownership percentage
        player1OwnershipBar.setSize({ownershipBarWidth * player1Percentage, ownershipBarHeight});
        player2OwnershipBar.setSize({ownershipBarWidth * player2Percentage, ownershipBarHeight});

        // Position bars next to each other within the background
        player1OwnershipBar.setPosition(ownershipBarBackground.getPosition().x, ownershipBarBackground.getPosition().y);
        player2OwnershipBar.setPosition(ownershipBarBackground.getPosition().x + player1OwnershipBar.getSize().x, ownershipBarBackground.getPosition().y);

        // Update percentage texts
        player1PercentageText.setString(std::to_string(static_cast<int>(player1Percentage * 100)) + "%");
        player2PercentageText.setString(std::to_string(static_cast<int>(player2Percentage * 100)) + "%");

        // Center percentage texts within their respective bar segments, or just above them for clarity
        player1PercentageText.setPosition(player1OwnershipBar.getPosition().x + player1OwnershipBar.getGlobalBounds().width / 2.0f - player1PercentageText.getGlobalBounds().width / 2.0f,
                                          player1OwnershipBar.getPosition().y + ownershipBarHeight / 2.0f - player1PercentageText.getGlobalBounds().height / 2.0f);
        player2PercentageText.setPosition(player2OwnershipBar.getPosition().x + player2OwnershipBar.getGlobalBounds().width / 2.0f - player2PercentageText.getGlobalBounds().width / 2.0f,
                                          player2OwnershipBar.getPosition().y + ownershipBarHeight / 2.0f - player2PercentageText.getGlobalBounds().height / 2.0f);

        // Draw ownership bar elements
        window.draw(ownershipBarBackground);
        window.draw(player1OwnershipBar);
        window.draw(player2OwnershipBar);
        window.draw(player1PercentageText);
        window.draw(player2PercentageText);
        window.draw(totalTilesText); // Total tiles text remains at its fixed position

        window.display();
    }
}