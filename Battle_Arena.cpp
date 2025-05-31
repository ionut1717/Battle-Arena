#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <SFML/System.hpp>
#include "Game.h"
int main() {

    Game_Engine &game_engine=Game_Engine::Instance();
    game_engine.GameLoop();
    return 0;
}