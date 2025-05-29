#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "item.h"

// Player stats
typedef struct {
    int health;
    int maxHealth;
    int stamina;
    int maxStamina;
    int mana;
    int maxMana;
    int level;
    int experience;
    int experienceToNextLevel;
    int strength;
    int dexterity;
    int intelligence;
} PlayerStats;

// Player inventory
typedef struct {
    Item* items;
    int itemCount;
    int maxItems;
    int equippedWeapon;
    int equippedArmor;
    int equippedAccessory;
} Inventory;

// Player structure
typedef struct Player {
    // Player position and movement
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    float speed;
    float jumpForce;
    bool isJumping;
    bool isGrounded;
    float height;
    float radius;
    
    // Player stats
    PlayerStats stats;
    
    // Player inventory
    Inventory inventory;
    
    // Player attack properties
    float attackTimer;
    float attackCooldown;
    bool isAttacking;
    float attackRange;
    int attackDamage;
    
    // Player model and animations
    Model model;
    float rotationAngle;
    
    // Camera offset from player position
    Vector3 cameraOffset;
    float cameraHeight;
    
    // HUD elements
    Texture2D healthBar;
    Texture2D staminaBar;
    Texture2D manaBar;
    Texture2D itemSlot;
    Texture2D unknownItem;
} Player;

// Player functions
void InitPlayer(Player* player);
void UpdatePlayer(Player* player, float deltaTime);
void DrawPlayer(Player* player);
void UnloadPlayer(Player* player);
void PlayerAttack(Player* player);
void PlayerTakeDamage(Player* player, int damage);
void PlayerHeal(Player* player, int amount);
void PlayerLevelUp(Player* player);
void PlayerAddExperience(Player* player, int amount);
void PlayerAddItem(Player* player, Item item);
void PlayerEquipItem(Player* player, int itemIndex);
void PlayerUseItem(Player* player, int itemIndex);

#endif // PLAYER_H
