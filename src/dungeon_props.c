#include "../include/dungeon.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Models and textures for props
Model torchModel, barrelModel, crateModel, tableModel;
Texture2D torchTexture, barrelTexture, crateTexture, tableTexture;

// Function to clear 90-degree corners for better navigation
void ClearCornerBlocks(Dungeon* dungeon) {
    // Scan through the dungeon to find and clear 90-degree corners
    for (int x = 1; x < dungeon->width - 1; x++) {
        for (int y = 1; y < dungeon->height - 1; y++) {
            // Skip if not a wall
            if (dungeon->tiles[x][y] != TILE_WALL) continue;
            
            // Check for 90-degree corner patterns
            // Pattern 1: Floor to the left and above, with walls at diagonal
            if (dungeon->tiles[x-1][y] == TILE_FLOOR && 
                dungeon->tiles[x][y-1] == TILE_FLOOR && 
                dungeon->tiles[x-1][y-1] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
            
            // Pattern 2: Floor to the right and above, with walls at diagonal
            else if (dungeon->tiles[x+1][y] == TILE_FLOOR && 
                     dungeon->tiles[x][y-1] == TILE_FLOOR && 
                     dungeon->tiles[x+1][y-1] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
            
            // Pattern 3: Floor to the left and below, with walls at diagonal
            else if (dungeon->tiles[x-1][y] == TILE_FLOOR && 
                     dungeon->tiles[x][y+1] == TILE_FLOOR && 
                     dungeon->tiles[x-1][y+1] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
            
            // Pattern 4: Floor to the right and below, with walls at diagonal
            else if (dungeon->tiles[x+1][y] == TILE_FLOOR && 
                     dungeon->tiles[x][y+1] == TILE_FLOOR && 
                     dungeon->tiles[x+1][y+1] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
        }
    }
}

// Function to add decorative props to the dungeon
void AddDecorativeProps(Dungeon* dungeon) {
    // Create prop models
    torchModel = LoadModelFromMesh(GenMeshCylinder(0.05f, 0.5f, 8));
    barrelModel = LoadModelFromMesh(GenMeshCylinder(0.3f, 0.6f, 8));
    crateModel = LoadModelFromMesh(GenMeshCube(0.5f, 0.5f, 0.5f));
    tableModel = LoadModelFromMesh(GenMeshCube(0.8f, 0.5f, 0.5f));
    
    // Create basic colored textures for props
    Image torchImg = GenImageColor(64, 64, (Color){200, 150, 50, 255});
    Image barrelImg = GenImageColor(64, 64, (Color){139, 69, 19, 255});
    Image crateImg = GenImageColor(64, 64, (Color){160, 120, 80, 255});
    Image tableImg = GenImageColor(64, 64, (Color){120, 80, 40, 255});
    
    torchTexture = LoadTextureFromImage(torchImg);
    barrelTexture = LoadTextureFromImage(barrelImg);
    crateTexture = LoadTextureFromImage(crateImg);
    tableTexture = LoadTextureFromImage(tableImg);
    
    UnloadImage(torchImg);
    UnloadImage(barrelImg);
    UnloadImage(crateImg);
    UnloadImage(tableImg);
    
    // Apply textures to models
    torchModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = torchTexture;
    barrelModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = barrelTexture;
    crateModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = crateTexture;
    tableModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tableTexture;
    
    // Place torches along walls
    for (int x = 1; x < dungeon->width - 1; x++) {
        for (int y = 1; y < dungeon->height - 1; y++) {
            // Place torches on walls with floor adjacent to them
            if (dungeon->tiles[x][y] == TILE_WALL) {
                bool hasAdjacentFloor = false;
                
                // Check all four directions for adjacent floor
                if ((x > 0 && dungeon->tiles[x-1][y] == TILE_FLOOR) ||
                    (x < dungeon->width-1 && dungeon->tiles[x+1][y] == TILE_FLOOR) ||
                    (y > 0 && dungeon->tiles[x][y-1] == TILE_FLOOR) ||
                    (y < dungeon->height-1 && dungeon->tiles[x][y+1] == TILE_FLOOR)) {
                    hasAdjacentFloor = true;
                }
                
                // Add a torch with a 5% chance if there's an adjacent floor
                if (hasAdjacentFloor && GetRandomValue(0, 19) == 0) {
                    // Store the torch position (we'll use a custom struct or array for this in a full implementation)
                    // For this example, we'll just mark it in memory
                    // In a full implementation, these would be stored in a prop list in the dungeon struct
                    
                    // Note: In a production game, you would:
                    // 1. Create a Props array in the Dungeon struct
                    // 2. Add each prop with its type, position, and rotation
                    // 3. Render props in the rendering function
                }
            }
            
            // Place barrels and crates in rooms (on floor tiles)
            if (dungeon->tiles[x][y] == TILE_FLOOR) {
                // Check if we're in a room (not a corridor)
                bool isInRoom = false;
                
                // Simple heuristic: if there are floor tiles in all 4 directions, likely in a room
                if (x > 1 && x < dungeon->width-2 && y > 1 && y < dungeon->height-2) {
                    if (dungeon->tiles[x-1][y] == TILE_FLOOR && 
                        dungeon->tiles[x+1][y] == TILE_FLOOR &&
                        dungeon->tiles[x][y-1] == TILE_FLOOR && 
                        dungeon->tiles[x][y+1] == TILE_FLOOR) {
                        isInRoom = true;
                    }
                }
                
                // Add props with low probability to avoid cluttering
                if (isInRoom) {
                    // 1% chance for a barrel
                    if (GetRandomValue(0, 99) == 0) {
                        // Store barrel position for rendering
                    }
                    
                    // 1% chance for a crate
                    if (GetRandomValue(0, 99) == 0) {
                        // Store crate position for rendering
                    }
                    
                    // 0.5% chance for a table
                    if (GetRandomValue(0, 199) == 0) {
                        // Store table position for rendering
                    }
                }
            }
        }
    }
}

// Unload prop resources
void UnloadProps() {
    UnloadModel(torchModel);
    UnloadModel(barrelModel);
    UnloadModel(crateModel);
    UnloadModel(tableModel);
    
    UnloadTexture(torchTexture);
    UnloadTexture(barrelTexture);
    UnloadTexture(crateTexture);
    UnloadTexture(tableTexture);
}

// Draw props - this would be called from the main rendering loop
void DrawDungeonProps(Dungeon* dungeon) {
    // In a full implementation, you would iterate through the prop list and draw each one
    // For now, we'll just demonstrate the concept with placeholder props
    
    // Placeholder torch positions (in a real implementation, these would come from the dungeon's prop list)
    DrawModel(torchModel, (Vector3){5.0f, 1.5f, 5.0f}, 1.0f, WHITE);
    DrawModel(torchModel, (Vector3){10.0f, 1.5f, 8.0f}, 1.0f, WHITE);
    
    // Placeholder barrel positions
    DrawModel(barrelModel, (Vector3){7.0f, 0.3f, 12.0f}, 1.0f, WHITE);
    DrawModel(barrelModel, (Vector3){15.0f, 0.3f, 18.0f}, 1.0f, WHITE);
    
    // Placeholder crate positions
    DrawModel(crateModel, (Vector3){9.0f, 0.25f, 7.0f}, 1.0f, WHITE);
    
    // Placeholder table position
    DrawModel(tableModel, (Vector3){12.0f, 0.25f, 15.0f}, 1.0f, WHITE);
}
