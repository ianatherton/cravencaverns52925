#include "../include/ui.h"
#include "../include/game.h"
#include "../include/player.h"
#include <stdio.h>
#include <math.h>

// Draw all UI elements
void DrawUI(GameState* gameState) {
    // Draw health bar
    Rectangle healthBarBounds = {10, gameState->screenHeight - 60, 200, 20};
    DrawHealthBar(gameState, healthBarBounds, 
                 gameState->player->stats.health, 
                 gameState->player->stats.maxHealth, 
                 RED);
    
    // Draw stamina bar
    Rectangle staminaBarBounds = {10, gameState->screenHeight - 35, 200, 10};
    DrawStaminaBar(gameState, staminaBarBounds, 
                  gameState->player->stats.stamina, 
                  gameState->player->stats.maxStamina, 
                  GREEN);
    
    // Draw mana bar
    Rectangle manaBarBounds = {10, gameState->screenHeight - 20, 200, 10};
    DrawManaBar(gameState, manaBarBounds, 
               gameState->player->stats.mana, 
               gameState->player->stats.maxMana, 
               BLUE);
    
    // Draw level and experience
    char levelText[50];
    sprintf(levelText, "Level: %d", gameState->player->stats.level);
    DrawText(levelText, 220, gameState->screenHeight - 60, 20, WHITE);
    
    // Draw experience bar
    Rectangle expBarBounds = {220, gameState->screenHeight - 35, 200, 10};
    float expRatio = (float)gameState->player->stats.experience / 
                    (float)gameState->player->stats.experienceToNextLevel;
    DrawRectangle(expBarBounds.x, expBarBounds.y, 
                 expBarBounds.width, expBarBounds.height, 
                 DARKGRAY);
    DrawRectangle(expBarBounds.x, expBarBounds.y, 
                 expBarBounds.width * expRatio, expBarBounds.height, 
                 PURPLE);
    DrawRectangleLines(expBarBounds.x, expBarBounds.y, 
                      expBarBounds.width, expBarBounds.height, 
                      LIGHTGRAY);
    
    // Draw current dungeon level
    char dungeonLevelText[50];
    sprintf(dungeonLevelText, "Dungeon Level: %d/%d", 
           gameState->currentLevel, gameState->maxLevel);
    DrawText(dungeonLevelText, 220, gameState->screenHeight - 20, 20, WHITE);
    
    // Draw hotbar (quick items)
    DrawInventory(gameState);
    
    // Draw minimap in top-right corner
    DrawMinimapUI(gameState);
}

// Draw health bar
void DrawHealthBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color) {
    float healthRatio = value / maxValue;
    
    // Draw background
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, DARKGRAY);
    
    // Draw health
    DrawRectangle(bounds.x, bounds.y, bounds.width * healthRatio, bounds.height, color);
    
    // Draw border
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, LIGHTGRAY);
    
    // Draw text
    char healthText[20];
    sprintf(healthText, "HP: %.0f/%.0f", value, maxValue);
    DrawText(healthText, bounds.x + 5, bounds.y + bounds.height/2 - 5, 10, WHITE);
}

// Draw stamina bar
void DrawStaminaBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color) {
    float staminaRatio = value / maxValue;
    
    // Draw background
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, DARKGRAY);
    
    // Draw stamina
    DrawRectangle(bounds.x, bounds.y, bounds.width * staminaRatio, bounds.height, color);
    
    // Draw border
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, LIGHTGRAY);
}

// Draw mana bar
void DrawManaBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color) {
    float manaRatio = value / maxValue;
    
    // Draw background
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, DARKGRAY);
    
    // Draw mana
    DrawRectangle(bounds.x, bounds.y, bounds.width * manaRatio, bounds.height, color);
    
    // Draw border
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, LIGHTGRAY);
}

