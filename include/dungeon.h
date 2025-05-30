#ifndef DUNGEON_H
#define DUNGEON_H

#include "raylib.h"

// Define tile types for the dungeon
typedef enum {
    TILE_NONE,
    TILE_FLOOR,
    TILE_WALL,
    TILE_DOOR,
    TILE_STAIRS_DOWN,
    TILE_STAIRS_UP,
    TILE_TRAP,
    TILE_CHEST
} TileType;

// Room structure definition
typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool connected;
} Room;

// Dungeon structure definition
typedef struct Dungeon {
    int width;
    int height;
    TileType** tiles;
    
    // Rooms information
    Room* rooms;
    int roomCount;
    int maxRooms;
    
    // Starting and ending points
    Vector3 startPosition;
    Vector3 endPosition;
    
    // Dungeon theme (affects textures and models used)
    int theme;
    
    // Models used for dungeon rendering
    Model floorModel;
    Model wallModel;
    Model ceilingModel; // Added ceiling model
    Model doorModel;
    Model stairsUpModel;
    Model stairsDownModel;
    Model trapModel;
    Model chestModel;
    
    // Textures for dungeon elements
    Texture2D floorTexture;
    Texture2D wallTexture;
    Texture2D ceilingTexture; // Added ceiling texture
    Texture2D doorTexture;
    Texture2D stairsTexture;
    Texture2D trapTexture;
    Texture2D chestTexture;
} Dungeon;

// Dungeon generation and management functions
void InitDungeon(Dungeon* dungeon);
void GenerateDungeon(Dungeon* dungeon, int width, int height, int maxRooms, int theme);
void LoadDungeonAssets(Dungeon* dungeon, int theme);
void UnloadDungeon(Dungeon* dungeon);
void DrawDungeon(Dungeon* dungeon);
Vector3 GetRandomFloorPosition(Dungeon* dungeon);
bool IsWalkable(Dungeon* dungeon, float x, float z, float radius);
bool IsInsideDungeon(Dungeon* dungeon, float x, float z);

#endif // DUNGEON_H
