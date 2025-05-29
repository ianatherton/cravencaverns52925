#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "dungeon.h"
#include "player.h"
#include "enemy.h"
#include "item.h"

// Game state enumeration
typedef enum {
    TITLE_SCREEN,
    GAMEPLAY,
    GAME_OVER,
    VICTORY
} GameStateEnum;

// Game state structure
typedef struct GameState {
    GameStateEnum currentState;
    int screenWidth;
    int screenHeight;
    
    // Game components
    Player* player;
    Dungeon* dungeon;
    
    // Enemy management
    Enemy* enemies;
    int enemyCount;
    int maxEnemies;
    
    // Item management
    Item* items;
    int itemCount;
    int maxItems;
    
    // Game assets
    Model* models;
    Texture2D* textures;
    Sound* sounds;
    
    // Game camera
    Camera camera;
    
    // Game settings
    float gameTime;
    int currentLevel;
    int maxLevel;
    bool isPaused;
} GameState;

// Game functions
void LoadGameAssets(GameState* gameState);
void UnloadGameAssets(GameState* gameState);
void UpdateGame(GameState* gameState, float deltaTime);
void DrawTitleScreen(GameState* gameState);
void DrawGameplay(GameState* gameState);
void DrawGameOver(GameState* gameState);
void DrawVictory(GameState* gameState);

#endif // GAME_H