// Draw inventory UI
void DrawInventory(GameState* gameState) {
    // Draw hotbar slots at the bottom of the screen
    int slotSize = 50;
    int slotPadding = 5;
    int totalWidth = 10 * (slotSize + slotPadding) - slotPadding;
    int startX = (gameState->screenWidth - totalWidth) / 2;
    int startY = gameState->screenHeight - 80;
    
    // Draw the item slots
    for (int i = 0; i < 10; i++) {
        Rectangle slotBounds = {
            startX + i * (slotSize + slotPadding),
            startY,
            slotSize,
            slotSize
        };
        
        bool isSelected = (i == 0); // Currently selected slot (would be set based on player input)
        DrawItemSlot(slotBounds, gameState->player->itemSlot, isSelected);
        
        // Draw item if there is one in this slot
        if (i < gameState->player->inventory.itemCount) {
            Item* item = &gameState->player->inventory.items[i];
            
            // If item has an icon, draw it, otherwise use unknown_item texture
            if (item->icon.id != 0) {
                DrawTexture(item->icon, slotBounds.x + 5, slotBounds.y + 5, WHITE);
            } else {
                DrawTexture(gameState->player->unknownItem, slotBounds.x + 5, slotBounds.y + 5, WHITE);
            }
            
            // Draw item count if stackable
            if (item->type == ITEM_POTION || item->type == ITEM_GOLD) {
                char countText[10];
                sprintf(countText, "%d", 1); // In a real implementation, items would have a count property
                DrawText(countText, slotBounds.x + slotSize - 15, slotBounds.y + slotSize - 15, 10, WHITE);
            }
            
            // Draw equipped indicator
            if (item->isEquipped) {
                DrawRectangleLines(slotBounds.x, slotBounds.y, slotSize, slotSize, YELLOW);
            }
        }
        
        // Draw hotkey number
        char keyText[5];
        sprintf(keyText, "%d", (i + 1) % 10);
        DrawText(keyText, slotBounds.x + 5, slotBounds.y + 5, 10, LIGHTGRAY);
    }
}

// Draw equipment UI
void DrawEquipment(GameState* gameState) {
    // This would show the currently equipped items in a more detailed view
    // For simplicity, we're only implementing the hotbar in this example
}

// Draw player stats UI
void DrawStats(GameState* gameState) {
    // This would show detailed player stats in a menu view
    // For simplicity, we're only implementing the basic HUD in this example
}

