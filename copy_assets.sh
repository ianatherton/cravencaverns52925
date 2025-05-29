#!/bin/bash

# Create asset directories if they don't exist
mkdir -p assets/models/dungeon
mkdir -p assets/models/skeletons
mkdir -p assets/textures/dungeon
mkdir -p assets/textures/skeletons
mkdir -p assets/textures/ui
mkdir -p assets/sounds

# Copy dungeon models (only copying a few key assets for demonstration)
echo "Copying dungeon models..."
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/barrel* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/chest* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/floor* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/wall* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/stairs* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/door* assets/models/dungeon/
cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/gltf/torch* assets/models/dungeon/

# Copy skeleton models
echo "Copying skeleton models..."
cp -r raw-assets/KayKit_Skeletons_1.0_FREE/assets/gltf/* assets/models/skeletons/

# Copy textures
echo "Copying textures..."
if [ -d "raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/textures" ]; then
    cp -r raw-assets/KayKit_DungeonRemastered_1.1_FREE/Assets/textures/* assets/textures/dungeon/
fi

if [ -d "raw-assets/KayKit_Skeletons_1.0_FREE/texture" ]; then
    cp -r raw-assets/KayKit_Skeletons_1.0_FREE/texture/* assets/textures/skeletons/
fi

echo "Asset copying completed!"
