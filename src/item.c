#include "../include/item.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Forward declarations of helper functions
void ConfigureWeapon(Item* item);
void ConfigureArmor(Item* item);
void ConfigureAccessory(Item* item);
void ConfigurePotion(Item* item);
void ConfigureScroll(Item* item);
void ConfigureKey(Item* item);
void ConfigureGold(Item* item);
void ApplyLevelScaling(Item* item);
void CreateItemModel(Item* item);

// Initialize an item
void InitItem(Item* item, ItemType type, int subType, int level, Vector3 position) {
    // Clear item data
    memset(item, 0, sizeof(Item));
    
    // Set basic properties
    item->type = type;
    item->subType = subType;
    item->level = level;
    item->position = position;
    item->isOnGround = true;
    item->rotationAngle = 0.0f;
    item->bobHeight = 0.0f;
    item->bobTimer = 0.0f;
    item->isEquipped = false;
    item->isIdentified = false;
    
    // Set item properties based on type and subtype
    switch(type) {
        case ITEM_WEAPON:
            ConfigureWeapon(item);
            break;
        case ITEM_ARMOR:
            ConfigureArmor(item);
            break;
        case ITEM_ACCESSORY:
            ConfigureAccessory(item);
            break;
        case ITEM_POTION:
            ConfigurePotion(item);
            break;
        case ITEM_SCROLL:
            ConfigureScroll(item);
            break;
        case ITEM_KEY:
            ConfigureKey(item);
            break;
        case ITEM_GOLD:
            ConfigureGold(item);
            break;
        default:
            break;
    }
    
    // Apply level-based scaling to stats
    ApplyLevelScaling(item);
    
    // Create a simple model for the item based on its type
    CreateItemModel(item);
}

// Configure weapon properties
void ConfigureWeapon(Item* item) {
    switch(item->subType) {
        case WEAPON_SWORD:
            strcpy(item->name, "Sword");
            strcpy(item->description, "A sharp blade for cutting enemies.");
            item->minDamage = 5;
            item->maxDamage = 10;
            item->value = 100;
            item->color = RED;
            break;
        case WEAPON_AXE:
            strcpy(item->name, "Axe");
            strcpy(item->description, "A heavy axe that deals crushing blows.");
            item->minDamage = 7;
            item->maxDamage = 12;
            item->value = 120;
            item->color = DARKGRAY;
            break;
        case WEAPON_MACE:
            strcpy(item->name, "Mace");
            strcpy(item->description, "A blunt weapon good against armored foes.");
            item->minDamage = 6;
            item->maxDamage = 11;
            item->value = 110;
            item->color = GRAY;
            break;
        case WEAPON_STAFF:
            strcpy(item->name, "Staff");
            strcpy(item->description, "A magical staff that boosts intelligence.");
            item->minDamage = 4;
            item->maxDamage = 8;
            item->intelligence = 5;
            item->value = 150;
            item->color = PURPLE;
            break;
        case WEAPON_BOW:
            strcpy(item->name, "Bow");
            strcpy(item->description, "A ranged weapon for attacking from afar.");
            item->minDamage = 5;
            item->maxDamage = 9;
            item->dexterity = 3;
            item->value = 130;
            item->color = BROWN;
            break;
        default:
            strcpy(item->name, "Unknown Weapon");
            strcpy(item->description, "A mysterious weapon.");
            item->minDamage = 3;
            item->maxDamage = 7;
            item->value = 50;
            item->color = WHITE;
            break;
    }
}

// Configure armor properties
void ConfigureArmor(Item* item) {
    switch(item->subType) {
        case ARMOR_HELMET:
            strcpy(item->name, "Helmet");
            strcpy(item->description, "Protects your head from damage.");
            item->armor = 5;
            item->value = 80;
            item->color = LIGHTGRAY;
            break;
        case ARMOR_CHEST:
            strcpy(item->name, "Chestplate");
            strcpy(item->description, "Heavy armor for your torso.");
            item->armor = 10;
            item->value = 150;
            item->color = DARKGRAY;
            break;
        case ARMOR_GLOVES:
            strcpy(item->name, "Gloves");
            strcpy(item->description, "Protective hand coverings.");
            item->armor = 3;
            item->dexterity = 2;
            item->value = 60;
            item->color = BROWN;
            break;
        case ARMOR_BOOTS:
            strcpy(item->name, "Boots");
            strcpy(item->description, "Sturdy footwear for the dungeon.");
            item->armor = 4;
            item->value = 70;
            item->color = DARKBROWN;
            break;
        default:
            strcpy(item->name, "Unknown Armor");
            strcpy(item->description, "A mysterious piece of armor.");
            item->armor = 3;
            item->value = 50;
            item->color = GRAY;
            break;
    }
}