// Draw minimap UI
void DrawMinimapUI(GameState* gameState) {
    // Define minimap size and position
    int minimapSize = 150;
    int minimapX = gameState->screenWidth - minimapSize - 10;
    int minimapY = 10;
    int minimapScale = 3; // Each dungeon tile is represented by this many pixels
    
    // Draw minimap background
    DrawRectangle(minimapX, minimapY, minimapSize, minimapSize, (Color){0, 0, 0, 150});
    
    // Draw dungeon tiles on minimap
    for (int x = 0; x < gameState->dungeon->width; x++) {
        for (int y = 0; y < gameState->dungeon->height; y++) {
            // Calculate minimap pixel position
            int pixelX = minimapX + (x * minimapScale) % minimapSize;
            int pixelY = minimapY + (y * minimapScale) % minimapSize;
            
            // Skip if outside minimap bounds
            if (pixelX >= minimapX + minimapSize || pixelY >= minimapY + minimapSize) {
                continue;
            }
            
            // Draw different colors based on tile type
            Color pixelColor = BLACK;
            
            switch (gameState->dungeon->tiles[x][y]) {
                case TILE_FLOOR:
                    pixelColor = (Color){100, 100, 100, 255};
                    break;
                case TILE_WALL:
                    pixelColor = (Color){50, 50, 50, 255};
                    break;
                case TILE_DOOR:
                    pixelColor = (Color){150, 75, 0, 255};
                    break;
                case TILE_STAIRS_UP:
                    pixelColor = (Color){0, 255, 0, 255};
                    break;
                case TILE_STAIRS_DOWN:
                    pixelColor = (Color){255, 0, 0, 255};
                    break;
                case TILE_TRAP:
                    pixelColor = (Color){255, 0, 0, 255};
                    break;
                case TILE_CHEST:
                    pixelColor = (Color){255, 255, 0, 255};
                    break;
                default:
                    break;
            }
            
            // Draw the minimap tile
            DrawRectangle(pixelX, pixelY, minimapScale, minimapScale, pixelColor);
        }
    }
    
    // Draw player position on minimap
    int playerX = minimapX + ((int)gameState->player->position.x * minimapScale) % minimapSize;
    int playerY = minimapY + ((int)gameState->player->position.z * minimapScale) % minimapSize;
    
    if (playerX >= minimapX && playerX < minimapX + minimapSize &&
        playerY >= minimapY && playerY < minimapY + minimapSize) {
        // Draw player as blue square
        DrawRectangle(playerX, playerY, minimapScale, minimapScale, BLUE);
        
        // Add a flashing white dot in the direction the player is facing
        // Calculate the direction offset based on player direction vector
        float dirX = gameState->player->direction.x;
        float dirZ = gameState->player->direction.z;
        
        // Scale the direction vector to determine dot position
        // The dot should be 2 units away from player position
        int dotDistance = 2 * minimapScale;
        int directionX = playerX + (int)(dirX * dotDistance);
        int directionY = playerY + (int)(dirZ * dotDistance);
        
        // Make sure the direction indicator is within minimap bounds
        if (directionX >= minimapX && directionX < minimapX + minimapSize &&
            directionY >= minimapY && directionY < minimapY + minimapSize) {
            
            // Make the dot flash by checking the game time
            float gameTime = GetTime();
            if (fmodf(gameTime, 0.5f) < 0.25f) {
                // Draw the direction indicator dot
                DrawRectangle(directionX, directionY, minimapScale, minimapScale, WHITE);
            }
        }
    }
    
    // Draw minimap border
    DrawRectangleLines(minimapX, minimapY, minimapSize, minimapSize, WHITE);
}

// Draw game menu
void DrawGameMenu(GameState* gameState) {
    // This would be a full-screen menu when the game is paused
    // For simplicity, we're only implementing a basic pause overlay in the main game loop
}

// Check if a UI button was pressed
bool UIButtonPressed(UIElement button, Vector2 mousePosition, bool mousePressed) {
    if (CheckCollisionPointRec(mousePosition, button.bounds)) {
        button.isHovered = true;
        
        if (mousePressed) {
            button.isPressed = true;
            return true;
        }
    } else {
        button.isHovered = false;
        button.isPressed = false;
    }
    
    return false;
}

// Draw a UI element
void DrawUIElement(UIElement element) {
    // Draw based on element type
    switch (element.type) {
        case UI_BUTTON:
            // Draw button with different colors based on state
            if (element.isPressed) {
                DrawRectangle(element.bounds.x, element.bounds.y, 
                             element.bounds.width, element.bounds.height, 
                             (Color){element.color.r/2, element.color.g/2, element.color.b/2, element.color.a});
            } else if (element.isHovered) {
                DrawRectangle(element.bounds.x, element.bounds.y, 
                             element.bounds.width, element.bounds.height, 
                             (Color){element.color.r * 1.2f, element.color.g * 1.2f, element.color.b * 1.2f, element.color.a});
            } else {
                DrawRectangle(element.bounds.x, element.bounds.y, 
                             element.bounds.width, element.bounds.height, 
                             element.color);
            }
            
            // Draw button text
            int fontSize = 20;
            Vector2 textSize = MeasureTextEx(GetFontDefault(), element.text, fontSize, 2);
            DrawText(element.text, 
                    element.bounds.x + (element.bounds.width - textSize.x) / 2, 
                    element.bounds.y + (element.bounds.height - textSize.y) / 2, 
                    fontSize, element.textColor);
            break;
            
        case UI_PROGRESS_BAR:
            // Draw progress bar background
            DrawRectangle(element.bounds.x, element.bounds.y, 
                         element.bounds.width, element.bounds.height, 
                         DARKGRAY);
            
            // Draw progress
            float progress = element.value / (element.maxValue - element.minValue);
            DrawRectangle(element.bounds.x, element.bounds.y, 
                         element.bounds.width * progress, element.bounds.height, 
                         element.color);
            
            // Draw border
            DrawRectangleLines(element.bounds.x, element.bounds.y, 
                              element.bounds.width, element.bounds.height, 
                              LIGHTGRAY);
            break;
            
        case UI_IMAGE:
            // Draw image
            if (element.texture.id != 0) {
                DrawTexture(element.texture, 
                           element.bounds.x, element.bounds.y, 
                           WHITE);
            }
            break;
            
        case UI_TEXT:
            // Draw text
            DrawText(element.text, 
                    element.bounds.x, element.bounds.y, 
                    20, element.textColor);
            break;
            
        default:
            break;
    }
}

