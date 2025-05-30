#ifndef DUNGEON_PROPS_H
#define DUNGEON_PROPS_H

#include "dungeon.h"

// Function to clear 90-degree corners for better navigation
void ClearCornerBlocks(Dungeon* dungeon);

// Function to add decorative props to the dungeon
void AddDecorativeProps(Dungeon* dungeon);

// Unload prop resources
void UnloadProps();

// Draw props - called from the main rendering loop
void DrawDungeonProps(Dungeon* dungeon);

#endif // DUNGEON_PROPS_H
