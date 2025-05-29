# Craven Caverns

A 3D first-person action RPG with randomly generated dungeons, created using Raylib.

## Features

- First-person 3D perspective gameplay
- Procedurally generated dungeons with random layouts
- Different enemy types with unique behaviors (skeletons)
- Item and inventory system with weapons, armor, potions, and more
- RPG progression system with experience and leveling
- Multiple dungeon levels with increasing difficulty
- UI system with health/stamina/mana bars, minimap, and inventory display

## Controls

- **WASD**: Move the player
- **Mouse**: Look around
- **Left Mouse Button**: Attack
- **Space**: Jump
- **E**: Interact/Pick up items
- **1-9**: Use/equip inventory items
- **P or ESC**: Pause game
- **R**: Restart (when game over)

## Requirements

- Raylib library
- GCC compiler
- Linux environment

## Building and Running

1. Make sure you have raylib installed on your system. On Ubuntu/Debian, you can install it with:
   ```
   sudo apt install libraylib-dev
   ```

2. Clone this repository and navigate to the project directory

3. Build the project:
   ```
   make
   ```

4. Run the game:
   ```
   ./craven_caverns
   ```

## Asset Credits

This project uses assets from:
- KayKit Dungeon Pack Remastered
- KayKit Skeletons Pack

## Project Structure

- `src/`: Source code files
  - `main.c`: Entry point and main game loop
  - `game.c`: Game state and management
  - `dungeon.c`: Procedural dungeon generation
  - `player.c`: Player controls and mechanics
  - `enemy.c`: Enemy AI and behaviors
  - `item.c`: Item system and inventory
  - `ui.c`: User interface rendering
- `include/`: Header files
- `assets/`: Game assets (models, textures, sounds)
  - `models/`: 3D models
  - `textures/`: Textures
  - `sounds/`: Sound effects and music
- `raw-assets/`: Original asset files

## Development Notes

The game's key systems:

1. **Dungeon Generation**: Uses a room-based approach with connecting corridors
2. **Enemy AI**: State-based AI with idle, patrol, chase, and attack behaviors
3. **RPG Systems**: Experience, leveling, item stats, and combat calculations
4. **Rendering**: Uses Raylib for 3D rendering and UI
