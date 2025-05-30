#include "../include/game.h"
#include "../include/dungeon.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/item.h"
#include "../include/ui.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

// Asset paths
#define MODELS_PATH "assets/models/"
#define TEXTURES_PATH "assets/textures/"
#define SOUNDS_PATH "assets/sounds/"

// Game constants
#define MAX_ENEMIES 50
#define MAX_ITEMS 100
#define MAX_LEVEL 5

void LoadGameAssets(GameState* gameState) {
    // Initialize game camera (first person view)
    gameState->camera = (Camera){
        .position = (Vector3){ 0.0f, 0.3f, 0.0f },
        .target = (Vector3){ 0.0f, 0.3f, 1.0f },
        .up = (Vector3){ 0.0f, 1.0f, 0.0f },
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    // Set up game parameters
    gameState->currentLevel = 1;
    gameState->maxLevel = MAX_LEVEL;
    gameState->gameTime = 0.0f;
    gameState->isPaused = false;
    
    // Allocate memory for enemies and items
    gameState->enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));
    gameState->maxEnemies = MAX_ENEMIES;
    gameState->enemyCount = 0;
    
    gameState->items = (Item*)malloc(MAX_ITEMS * sizeof(Item));
    gameState->maxItems = MAX_ITEMS;
    gameState->itemCount = 0;
    
    // Load dungeon assets based on level theme
    int theme = GetRandomValue(0, 2); // Random theme (can be expanded)
    LoadDungeonAssets(gameState->dungeon, theme);
    
    // Generate initial dungeon
    GenerateDungeon(gameState->dungeon, 30, 30, 10, theme);
    
    // Place player at dungeon start position
    gameState->player->position = gameState->dungeon->startPosition;
    
    // Create a placeholder for missing UI textures
    CreateFallbackTextures(gameState);
}

void UnloadGameAssets(GameState* gameState) {
    // Free enemies
    for (int i = 0; i < gameState->enemyCount; i++) {
        UnloadEnemy(&gameState->enemies[i]);
    }
    free(gameState->enemies);
    
    // Free items
    for (int i = 0; i < gameState->itemCount; i++) {
        UnloadItem(&gameState->items[i]);
    }
    free(gameState->items);
}

