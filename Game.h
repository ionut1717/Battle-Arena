#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <optional>
#include "Player.h" // Player include Arena, care include Tiles
#include "Balloon.h" // Balloon include Player (via forward declaration)
// Arena și Tiles sunt incluse deja via Player.h și/sau Balloon.h (indirect/direct)
#include <algorithm>
#include <random>
#include <chrono> // Pentru a seed-ui rng-ul cu un timp mai precis

class Game_Engine {
protected:
    Game_Engine() = default;

public:
    static Game_Engine& Instance() {
        static Game_Engine instance;
        return instance;
    }
    Game_Engine(const Game_Engine&) = delete;
    Game_Engine(Game_Engine&&) = delete;
    Game_Engine& operator=(const Game_Engine&) = delete;
    Game_Engine& operator=(Game_Engine&&) = delete;

private:
    std::vector<AttackBalloon> activeBalloons;
    sf::Clock spawnClock;
    float spawnCooldown = 2.0f;
    sf::Clock gameClock;

    float player1ZoomFactor = 1.0f;
    float player2ZoomFactor = 1.0f;
    const float zoomSpeed = 0.9f;
    const float minZoom = 0.5f;
    const float maxZoom = 2.0f;
    const float initialLaunchForce = 1500.0f;
    const float spawnOffsetDistance = 30.0f;