// Draw a tooltip
void DrawTooltip(const char* text, Vector2 position) {
    int fontSize = 15;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
    
    // Draw background
    DrawRectangle(position.x, position.y, 
                 textSize.x + 10, textSize.y + 10, 
                 (Color){50, 50, 50, 200});
    
    // Draw text
    DrawText(text, position.x + 5, position.y + 5, fontSize, WHITE);
    
    // Draw border
    DrawRectangleLines(position.x, position.y, 
                      textSize.x + 10, textSize.y + 10, 
                      LIGHTGRAY);
}

// Draw an item slot
void DrawItemSlot(Rectangle bounds, Texture2D texture, bool isSelected) {
    // Draw slot background
    if (texture.id != 0) {
        // Draw the slot texture
        DrawTexture(texture, bounds.x, bounds.y, WHITE);
    } else {
        // Draw a fallback if texture is missing
        DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, DARKGRAY);
    }
    
    // Draw selection highlight
    if (isSelected) {
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, WHITE);
        DrawRectangleLines(bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height - 2, WHITE);
    } else {
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, LIGHTGRAY);
    }
}

// Create fallback textures for UI elements if they're missing
void CreateFallbackTextures(GameState* gameState) {
    // Create directory structure if it doesn't exist
    system("mkdir -p assets/textures/ui");
    
    // Generate fallback for item slot texture if needed
    if (gameState->player->itemSlot.id == 0) {
        // Create a blank image for the item slot
        Image itemSlotImg = GenImageColor(50, 50, (Color){70, 70, 70, 255});
        
        // Draw a border
        ImageDrawRectangleLines(&itemSlotImg, (Rectangle){0, 0, 50, 50}, 2, (Color){100, 100, 100, 255});
        
        // Load the texture from the image
        gameState->player->itemSlot = LoadTextureFromImage(itemSlotImg);
        
        // Save the image to file for future use
        ExportImage(itemSlotImg, "assets/textures/ui/item_slot.png");
        UnloadImage(itemSlotImg);
    }
    
    // Generate fallback for unknown item texture if needed
    if (gameState->player->unknownItem.id == 0) {
        // Create a blank image for the unknown item
        Image unknownItemImg = GenImageColor(40, 40, (Color){100, 100, 100, 255});
        
        // Draw a question mark
        ImageDrawRectangle(&unknownItemImg, 18, 8, 4, 4, (Color){200, 200, 200, 255});
        ImageDrawRectangle(&unknownItemImg, 18, 16, 4, 16, (Color){200, 200, 200, 255});
        ImageDrawRectangle(&unknownItemImg, 14, 12, 12, 4, (Color){200, 200, 200, 255});
        
        // Load the texture from the image
        gameState->player->unknownItem = LoadTextureFromImage(unknownItemImg);
        
        // Save the image to file for future use
        ExportImage(unknownItemImg, "assets/textures/ui/unknown_item.png");
        UnloadImage(unknownItemImg);
    }
}
