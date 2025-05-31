// GameState.h
#ifndef GAME_STATE_H
#define GAME_STATE_H

// Enum pentru stările generale ale jocului
enum class GameState {
    Menu,      // Starea de meniu principal
    Playing,   // Starea jocului activ
    Settings,  // Starea de setări
    Exiting,   // Starea de ieșire din aplicație
    GameOver   // Starea de Game Over (adăugată pentru o mai bună granularitate)
};

#endif // GAME_STATE_H