    // Generatori de numere aleatoare pentru teleportare
    std::mt19937 rng;
    std::uniform_int_distribution<int> coordDist;

public:
    void GameLoop() {
        int GridSize = Arena::GetGridSize();
        float TileSize = Tile::getSize();

        // Inițializați generatorul de numere aleatoare aici, după ce GridSize este cunoscut
        rng.seed(std::random_device{}()); // Seed cu random_device
        coordDist = std::uniform_int_distribution<int>(0, GridSize - 1); // Distribuție pentru coordonatele grilei

        sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Battle Arena (Split Screen)", sf::State::Fullscreen);
        unsigned int windowWidth = window.getSize().x;
        unsigned int windowHeight = window.getSize().y;
        window.setVerticalSyncEnabled(true);

        sf::Texture defaultTexture;
        // Încărcarea texturii înainte de a crea arena pentru a evita erorile
        if (!defaultTexture.loadFromFile("../sprite_tile2.jpg")) {
            std::cerr << "Eroare la incarcarea texturii default! Asigurati-va ca 'sprite_tile2.jpg' este in directorul corect. Tile-urile vor fi albe." << std::endl;
        }

        Arena arena(defaultTexture);
        // Tipul de grilă este acum vector de unique_ptr
        std::vector<std::vector<std::unique_ptr<Tile>>>& grid = arena.GetGrid();

        sf::Texture PlayerTexture; // This texture should be loaded once and potentially used by both players if they share the same spritesheet
        if (!PlayerTexture.loadFromFile("../hero.png")) {
            std::cerr << "Eroare la incarcarea texturii playerului (hero.png)! " << std::endl;
        }

        Player player1(1, sf::Color::Blue, {50.0f, 50.0f}, true,PlayerTexture);
        Player player2(2, sf::Color::Red, {static_cast<float>(GridSize * TileSize - 50.0f), 50.0f}, false,PlayerTexture); // Player 2 cu roșu

        sf::View view1;
        float viewHalfWidth = static_cast<float>(windowWidth / 2.0f);
        float viewHeight = static_cast<float>(windowHeight);
        view1.setSize({viewHalfWidth, viewHeight});
        view1.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f})); // left, top, width, height

        sf::View view2;
        view2.setSize({viewHalfWidth, viewHeight});
        view2.setViewport(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f})); // left, top, width, height

        sf::RectangleShape dividingLine;
        float lineWidth = 4.0f;
        dividingLine.setSize({lineWidth, static_cast<float>(windowHeight)});
        dividingLine.setFillColor(sf::Color::Black);
        dividingLine.setPosition({static_cast<float>(windowWidth / 2.0f - lineWidth / 2.0f), 0.0f});

        while (window.isOpen()) {
            sf::Time deltaTime = gameClock.restart();

            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    player1.set_key_pressed(keyPressed->code, true);
                    player2.set_key_pressed(keyPressed->code, true);

                    // Logică de zoom
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
                                launchDirection = {0.0f, 1.0f}; // Fallback: dacă nu se mișcă, lansează în jos
                            } else {
                                launchDirection /= length;
                            }

                            sf::Vector2f spawnPosition = player1.getPosition() + (launchDirection * spawnOffsetDistance);

                            activeBalloons.emplace_back(10.0f, sf::Color::Black, spawnPosition);
                            if (!activeBalloons.empty()) {
                                AttackBalloon& newBalloon = activeBalloons.back();
                                // Folosim adresa directă a obiectului player2
                                newBalloon.setTarget(&player2);
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
                        // Player 1 colorează tile-uri
                        player1.paintNearbyTiles(grid, 10);
                    }
                    if (keyPressed->code == sf::Keyboard::Key::RControl) {
                        // Player 2 colorează tile-uri
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

            // Actualizează poziția jucătorilor (transmite deltaTime)
            player1.update_position(deltaTime.asSeconds());
            player2.update_position(deltaTime.asSeconds());

            // Actualizează baloanele
            for (auto& balloon : activeBalloons) {
                balloon.update(deltaTime.asSeconds());
            }

            // --- Logica optimizată pentru coliziunile jucătorilor cu tile-uri speciale ---
            auto processPlayerTileInteraction = [&](Player& player, float dt) {
                sf::FloatRect playerBounds = player.getBounds();

                // Calculul range-ului de tile-uri de verificat
                int minTileX = static_cast<int>(playerBounds.position.x / TileSize);
                int maxTileX = static_cast<int>((playerBounds.position.x + playerBounds.size.x) / TileSize);
                int minTileY = static_cast<int>(playerBounds.position.y / TileSize);
                int maxTileY = static_cast<int>((playerBounds.position.y + playerBounds.size.y) / TileSize);

                minTileX = std::max(0, minTileX);
                maxTileX = std::min(GridSize - 1, maxTileX);
                minTileY = std::max(0, minTileY);
                maxTileY = std::min(GridSize - 1, maxTileY);

                player.setStuck(false); // Resetăm starea de blocare
                player.setOnDamageTile(false); // Resetăm starea de damage

                Tile* currentIntersectedTile = nullptr; // Pointer la tile-ul cu care s-a intersectat

                for (int y = minTileY; y <= maxTileY; ++y) {
                    for (int x = minTileX; x <= maxTileX; ++x) {
                        // Accesează Tile-ul prin dereferențierea unique_ptr
                        Tile& tile = *grid[y][x];
                        sf::FloatRect tileBounds = tile.getBounds();

                        // Verificarea coliziunii AABB
                        bool intersects =
                            playerBounds.position.x < tileBounds.position.x + tileBounds.size.x &&
                            playerBounds.position.x + playerBounds.size.x > tileBounds.position.x &&
                            playerBounds.position.y < tileBounds.position.y + tileBounds.size.y &&
                            playerBounds.position.y + playerBounds.size.y > tileBounds.position.y;

                        if (intersects) {
                            sf::Vector2i currentTileIdx = {x, y};
                            bool enteredThisTile = (player.getLastGridPosition().x != currentTileIdx.x ||
                                                    player.getLastGridPosition().y != currentTileIdx.y);

                            // --- Acum folosim getSpecialType() în loc de dynamic_cast ---
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
                                        // Teleportarea se face doar dacă tile-ul este deținut de jucătorul curent SAU nu este deținut de nimeni
                                        // Aceasta este o decizie de design: dacă teleportarea e un "secret" al proprietarului.
                                        // Dacă vrei ca oricine să poată fi teleportat, nu mai verifica owner.
                                        if (tile.getOwner() == player.GetPlayerID() || tile.getOwner() == -1) {
                                            int newX = coordDist(rng); // Folosim rng și coordDist din Game_Engine
                                            int newY = coordDist(rng);
                                            sf::Vector2f newPos = {static_cast<float>(newX * TileSize + TileSize / 2.0f),
                                                                   static_cast<float>(newY * TileSize + TileSize / 2.0f)};
                                            player.setPosition(newPos);
                                            std::cout << "Player " << player.GetPlayerID() << " teleportat la (" << newX << ", " << newY << ")" << std::endl;
                                            player.setLastGridPosition({-1,-1}); // Resetează pentru a permite re-teleportarea
                                        }
                                    }
                                    break;
                                case HEALING:
                                    // Adaugă logica pentru HealingTile aici
                                    // De exemplu, dacă un jucător trece peste un HealingTile pe care îl deține:
                                    if (tile.getOwner() == player.GetPlayerID() && enteredThisTile) {
                                        //player.heal(10); // Presupunând că ai o metodă heal() în Player
                                        // sau direct player.PlayerHealth = std::min(100u, player.PlayerHealth + 10);
                                        std::cout << "Player " << player.GetPlayerID() << " a calcat pe un Healing Tile! Viata: " << player.getHealth() << std::endl;
                                    }
                                    currentIntersectedTile = &tile;
                                    break;
                                case NONE:
                                default:
                                    // Nimic special de făcut pentru tile-uri normale
                                    break;
                            }
                        }
                    }
                    if (currentIntersectedTile) {
                        break; // Ieșim și din bucla exterioară dacă am găsit un tile special
                    }
                }

                // Actualizează lastGridPosition dacă jucătorul nu este pe un tile special sau a ieșit de pe unul
                if (!currentIntersectedTile && player.getLastGridPosition().x != -1) {
                    // Dacă jucătorul nu se mai află pe un tile special, actualizează poziția sa normală în grilă
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
            };

            processPlayerTileInteraction(player1, deltaTime.asSeconds());
            processPlayerTileInteraction(player2, deltaTime.asSeconds());

            window.clear(sf::Color::White);

            // --- Desenare pentru Player 1 (jumătatea stângă) ---
            view1.setCenter(player1.getPosition());
            window.setView(view1);

            // Desenează grila pentru Player 1, arătând secretele lui Player 1
            for (int i = 0; i < GridSize; i++) {
                for (int j = 0; j < GridSize; j++) {
                    grid[i][j]->draw(window, player1.GetPlayerID()); // Transmite ID-ul playerului 1
                }
            }
            player1.draw(window);
            player2.draw(window);
            for (const auto& balloon : activeBalloons) {
                window.draw(balloon);
            }

            // --- Desenare pentru Player 2 (jumătatea dreaptă) ---
            view2.setCenter(player2.getPosition());
            window.setView(view2);

            // Desenează grila pentru Player 2, arătând secretele lui Player 2
            for (int i = 0; i < GridSize; i++) {
                for (int j = 0; j < GridSize; j++) {
                    grid[i][j]->draw(window, player2.GetPlayerID()); // Transmite ID-ul playerului 2
                }
            }
            player1.draw(window);
            player2.draw(window);
            for (const auto& balloon : activeBalloons) {
                window.draw(balloon);
            }

            // Logica baloanelor și coliziunilor (se execută o singură dată)
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

            window.display();
        }
    }

};

#endif // GAME_H