// Configure accessory properties
void ConfigureAccessory(Item* item) {
    strcpy(item->name, "Magic Amulet");
    strcpy(item->description, "An accessory with magical properties.");
    item->intelligence = 3;
    item->dexterity = 2;
    item->strength = 2;
    item->value = 200;
    item->color = GOLD;
}

// Configure potion properties
void ConfigurePotion(Item* item) {
    switch(item->subType) {
        case POTION_HEALTH:
            strcpy(item->name, "Health Potion");
            strcpy(item->description, "Restores health when consumed.");
            item->effectValue = 30;
            item->value = 40;
            item->color = RED;
            break;
        case POTION_MANA:
            strcpy(item->name, "Mana Potion");
            strcpy(item->description, "Restores mana when consumed.");
            item->effectValue = 30;
            item->value = 40;
            item->color = BLUE;
            break;
        case POTION_STRENGTH:
            strcpy(item->name, "Strength Potion");
            strcpy(item->description, "Temporarily boosts strength.");
            item->effectValue = 5;
            item->effectDuration = 60.0f;
            item->value = 60;
            item->color = MAROON;
            break;
        case POTION_SPEED:
            strcpy(item->name, "Speed Potion");
            strcpy(item->description, "Temporarily increases movement speed.");
            item->effectValue = 2;
            item->effectDuration = 60.0f;
            item->value = 60;
            item->color = LIME;
            break;
        case POTION_INVISIBILITY:
            strcpy(item->name, "Invisibility Potion");
            strcpy(item->description, "Temporarily makes you harder to detect.");
            item->effectDuration = 30.0f;
            item->value = 100;
            item->color = SKYBLUE;
            break;
        default:
            strcpy(item->name, "Unknown Potion");
            strcpy(item->description, "A mysterious liquid in a bottle.");
            item->effectValue = 10;
            item->value = 20;
            item->color = PURPLE;
            break;
    }
}

// Configure scroll properties
void ConfigureScroll(Item* item) {
    strcpy(item->name, "Magic Scroll");
    strcpy(item->description, "A scroll with magical properties.");
    item->effectValue = 50;
    item->value = 75;
    item->color = BEIGE;
}

// Configure key properties
void ConfigureKey(Item* item) {
    strcpy(item->name, "Dungeon Key");
    strcpy(item->description, "A key to unlock doors in the dungeon.");
    item->value = 50;
    item->color = GOLD;
}

// Configure gold properties
void ConfigureGold(Item* item) {
    int amount = (item->level * 10) + GetRandomValue(1, 20);
    sprintf(item->name, "%d Gold", amount);
    strcpy(item->description, "Shiny gold coins.");
    item->value = amount;
    item->color = GOLD;
}

// Apply level scaling to item stats
void ApplyLevelScaling(Item* item) {
    // Scale stats based on item level
    float levelMultiplier = 1.0f + (item->level * 0.2f);
    
    // Apply scaling based on item type
    switch(item->type) {
        case ITEM_WEAPON:
            item->minDamage = (int)(item->minDamage * levelMultiplier);
            item->maxDamage = (int)(item->maxDamage * levelMultiplier);
            item->strength = (int)(item->strength * levelMultiplier);
            item->dexterity = (int)(item->dexterity * levelMultiplier);
            item->intelligence = (int)(item->intelligence * levelMultiplier);
            break;
        case ITEM_ARMOR:
            item->armor = (int)(item->armor * levelMultiplier);
            item->strength = (int)(item->strength * levelMultiplier);
            item->dexterity = (int)(item->dexterity * levelMultiplier);
            item->intelligence = (int)(item->intelligence * levelMultiplier);
            break;
        case ITEM_POTION:
            item->effectValue = (int)(item->effectValue * levelMultiplier);
            break;
        default:
            break;
    }
    
    // Scale value with level
    item->value = (int)(item->value * levelMultiplier);
    
    // Add level to name for higher level items
    if (item->level > 1) {
        char leveledName[64];
        sprintf(leveledName, "%s +%d", item->name, item->level);
        strcpy(item->name, leveledName);
    }
}

