#include "../include/dungeon.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Initialize dungeon structure
void InitDungeon(Dungeon* dungeon) {
    dungeon->width = 0;
    dungeon->height = 0;
    dungeon->tiles = NULL;
    dungeon->rooms = NULL;
    dungeon->roomCount = 0;
    dungeon->maxRooms = 0;
    dungeon->startPosition = (Vector3){0.0f, 0.0f, 0.0f};
    dungeon->endPosition = (Vector3){0.0f, 0.0f, 0.0f};
    dungeon->theme = 0;
}

// Generate a random dungeon layout
void GenerateDungeon(Dungeon* dungeon, int width, int height, int maxRooms, int theme) {
    // Set dungeon properties
    dungeon->width = width;
    dungeon->height = height;
    dungeon->maxRooms = maxRooms;
    dungeon->theme = theme;
    
    // Allocate memory for the tiles grid
    dungeon->tiles = (TileType**)malloc(width * sizeof(TileType*));
    for (int x = 0; x < width; x++) {
        dungeon->tiles[x] = (TileType*)malloc(height * sizeof(TileType));
        // Initialize all tiles as walls
        for (int y = 0; y < height; y++) {
            dungeon->tiles[x][y] = TILE_WALL;
        }
    }
    
    // Allocate memory for rooms
    dungeon->rooms = (Room*)malloc(maxRooms * sizeof(Room));
    dungeon->roomCount = 0;
    
    // Random seed if not already set
    if (GetRandomValue(0, 1) == 0) {
        SetRandomSeed(time(NULL));
    }
    
    // Generate rooms
    for (int i = 0; i < maxRooms; i++) {
        // Random room size (width and height)
        int roomWidth = GetRandomValue(4, 10);
        int roomHeight = GetRandomValue(4, 10);
        
        // Random room position (ensuring it fits within the dungeon)
        int roomX = GetRandomValue(1, width - roomWidth - 1);
        int roomY = GetRandomValue(1, height - roomHeight - 1);
        
        // Check if room overlaps with existing rooms
        bool overlaps = false;
        for (int j = 0; j < dungeon->roomCount; j++) {
            Room existingRoom = dungeon->rooms[j];
            
            // Check if the new room overlaps with the existing room (with a margin of 1)
            if (roomX <= existingRoom.x + existingRoom.width + 1 &&
                roomX + roomWidth + 1 >= existingRoom.x &&
                roomY <= existingRoom.y + existingRoom.height + 1 &&
                roomY + roomHeight + 1 >= existingRoom.y) {
                overlaps = true;
                break;
            }
        }
        
        if (!overlaps && dungeon->roomCount < maxRooms) {
            // Add room to the rooms array
            Room newRoom = {roomX, roomY, roomWidth, roomHeight, false};
            dungeon->rooms[dungeon->roomCount] = newRoom;
            
            // Carve out the room in the tiles grid
            for (int x = roomX; x < roomX + roomWidth; x++) {
                for (int y = roomY; y < roomY + roomHeight; y++) {
                    dungeon->tiles[x][y] = TILE_FLOOR;
                }
            }
            
            // Connect to the previous room if not the first room
            if (dungeon->roomCount > 0) {
                // Get the center of the current and previous room
                int newCenterX = roomX + roomWidth / 2;
                int newCenterY = roomY + roomHeight / 2;
                
                Room prevRoom = dungeon->rooms[dungeon->roomCount - 1];
                int prevCenterX = prevRoom.x + prevRoom.width / 2;
                int prevCenterY = prevRoom.y + prevRoom.height / 2;
                
                // Create corridors between the rooms
                // Randomly choose horizontal-then-vertical or vertical-then-horizontal
                if (GetRandomValue(0, 1) == 0) {
                    // Horizontal then vertical
                    CreateHorizontalCorridor(dungeon, prevCenterX, newCenterX, prevCenterY);
                    CreateVerticalCorridor(dungeon, prevCenterY, newCenterY, newCenterX);
                } else {
                    // Vertical then horizontal
                    CreateVerticalCorridor(dungeon, prevCenterY, newCenterY, prevCenterX);
                    CreateHorizontalCorridor(dungeon, prevCenterX, newCenterX, newCenterY);
                }
                
                // Mark the room as connected
                dungeon->rooms[dungeon->roomCount].connected = true;
            }
            
            dungeon->roomCount++;
        }
    }
    
    // Ensure all rooms are connected
    for (int i = 0; i < dungeon->roomCount; i++) {
        if (!dungeon->rooms[i].connected && i > 0) {
            // Connect to a random previous room
            int targetRoom = GetRandomValue(0, i - 1);
            
            int currentCenterX = dungeon->rooms[i].x + dungeon->rooms[i].width / 2;
            int currentCenterY = dungeon->rooms[i].y + dungeon->rooms[i].height / 2;
            
            int targetCenterX = dungeon->rooms[targetRoom].x + dungeon->rooms[targetRoom].width / 2;
            int targetCenterY = dungeon->rooms[targetRoom].y + dungeon->rooms[targetRoom].height / 2;
            
            // Create corridors between the rooms
            if (GetRandomValue(0, 1) == 0) {
                CreateHorizontalCorridor(dungeon, currentCenterX, targetCenterX, currentCenterY);
                CreateVerticalCorridor(dungeon, currentCenterY, targetCenterY, targetCenterX);
            } else {
                CreateVerticalCorridor(dungeon, currentCenterY, targetCenterY, currentCenterX);
                CreateHorizontalCorridor(dungeon, currentCenterX, targetCenterX, targetCenterY);
            }
            
            dungeon->rooms[i].connected = true;
        }
    }
    
    // Add some doors between corridors and rooms
    AddDoors(dungeon);
    
    // Place start and end points
    // Start in the first room
    Room startRoom = dungeon->rooms[0];
    dungeon->startPosition = (Vector3){
        startRoom.x + startRoom.width / 2,
        1.0f, // Player height above ground
        startRoom.y + startRoom.height / 2
    };
    
    // End in the last room
    Room endRoom = dungeon->rooms[dungeon->roomCount - 1];
    dungeon->endPosition = (Vector3){
        endRoom.x + endRoom.width / 2,
        0.0f, // On the ground
        endRoom.y + endRoom.height / 2
    };
    
    // Add stairs in the start and end rooms
    int startX = (int)dungeon->startPosition.x;
    int startZ = (int)dungeon->startPosition.z;
    dungeon->tiles[startX][startZ] = TILE_STAIRS_UP;
    
    int endX = (int)dungeon->endPosition.x;
    int endZ = (int)dungeon->endPosition.z;
    dungeon->tiles[endX][endZ] = TILE_STAIRS_DOWN;
    
    // Add some random traps and chests
    AddRandomTraps(dungeon, dungeon->width * dungeon->height / 100); // 1% of tiles are traps
    AddRandomChests(dungeon, maxRooms / 2); // 50% of max rooms have chests
}

