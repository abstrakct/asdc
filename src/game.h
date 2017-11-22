/*
 * game.h
 */

#pragma once

enum GameStateType {
    gsUnknown = 0,
    gsInGame,
    gsInInventory,
    gsWhatever
};

struct GameState {
    GameStateType state;
    bool isRunning = false;
    bool isWizardMode = false;
};
