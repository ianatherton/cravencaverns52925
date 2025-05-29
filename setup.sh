#!/bin/bash

echo "Setting up Craven Caverns..."

# Check for raylib installation
if ! pkg-config --exists raylib; then
    echo "WARNING: Raylib not found!"
    echo "The game requires Raylib to compile."
    echo "On Ubuntu/Debian, you can install it with: sudo apt install libraylib-dev"
    echo "On other systems, visit the Raylib website: https://www.raylib.com/"
    echo ""
    echo "Continuing setup without Raylib..."
else
    echo "Raylib found! Continuing setup..."
fi

# Create directory structure
mkdir -p obj
mkdir -p assets/models/dungeon
mkdir -p assets/models/skeletons
mkdir -p assets/textures/dungeon
mkdir -p assets/textures/skeletons
mkdir -p assets/textures/ui
mkdir -p assets/sounds

# Copy assets if present
echo "Copying assets from raw-assets folder..."
./copy_assets.sh

# Create placeholder UI textures if they don't exist
if [ ! -f "assets/textures/ui/item_slot.png" ]; then
    echo "Creating placeholder UI textures..."
    mkdir -p assets/textures/ui
fi

echo "Setup complete! You can now build the game with 'make' and run it with './craven_caverns'"
