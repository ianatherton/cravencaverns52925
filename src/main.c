#include "raylib.h"
#include "raymath.h"
#include "../include/game.h"
#include "../include/dungeon.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/item.h"
#include "../include/ui.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define GAME_TITLE "Craven Caverns"

int main(void) {
    // Initialize window and rendering
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);
    
    // Initialize 3D audio
    InitAudioDevice();
    
    // Initialize random seed for procedural generation
    SetRandomSeed(GetTime());
    
    // Initialize game state
    GameState gameState = {0};
    gameState.currentState = TITLE_SCREEN;
    gameState.screenWidth = SCREEN_WIDTH;
    gameState.screenHeight = SCREEN_HEIGHT;
    
    // Initialize player
    Player player = {0};
    InitPlayer(&player);
    gameState.player = &player;
    
    // Initialize dungeon
    Dungeon dungeon = {0};
    InitDungeon(&dungeon);
    gameState.dungeon = &dungeon;
    
    // Load game assets
    LoadGameAssets(&gameState);
    
    // Game loop
    while (!WindowShouldClose()) {
        // Update game state
        float deltaTime = GetFrameTime();
        UpdateGame(&gameState, deltaTime);
        
        // Draw frame
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw game based on current state
            switch(gameState.currentState) {
                case TITLE_SCREEN:
                    DrawTitleScreen(&gameState);
                    break;
                case GAMEPLAY:
                    DrawGameplay(&gameState);
                    break;
                case GAME_OVER:
                    DrawGameOver(&gameState);
                    break;
                case VICTORY:
                    DrawVictory(&gameState);
                    break;
                default:
                    break;
            }
            
            // Draw FPS counter
            DrawFPS(10, 10);
            
        EndDrawing();
    }
    
    // Clean up resources
    UnloadGameAssets(&gameState);
    UnloadDungeon(&dungeon);
    UnloadPlayer(&player);
    
    // Close window and audio
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