void UpdateGame(GameState* gameState, float deltaTime) {
    // Update game timer
    gameState->gameTime += deltaTime;
    
    // Process input based on current state
    switch(gameState->currentState) {
        case TITLE_SCREEN:
            // Check for game start input
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                gameState->currentState = GAMEPLAY;
            }
            break;
            
        case GAMEPLAY:
            if (!gameState->isPaused) {
                // Update player
                UpdatePlayer(gameState->player, deltaTime);
                
                // Store previous position for collision detection
                Vector3 previousPosition = gameState->player->position;
                
                // Calculate new position based on velocity
                Vector3 newPosition = previousPosition;
                newPosition.x += gameState->player->velocity.x * deltaTime;
                newPosition.y += gameState->player->velocity.y * deltaTime;
                newPosition.z += gameState->player->velocity.z * deltaTime;
                
                // Collision detection with walls
                // Check X-axis movement
                if (IsWalkable(gameState->dungeon, newPosition.x, previousPosition.z)) {
                    gameState->player->position.x = newPosition.x;
                }
                
                // Check Z-axis movement
                if (IsWalkable(gameState->dungeon, gameState->player->position.x, newPosition.z)) {
                    gameState->player->position.z = newPosition.z;
                }
                
                // Handle gravity and ground collision
                if (newPosition.y < 0) {
                    // Collision with ground
                    gameState->player->position.y = 0;
                    gameState->player->isGrounded = true;
                    gameState->player->isJumping = false;
                } else {
                    // Apply Y movement
                    gameState->player->position.y = newPosition.y;
                    
                    // If player is going down and not at ground level, they're not grounded
                    if (gameState->player->velocity.y < 0 && gameState->player->position.y > 0) {
                        gameState->player->isGrounded = false;
                    }
                }
                
                // Update camera to follow player at the new lower height
                gameState->camera.position = (Vector3){
                    gameState->player->position.x,
                    gameState->player->position.y + gameState->player->cameraHeight,
                    gameState->player->position.z
                };
                
                // Calculate the target position based on player direction
                gameState->camera.target = Vector3Add(
                    gameState->camera.position,
                    gameState->player->direction
                );
                
                // Update enemies
                for (int i = 0; i < gameState->enemyCount; i++) {
                    if (gameState->enemies[i].isAlive) {
                        // Check if enemy is visible to player (basic line of sight)
                        bool canSeePlayer = true; // Simplified, could implement proper raycasting
                        
                        // Update enemy behavior
                        UpdateEnemy(&gameState->enemies[i], gameState->player->position, deltaTime, canSeePlayer);
                        
                        // Check for enemy attack on player
                        int damageDealt = 0;
                        if (EnemyAttack(&gameState->enemies[i], gameState->player->position, gameState->player->radius, &damageDealt)) {
                            PlayerTakeDamage(gameState->player, damageDealt);
                        }
                        
                        // Check for player attack on enemy
                        if (gameState->player->isAttacking) {
                            // Calculate distance between player and enemy
                            Vector3 diff = Vector3Subtract(gameState->enemies[i].position, gameState->player->position);
                            float distance = Vector3Length(diff);
                            
                            // Check if enemy is within attack range and in front of player
                            if (distance <= gameState->player->attackRange) {
                                // Check if enemy is in player's field of view (simplified)
                                Vector3 normalizedDiff = Vector3Normalize(diff);
                                float dotProduct = Vector3DotProduct(gameState->player->direction, normalizedDiff);
                                
                                if (dotProduct > 0.5f) { // Within ~60 degree cone in front
                                    EnemyTakeDamage(&gameState->enemies[i], gameState->player->attackDamage);
                                    
                                    // If enemy died, give player experience
                                    if (!gameState->enemies[i].isAlive) {
                                        PlayerAddExperience(gameState->player, gameState->enemies[i].experienceValue);
                                        
                                        // Chance to drop item
                                        if (GetRandomValue(0, 100) < gameState->enemies[i].dropChance * 100) {
                                            if (gameState->itemCount < gameState->maxItems) {
                                                // Generate random item at enemy position
                                                InitItem(&gameState->items[gameState->itemCount], 
                                                         GetRandomValue(1, 6), // Random item type 
                                                         GetRandomValue(0, 4),  // Random subtype
                                                         gameState->currentLevel, 
                                                         gameState->enemies[i].position);
                                                gameState->itemCount++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Update items
                for (int i = 0; i < gameState->itemCount; i++) {
                    if (gameState->items[i].isOnGround) {
                        UpdateItem(&gameState->items[i], deltaTime);
                        
                        // Check if player is picking up the item
                        Vector3 diff = Vector3Subtract(gameState->items[i].position, gameState->player->position);
                        float distance = Vector3Length(diff);
                        
                        if (distance < 1.5f) { // Pickup range
                            if (IsKeyPressed(KEY_E)) { // Pickup key
                                PlayerAddItem(gameState->player, gameState->items[i]);
                                
                                // Remove item from ground (simplified, should use a better removal method)
                                gameState->items[i].isOnGround = false;
                            }
                        }
                    }
                }
                
                // Check for level completion (player reached end position)
                Vector3 diffToEnd = Vector3Subtract(gameState->dungeon->endPosition, gameState->player->position);
                float distanceToEnd = Vector3Length(diffToEnd);
                
                if (distanceToEnd < 1.5f) {
                    if (gameState->currentLevel < gameState->maxLevel) {
                        // Generate next level
                        gameState->currentLevel++;
                        
                        // Unload current level
                        UnloadDungeon(gameState->dungeon);
                        
                        // Clear enemies and items
                        gameState->enemyCount = 0;
                        gameState->itemCount = 0;
                        
                        // Generate new dungeon with increasing difficulty
                        int theme = GetRandomValue(0, 2);
                        LoadDungeonAssets(gameState->dungeon, theme);
                        GenerateDungeon(gameState->dungeon, 30 + gameState->currentLevel * 5, 
                                       30 + gameState->currentLevel * 5, 
                                       10 + gameState->currentLevel, theme);
                        
                        // Place player at dungeon start
                        gameState->player->position = gameState->dungeon->startPosition;
                    } else {
                        // Player completed all levels
                        gameState->currentState = VICTORY;
                    }
                }
                
                // Check for game over condition
                if (gameState->player->stats.health <= 0) {
                    gameState->currentState = GAME_OVER;
                }
            }
            
            // Toggle pause
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                gameState->isPaused = !gameState->isPaused;
            }
            break;
            
        case GAME_OVER:
            // Check for restart input
            if (IsKeyPressed(KEY_R)) {
                // Reset game state for a new game
                gameState->currentState = TITLE_SCREEN;
                gameState->currentLevel = 1;
                gameState->enemyCount = 0;
                gameState->itemCount = 0;
                
                // Reset player
                InitPlayer(gameState->player);
                
                // Generate new starting dungeon
                UnloadDungeon(gameState->dungeon);
                int theme = GetRandomValue(0, 2);
                LoadDungeonAssets(gameState->dungeon, theme);
                GenerateDungeon(gameState->dungeon, 30, 30, 10, theme);
                
                // Place player at dungeon start
                gameState->player->position = gameState->dungeon->startPosition;
            }
            break;
            
        case VICTORY:
            // Check for new game input
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                gameState->currentState = TITLE_SCREEN;
            }
            break;
            
        default:
            break;
    }
}

void DrawTitleScreen(GameState* gameState) {
    // Draw title screen background
    DrawRectangle(0, 0, gameState->screenWidth, gameState->screenHeight, BLACK);
    
    // Draw game title
    const char* title = "CRAVEN CAVERNS";
    int fontSize = 60;
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, fontSize, 2);
    DrawText(title, (gameState->screenWidth - titleSize.x) / 2, gameState->screenHeight / 4, fontSize, RED);
    
    // Draw subtitle
    const char* subtitle = "A First-Person Action RPG";
    fontSize = 30;
    Vector2 subtitleSize = MeasureTextEx(GetFontDefault(), subtitle, fontSize, 2);
    DrawText(subtitle, (gameState->screenWidth - subtitleSize.x) / 2, gameState->screenHeight / 4 + 70, fontSize, GRAY);
    
    // Draw instructions
    const char* instructions = "Press ENTER or SPACE to Start";
    fontSize = 20;
    Vector2 instructionsSize = MeasureTextEx(GetFontDefault(), instructions, fontSize, 2);
    DrawText(instructions, (gameState->screenWidth - instructionsSize.x) / 2, 
             gameState->screenHeight * 3/4, fontSize, WHITE);
    
    // Draw controls info
    const char* controls = "Controls: WASD to move, MOUSE to look, LEFT CLICK to attack, E to interact";
    fontSize = 15;
    Vector2 controlsSize = MeasureTextEx(GetFontDefault(), controls, fontSize, 2);
    DrawText(controls, (gameState->screenWidth - controlsSize.x) / 2, 
             gameState->screenHeight * 3/4 + 40, fontSize, LIGHTGRAY);
}

void DrawGameplay(GameState* gameState) {
    // Enable 3D mode with the camera
    BeginMode3D(gameState->camera);
        
        // Draw the dungeon
        DrawDungeon(gameState->dungeon);
        
        // Draw enemies
        for (int i = 0; i < gameState->enemyCount; i++) {
            if (gameState->enemies[i].isAlive) {
                DrawEnemy(&gameState->enemies[i], gameState->camera);
            }
        }
        
        // Draw items on the ground
        for (int i = 0; i < gameState->itemCount; i++) {
            if (gameState->items[i].isOnGround) {
                DrawItem(&gameState->items[i]);
            }
        }
        
    EndMode3D();
    
    // Draw 2D UI elements
    DrawUI(gameState);
    
    // Draw crosshair in the center of the screen
    DrawRectangle(gameState->screenWidth/2 - 1, gameState->screenHeight/2 - 10, 2, 20, WHITE);
    DrawRectangle(gameState->screenWidth/2 - 10, gameState->screenHeight/2 - 1, 20, 2, WHITE);
    
    // Draw pause menu if game is paused
    if (gameState->isPaused) {
        DrawRectangle(0, 0, gameState->screenWidth, gameState->screenHeight, 
                     (Color){0, 0, 0, 150}); // Semi-transparent black
        
        const char* pausedText = "PAUSED";
        int fontSize = 40;
        Vector2 textSize = MeasureTextEx(GetFontDefault(), pausedText, fontSize, 2);
        DrawText(pausedText, (gameState->screenWidth - textSize.x) / 2, 
                gameState->screenHeight / 2 - 50, fontSize, WHITE);
        
        const char* resumeText = "Press ESC or P to Resume";
        fontSize = 20;
        textSize = MeasureTextEx(GetFontDefault(), resumeText, fontSize, 2);
        DrawText(resumeText, (gameState->screenWidth - textSize.x) / 2, 
                gameState->screenHeight / 2 + 20, fontSize, LIGHTGRAY);
    }
}

void DrawGameOver(GameState* gameState) {
    // Draw game over screen
    DrawRectangle(0, 0, gameState->screenWidth, gameState->screenHeight, BLACK);
    
    const char* gameOverText = "GAME OVER";
    int fontSize = 60;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), gameOverText, fontSize, 2);
    DrawText(gameOverText, (gameState->screenWidth - textSize.x) / 2, 
            gameState->screenHeight / 3, fontSize, RED);
    
    // Draw player stats
    char statsText[100];
    sprintf(statsText, "Level Reached: %d", gameState->currentLevel);
    fontSize = 30;
    DrawText(statsText, gameState->screenWidth / 2 - 150, gameState->screenHeight / 2, fontSize, WHITE);
    
    sprintf(statsText, "Player Level: %d", gameState->player->stats.level);
    DrawText(statsText, gameState->screenWidth / 2 - 150, gameState->screenHeight / 2 + 40, fontSize, WHITE);
    
    // Draw restart instructions
    const char* restartText = "Press R to Restart";
    fontSize = 25;
    textSize = MeasureTextEx(GetFontDefault(), restartText, fontSize, 2);
    DrawText(restartText, (gameState->screenWidth - textSize.x) / 2, 
            gameState->screenHeight * 3/4, fontSize, GRAY);
}

void DrawVictory(GameState* gameState) {
    // Draw victory screen
    DrawRectangle(0, 0, gameState->screenWidth, gameState->screenHeight, BLACK);
    
    const char* victoryText = "VICTORY!";
    int fontSize = 60;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), victoryText, fontSize, 2);
    DrawText(victoryText, (gameState->screenWidth - textSize.x) / 2, 
            gameState->screenHeight / 3, fontSize, GREEN);
    
    // Draw congratulatory message
    const char* congratsText = "You have conquered the Craven Caverns!";
    fontSize = 30;
    textSize = MeasureTextEx(GetFontDefault(), congratsText, fontSize, 2);
    DrawText(congratsText, (gameState->screenWidth - textSize.x) / 2, 
            gameState->screenHeight / 2, fontSize, WHITE);
    
    // Draw player stats
    char statsText[100];
    sprintf(statsText, "Final Player Level: %d", gameState->player->stats.level);
    fontSize = 25;
    DrawText(statsText, gameState->screenWidth / 2 - 150, gameState->screenHeight / 2 + 60, fontSize, LIGHTGRAY);
    
    // Draw new game instructions
    const char* newGameText = "Press ENTER or SPACE for New Game";
    fontSize = 25;
    textSize = MeasureTextEx(GetFontDefault(), newGameText, fontSize, 2);
    DrawText(newGameText, (gameState->screenWidth - textSize.x) / 2, 
            gameState->screenHeight * 3/4, fontSize, GRAY);
}