// Create a model for the item
void CreateItemModel(Item* item) {
    // Create different shapes based on item type
    switch(item->type) {
        case ITEM_WEAPON:
            if (item->subType == WEAPON_SWORD) {
                item->model = LoadModelFromMesh(GenMeshCube(0.1f, 0.8f, 0.1f));
            } else if (item->subType == WEAPON_AXE) {
                item->model = LoadModelFromMesh(GenMeshCube(0.3f, 0.6f, 0.1f));
            } else if (item->subType == WEAPON_STAFF) {
                item->model = LoadModelFromMesh(GenMeshCylinder(0.05f, 1.0f, 8));
            } else {
                item->model = LoadModelFromMesh(GenMeshCube(0.2f, 0.6f, 0.2f));
            }
            break;
        case ITEM_ARMOR:
            item->model = LoadModelFromMesh(GenMeshCube(0.4f, 0.4f, 0.4f));
            break;
        case ITEM_POTION:
            item->model = LoadModelFromMesh(GenMeshCylinder(0.1f, 0.3f, 8));
            break;
        case ITEM_SCROLL:
            item->model = LoadModelFromMesh(GenMeshCylinder(0.1f, 0.3f, 8));
            break;
        case ITEM_GOLD:
            item->model = LoadModelFromMesh(GenMeshSphere(0.15f, 8, 8));
            break;
        default:
            item->model = LoadModelFromMesh(GenMeshCube(0.3f, 0.3f, 0.3f));
            break;
    }
    
    // Set the model color
    item->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = item->color;
    
    // Create a simple icon for the item
    Image iconImage = GenImageColor(40, 40, item->color);
    item->icon = LoadTextureFromImage(iconImage);
    UnloadImage(iconImage);
}

// Update item state
void UpdateItem(Item* item, float deltaTime) {
    if (item->isOnGround) {
        // Make the item bob up and down
        item->bobTimer += deltaTime * 2.0f;
        item->bobHeight = sinf(item->bobTimer) * 0.1f;
        
        // Make the item rotate slowly
        item->rotationAngle += 45.0f * deltaTime;
        if (item->rotationAngle > 360.0f) {
            item->rotationAngle -= 360.0f;
        }
    }
}

// Draw item in the world
void DrawItem(Item* item) {
    if (item->isOnGround) {
        // Calculate position with bobbing effect
        Vector3 drawPosition = {
            item->position.x,
            item->position.y + 0.5f + item->bobHeight,
            item->position.z
        };
        
        // Draw the item with rotation
        DrawModelEx(item->model, 
                   drawPosition, 
                   (Vector3){0.0f, 1.0f, 0.0f}, 
                   item->rotationAngle, 
                   (Vector3){1.0f, 1.0f, 1.0f}, 
                   WHITE);
    }
}

// Draw item icon in UI
void DrawItemIcon(Item* item, Rectangle bounds) {
    if (item->icon.id != 0) {
        DrawTexture(item->icon, bounds.x, bounds.y, WHITE);
    } else {
        // Fallback if icon is missing
        DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, item->color);
    }
    
    // Draw border
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GRAY);
}

// Unload item resources
void UnloadItem(Item* item) {
    UnloadModel(item->model);
    if (item->icon.id != 0) {
        UnloadTexture(item->icon);
    }
}

// Generate a random item appropriate for the given level
Item GenerateRandomItem(int level) {
    Item item;
    
    // Randomize item type with weighted probabilities
    int typeRoll = GetRandomValue(1, 100);
    ItemType type;
    int subType = 0;
    
    if (typeRoll <= 35) {
        // 35% chance for weapon
        type = ITEM_WEAPON;
        subType = GetRandomValue(0, 4); // Random weapon type
    } else if (typeRoll <= 65) {
        // 30% chance for armor
        type = ITEM_ARMOR;
        subType = GetRandomValue(0, 3); // Random armor type
    } else if (typeRoll <= 85) {
        // 20% chance for potion
        type = ITEM_POTION;
        subType = GetRandomValue(0, 4); // Random potion type
    } else if (typeRoll <= 90) {
        // 5% chance for accessory
        type = ITEM_ACCESSORY;
    } else if (typeRoll <= 95) {
        // 5% chance for scroll
        type = ITEM_SCROLL;
    } else {
        // 5% chance for gold
        type = ITEM_GOLD;
    }
    
    // Random position (this would typically be set when the item is spawned)
    Vector3 position = {0.0f, 0.0f, 0.0f};
    
    // Initialize the item
    InitItem(&item, type, subType, level, position);
    
    return item;
}

// Get item name
const char* GetItemName(Item* item) {
    return item->name;
}

// Get item description
const char* GetItemDescription(Item* item) {
    return item->description;
}

// Use consumable item
bool UseItem(Item* item) {
    // Check if item is usable
    if (item->type == ITEM_POTION || item->type == ITEM_SCROLL) {
        // Item is used
        return true;
    }
    
    // Item can't be used directly
    return false;
}

// Get item damage (for weapons)
int GetItemDamage(Item* item) {
    if (item->type == ITEM_WEAPON) {
        // Return a random value between min and max damage
        return GetRandomValue(item->minDamage, item->maxDamage);
    }
    
    return 0;
}

// Get item armor value
int GetItemArmor(Item* item) {
    if (item->type == ITEM_ARMOR) {
        return item->armor;
    }
    
    return 0;
}