// Create a horizontal corridor between x1 and x2 at y
void CreateHorizontalCorridor(Dungeon* dungeon, int x1, int x2, int y) {
    // Ensure x1 is less than x2
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    
    // Create corridor
    for (int x = x1; x <= x2; x++) {
        if (x >= 0 && x < dungeon->width && y >= 0 && y < dungeon->height) {
            // Only replace walls, don't overwrite floors
            if (dungeon->tiles[x][y] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
        }
    }
}

// Create a vertical corridor between y1 and y2 at x
void CreateVerticalCorridor(Dungeon* dungeon, int y1, int y2, int x) {
    // Ensure y1 is less than y2
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    
    // Create corridor
    for (int y = y1; y <= y2; y++) {
        if (x >= 0 && x < dungeon->width && y >= 0 && y < dungeon->height) {
            // Only replace walls, don't overwrite floors
            if (dungeon->tiles[x][y] == TILE_WALL) {
                dungeon->tiles[x][y] = TILE_FLOOR;
            }
        }
    }
}

// Add doors to the dungeon
void AddDoors(Dungeon* dungeon) {
    // Find potential door locations (where a floor is adjacent to a wall)
    for (int x = 1; x < dungeon->width - 1; x++) {
        for (int y = 1; y < dungeon->height - 1; y++) {
            if (dungeon->tiles[x][y] == TILE_FLOOR) {
                // Check if this is a corridor (has walls on opposite sides)
                bool isHorizontalCorridor = (dungeon->tiles[x][y-1] == TILE_WALL && 
                                            dungeon->tiles[x][y+1] == TILE_WALL);
                                            
                bool isVerticalCorridor = (dungeon->tiles[x-1][y] == TILE_WALL && 
                                          dungeon->tiles[x+1][y] == TILE_WALL);
                
                // Add a door with a certain probability
                if ((isHorizontalCorridor || isVerticalCorridor) && GetRandomValue(0, 20) == 0) {
                    dungeon->tiles[x][y] = TILE_DOOR;
                }
            }
        }
    }
}

// Add random traps to the dungeon
void AddRandomTraps(Dungeon* dungeon, int count) {
    for (int i = 0; i < count; i++) {
        // Get random position
        int x = GetRandomValue(1, dungeon->width - 2);
        int y = GetRandomValue(1, dungeon->height - 2);
        
        // Only place traps on floor tiles, not near start or end
        if (dungeon->tiles[x][y] == TILE_FLOOR) {
            // Check distance from start and end positions
            Vector3 trapPos = {x, 0, y};
            if (Vector3Distance(trapPos, dungeon->startPosition) > 5.0f && 
                Vector3Distance(trapPos, dungeon->endPosition) > 5.0f) {
                dungeon->tiles[x][y] = TILE_TRAP;
            }
        }
    }
}

// Add random chests to the dungeon
void AddRandomChests(Dungeon* dungeon, int count) {
    for (int i = 0; i < count; i++) {
        // Choose a random room (not the first or last)
        if (dungeon->roomCount <= 2) continue; // Need at least 3 rooms
        
        int roomIndex = GetRandomValue(1, dungeon->roomCount - 2);
        Room room = dungeon->rooms[roomIndex];
        
        // Place chest at a random position in the room
        int chestX = room.x + GetRandomValue(1, room.width - 2);
        int chestY = room.y + GetRandomValue(1, room.height - 2);
        
        if (dungeon->tiles[chestX][chestY] == TILE_FLOOR) {
            dungeon->tiles[chestX][chestY] = TILE_CHEST;
        }
    }
}

// Load dungeon assets based on theme
void LoadDungeonAssets(Dungeon* dungeon, int theme) {
    // Path to models directory
    const char* modelsPath = "assets/models/";
    const char* texturesPath = "assets/textures/";
    
    // Asset file names for different themes
    const char* themeNames[] = {"dungeon", "cave", "crypt"};
    const char* themeName = themeNames[theme % 3]; // Ensure theme is valid
    
    char modelPath[256];
    char texturePath[256];
    
    // Create directory structure for models if it doesn't exist
    snprintf(modelPath, sizeof(modelPath), "mkdir -p assets/models/%s", themeName);
    system(modelPath);
    
    // Create directory structure for textures if it doesn't exist
    snprintf(texturePath, sizeof(texturePath), "mkdir -p assets/textures/%s", themeName);
    system(texturePath);
    
    // Copy models and textures from raw-assets to our asset directory
    // For now, we'll use the raylib's built-in primitive shapes for testing
    // In a real implementation, you'd load actual model files
    
    // Create models using raylib primitives
    // Floor
    dungeon->floorModel = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 2, 2)); // 2x2 segments for better texture mapping
    
    // Wall - increased height to 3.0f
    dungeon->wallModel = LoadModelFromMesh(GenMeshCube(1.0f, 3.0f, 1.0f));
    
    // Door - increased height to match walls
    dungeon->doorModel = LoadModelFromMesh(GenMeshCube(1.0f, 3.0f, 0.2f));
    
    // Stairs
    dungeon->stairsUpModel = LoadModelFromMesh(GenMeshCube(1.0f, 0.5f, 1.0f));
    dungeon->stairsDownModel = LoadModelFromMesh(GenMeshCube(1.0f, 0.5f, 1.0f));
    
    // Trap
    dungeon->trapModel = LoadModelFromMesh(GenMeshPlane(0.8f, 0.8f, 1, 1));
    
    // Chest
    dungeon->chestModel = LoadModelFromMesh(GenMeshCube(0.8f, 0.5f, 0.5f));
    
    // Load actual textures for floor and walls
    char wallTexturePath[256];
    char floorTexturePath[256];
    
    // Build paths to textures
    snprintf(wallTexturePath, sizeof(wallTexturePath), "%sdungeon/tiling_dungeon_brickwall01.png", texturesPath);
    snprintf(floorTexturePath, sizeof(floorTexturePath), "%sdungeon/tiling_dungeon_floor01.png", texturesPath);
    
    // Load wall texture
    dungeon->wallTexture = LoadTexture(wallTexturePath);
    
    // Load floor texture
    dungeon->floorTexture = LoadTexture(floorTexturePath);
    
    // If textures couldn't be loaded, create fallbacks
    if (dungeon->wallTexture.id == 0) {
        Image img = GenImageColor(128, 128, (Color){128, 128, 128, 255}); // Gray
        dungeon->wallTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    
    if (dungeon->floorTexture.id == 0) {
        Image img = GenImageColor(128, 128, (Color){139, 69, 19, 255}); // Brown
        dungeon->floorTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    
    // Create fallback images for other textures
    Image img;
    
    // Door texture (dark brown) - now using wall texture instead
    dungeon->doorTexture = dungeon->wallTexture;
    
    // Stairs texture (white)
    img = GenImageColor(128, 128, (Color){200, 200, 200, 255});
    dungeon->stairsTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Trap texture (red)
    img = GenImageColor(128, 128, (Color){200, 0, 0, 255});
    dungeon->trapTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Chest texture (yellow)
    img = GenImageColor(128, 128, (Color){218, 165, 32, 255});
    dungeon->chestTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Set material for each model
    dungeon->wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->wallTexture;
    dungeon->floorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->floorTexture;
    dungeon->doorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->wallTexture; // Use wall texture for doors
    dungeon->stairsUpModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->stairsTexture;
    dungeon->stairsDownModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->stairsTexture;
    dungeon->trapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->trapTexture;
    dungeon->chestModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = dungeon->chestTexture;
    
    // Configure texture wrapping for proper tiling
    SetTextureWrap(dungeon->wallTexture, TEXTURE_WRAP_REPEAT);
    SetTextureWrap(dungeon->floorTexture, TEXTURE_WRAP_REPEAT);
}

// Draw the dungeon
void DrawDungeon(Dungeon* dungeon) {
    // Draw each tile
    for (int x = 0; x < dungeon->width; x++) {
        for (int y = 0; y < dungeon->height; y++) {
            TileType tile = dungeon->tiles[x][y];
            
            switch (tile) {
                case TILE_FLOOR:
                    // Draw floor
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    break;
                    
                case TILE_WALL:
                    // Draw wall - adjusted Y position to account for taller walls
                    DrawModel(dungeon->wallModel, (Vector3){x, 1.5f, y}, 1.0f, WHITE);
                    break;
                    
                case TILE_DOOR:
                    // Draw floor under door
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    
                    // Determine door orientation
                    bool isHorizontalCorridor = false;
                    if (x > 0 && x < dungeon->width - 1) {
                        isHorizontalCorridor = (dungeon->tiles[x-1][y] != TILE_WALL && 
                                              dungeon->tiles[x+1][y] != TILE_WALL);
                    }
                    
                    // Draw door with appropriate rotation
                    if (isHorizontalCorridor) {
                        // Door facing north-south
                        DrawModelEx(dungeon->doorModel, 
                                   (Vector3){x, 1.5f, y}, 
                                   (Vector3){0.0f, 1.0f, 0.0f}, 
                                   90.0f, 
                                   (Vector3){1.0f, 1.0f, 1.0f}, 
                                   WHITE);
                    } else {
                        // Door facing east-west
                        DrawModelEx(dungeon->doorModel, 
                                   (Vector3){x, 1.5f, y}, 
                                   (Vector3){0.0f, 1.0f, 0.0f}, 
                                   0.0f, 
                                   (Vector3){1.0f, 1.0f, 1.0f}, 
                                   WHITE);
                    }
                    break;
                    
                case TILE_STAIRS_UP:
                    // Draw stairs up
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    DrawModelEx(dungeon->stairsUpModel, 
                               (Vector3){x, 0.25f, y}, 
                               (Vector3){1.0f, 0.0f, 0.0f}, 
                               20.0f, 
                               (Vector3){1.0f, 1.0f, 1.0f}, 
                               WHITE);
                    break;
                    
                case TILE_STAIRS_DOWN:
                    // Draw stairs down
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    DrawModelEx(dungeon->stairsDownModel, 
                               (Vector3){x, 0.25f, y}, 
                               (Vector3){1.0f, 0.0f, 0.0f}, 
                               -20.0f, 
                               (Vector3){1.0f, 1.0f, 1.0f}, 
                               WHITE);
                    break;
                    
                case TILE_TRAP:
                    // Draw floor with trap
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    DrawModel(dungeon->trapModel, (Vector3){x, 0.01f, y}, 1.0f, WHITE);
                    break;
                    
                case TILE_CHEST:
                    // Draw floor with chest
                    DrawModel(dungeon->floorModel, (Vector3){x, 0.0f, y}, 1.0f, WHITE);
                    DrawModel(dungeon->chestModel, (Vector3){x, 0.25f, y}, 1.0f, WHITE);
                    break;
                    
                default:
                    break;
            }
        }
    }
}

// Free dungeon resources
void UnloadDungeon(Dungeon* dungeon) {
    // Free the tiles grid
    if (dungeon->tiles != NULL) {
        for (int x = 0; x < dungeon->width; x++) {
            free(dungeon->tiles[x]);
        }
        free(dungeon->tiles);
        dungeon->tiles = NULL;
    }
    
    // Free the rooms array
    if (dungeon->rooms != NULL) {
        free(dungeon->rooms);
        dungeon->rooms = NULL;
    }
    
    // Unload models
    UnloadModel(dungeon->floorModel);
    UnloadModel(dungeon->wallModel);
    UnloadModel(dungeon->doorModel);
    UnloadModel(dungeon->stairsUpModel);
    UnloadModel(dungeon->stairsDownModel);
    UnloadModel(dungeon->trapModel);
    UnloadModel(dungeon->chestModel);
    
    // Unload textures
    UnloadTexture(dungeon->floorTexture);
    UnloadTexture(dungeon->wallTexture);
    UnloadTexture(dungeon->doorTexture);
    UnloadTexture(dungeon->stairsTexture);
    UnloadTexture(dungeon->trapTexture);
    UnloadTexture(dungeon->chestTexture);
    
    // Reset dungeon properties
    dungeon->width = 0;
    dungeon->height = 0;
    dungeon->roomCount = 0;
    dungeon->maxRooms = 0;
}

// Get a random walkable position in the dungeon
Vector3 GetRandomFloorPosition(Dungeon* dungeon) {
    int x, y;
    
    // Keep trying until we find a floor tile
    do {
        x = GetRandomValue(0, dungeon->width - 1);
        y = GetRandomValue(0, dungeon->height - 1);
    } while (dungeon->tiles[x][y] != TILE_FLOOR);
    
    return (Vector3){x, 0.0f, y};
}

// Check if a position is walkable
bool IsWalkable(Dungeon* dungeon, float x, float z) {
    // Convert floating point coordinates to integer tile coordinates
    int tileX = (int)x;
    int tileZ = (int)z;
    
    // Check if the position is inside the dungeon bounds
    if (!IsInsideDungeon(dungeon, x, z)) {
        return false;
    }
    
    // Check if the tile is walkable
    TileType tile = dungeon->tiles[tileX][tileZ];
    
    return (tile == TILE_FLOOR || 
            tile == TILE_DOOR || 
            tile == TILE_STAIRS_UP || 
            tile == TILE_STAIRS_DOWN || 
            tile == TILE_TRAP || 
            tile == TILE_CHEST);
}

// Check if a position is inside the dungeon bounds
bool IsInsideDungeon(Dungeon* dungeon, float x, float z) {
    int tileX = (int)x;
    int tileZ = (int)z;
    
    return (tileX >= 0 && tileX < dungeon->width && 
            tileZ >= 0 && tileZ < dungeon->height);
}
