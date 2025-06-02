#include "Game.h"
int main() {
    Game_Engine &game_engine=Game_Engine::Instance();
    game_engine.GameLoop();
    return 0;
}