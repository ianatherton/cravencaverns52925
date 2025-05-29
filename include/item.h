#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"

// Item types
typedef enum {
    ITEM_NONE,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_ACCESSORY,
    ITEM_POTION,
    ITEM_SCROLL,
    ITEM_KEY,
    ITEM_GOLD
} ItemType;

// Weapon types
typedef enum {
    WEAPON_SWORD,
    WEAPON_AXE,
    WEAPON_MACE,
    WEAPON_STAFF,
    WEAPON_BOW
} WeaponType;

// Armor types
typedef enum {
    ARMOR_HELMET,
    ARMOR_CHEST,
    ARMOR_GLOVES,
    ARMOR_BOOTS
} ArmorType;

// Potion types
typedef enum {
    POTION_HEALTH,
    POTION_MANA,
    POTION_STRENGTH,
    POTION_SPEED,
    POTION_INVISIBILITY
} PotionType;

// Item structure
typedef struct Item {
    // Basic item properties
    char name[64];
    char description[256];
    ItemType type;
    int subType;
    int level;
    int value;
    bool isEquipped;
    bool isIdentified;
    
    // Combat stats (for equipment)
    int minDamage;
    int maxDamage;
    int armor;
    int strength;
    int dexterity;
    int intelligence;
    
    // Effect properties (for consumables)
    int effectValue;
    float effectDuration;
    
    // Visual representation
    Model model;
    Texture2D icon;
    Color color;
    
    // World properties
    Vector3 position;
    float rotationAngle;
    bool isOnGround;
    float bobHeight;
    float bobTimer;
} Item;

// Item functions
void InitItem(Item* item, ItemType type, int subType, int level, Vector3 position);
void UpdateItem(Item* item, float deltaTime);
void DrawItem(Item* item);
void DrawItemIcon(Item* item, Rectangle bounds);
void UnloadItem(Item* item);
Item GenerateRandomItem(int level);
const char* GetItemName(Item* item);
const char* GetItemDescription(Item* item);
bool UseItem(Item* item);
int GetItemDamage(Item* item);
int GetItemArmor(Item* item);

#endif // ITEM_H
