#include "../include/player.h"
#include "../include/dungeon.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

// Initialize player
void InitPlayer(Player* player) {
    // Set default position
    player->position = (Vector3){0.0f, 1.8f, 0.0f};
    player->velocity = (Vector3){0.0f, 0.0f, 0.0f};
    player->direction = (Vector3){0.0f, 0.0f, 1.0f};
    player->speed = 5.0f;
    player->jumpForce = 8.0f;
    player->isJumping = false;
    player->isGrounded = true;
    player->height = 1.8f;
    player->radius = 0.4f;
    player->rotationAngle = 0.0f;
    
    // Set camera properties
    player->cameraOffset = (Vector3){0.0f, 0.0f, 0.0f};
    player->cameraHeight = 1.7f;
    
    // Initialize stats
    player->stats.health = 100;
    player->stats.maxHealth = 100;
    player->stats.stamina = 100;
    player->stats.maxStamina = 100;
    player->stats.mana = 100;
    player->stats.maxMana = 100;
    player->stats.level = 1;
    player->stats.experience = 0;
    player->stats.experienceToNextLevel = 100;
    player->stats.strength = 10;
    player->stats.dexterity = 10;
    player->stats.intelligence = 10;
    
    // Initialize combat properties
    player->attackTimer = 0.0f;
    player->attackCooldown = 0.5f;
    player->isAttacking = false;
    player->attackRange = 2.0f;
    player->attackDamage = 10;
    
    // Initialize inventory
    player->inventory.maxItems = 20;
    player->inventory.items = (Item*)malloc(sizeof(Item) * player->inventory.maxItems);
    player->inventory.itemCount = 0;
    player->inventory.equippedWeapon = -1;
    player->inventory.equippedArmor = -1;
    player->inventory.equippedAccessory = -1;
    
    // Create a fallback model using a cube primitive for now
    player->model = LoadModelFromMesh(GenMeshCube(0.8f, 1.8f, 0.8f));
    
    // Initialize UI textures as NULL - they'll be loaded or created as fallbacks
    player->healthBar = (Texture2D){0};
    player->staminaBar = (Texture2D){0};
    player->manaBar = (Texture2D){0};
    player->itemSlot = (Texture2D){0};
    player->unknownItem = (Texture2D){0};
}

// Update player state
void UpdatePlayer(Player* player, float deltaTime) {
    // Store previous position for collision checks
    Vector3 previousPosition = player->position;
    
    // Handle rotation with mouse (this would typically be handled in the main game loop)
    // This assumes that the mouse controls have been set up in the main loop
    
    // Process movement
    // Reset movement velocity
    player->velocity.x = 0;
    player->velocity.z = 0;
    
    // Get player input
    if (IsKeyDown(KEY_W)) {
        // Move forward
        player->velocity.x += player->direction.x * player->speed;
        player->velocity.z += player->direction.z * player->speed;
    }
    if (IsKeyDown(KEY_S)) {
        // Move backward
        player->velocity.x -= player->direction.x * player->speed;
        player->velocity.z -= player->direction.z * player->speed;
    }
    
    // Strafe left/right
    Vector3 right = Vector3CrossProduct(player->direction, (Vector3){0, 1, 0});
    right = Vector3Normalize(right);
    
    if (IsKeyDown(KEY_A)) {
        // Strafe left
        player->velocity.x -= right.x * player->speed;
        player->velocity.z -= right.z * player->speed;
    }
    if (IsKeyDown(KEY_D)) {
        // Strafe right
        player->velocity.x += right.x * player->speed;
        player->velocity.z += right.z * player->speed;
    }
    
    // Normalize velocity if moving diagonally to avoid faster diagonal movement
    if ((player->velocity.x != 0.0f) || (player->velocity.z != 0.0f)) {
        Vector3 velocity = {player->velocity.x, 0, player->velocity.z};
        velocity = Vector3Normalize(velocity);
        player->velocity.x = velocity.x * player->speed;
        player->velocity.z = velocity.z * player->speed;
    }
    
    // Apply gravity and jumping
    if (player->isGrounded) {
        player->velocity.y = 0;
        
        // Jump if space is pressed
        if (IsKeyPressed(KEY_SPACE)) {
            player->velocity.y = player->jumpForce;
            player->isGrounded = false;
            player->isJumping = true;
        }
    } else {
        // Apply gravity
        player->velocity.y -= 20.0f * deltaTime;
        
        // Check if we hit the ground
        if (player->position.y <= player->height / 2) {
            player->position.y = player->height / 2;
            player->isGrounded = true;
            player->isJumping = false;
        }
    }
    
    // Update position based on velocity
    player->position.x += player->velocity.x * deltaTime;
    player->position.y += player->velocity.y * deltaTime;
    player->position.z += player->velocity.z * deltaTime;
    
    // Note: Actual collision detection with dungeon would be handled in the game.c 
    // where the dungeon context is available
    
    // Update attack timer
    if (player->isAttacking) {
        player->attackTimer += deltaTime;
        
        if (player->attackTimer >= player->attackCooldown) {
            player->isAttacking = false;
            player->attackTimer = 0.0f;
        }
    }
    
    // Initiate attack with left mouse button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player->isAttacking) {
        PlayerAttack(player);
    }
    
    // Update stamina and mana regeneration
    if (player->stats.stamina < player->stats.maxStamina) {
        player->stats.stamina += 10.0f * deltaTime;
        if (player->stats.stamina > player->stats.maxStamina) {
            player->stats.stamina = player->stats.maxStamina;
        }
    }
    
    if (player->stats.mana < player->stats.maxMana) {
        player->stats.mana += 5.0f * deltaTime;
        if (player->stats.mana > player->stats.maxMana) {
            player->stats.mana = player->stats.maxMana;
        }
    }
}

// Perform attack action
void PlayerAttack(Player* player) {
    player->isAttacking = true;
    player->attackTimer = 0.0f;
    
    // Stamina cost for attack
    player->stats.stamina -= 10;
    if (player->stats.stamina < 0) player->stats.stamina = 0;
    
    // Note: Actual damage calculation and enemy hit detection
    // would be handled in the main game update
}

// Take damage
void PlayerTakeDamage(Player* player, int damage) {
    // Apply damage reduction based on armor and stats
    float damageReduction = 0.0f; // Could be calculated based on armor and stats
    int actualDamage = damage * (1.0f - damageReduction);
    
    if (actualDamage < 1) actualDamage = 1; // Minimum damage
    
    player->stats.health -= actualDamage;
    
    // Check if health drops below zero
    if (player->stats.health < 0) player->stats.health = 0;
}

// Heal player
void PlayerHeal(Player* player, int amount) {
    player->stats.health += amount;
    
    // Cap health at maximum
    if (player->stats.health > player->stats.maxHealth) {
        player->stats.health = player->stats.maxHealth;
    }
}

// Level up player
void PlayerLevelUp(Player* player) {
    player->stats.level++;
    
    // Increase stats
    player->stats.maxHealth += 10;
    player->stats.maxStamina += 5;
    player->stats.maxMana += 5;
    player->stats.strength += 2;
    player->stats.dexterity += 2;
    player->stats.intelligence += 2;
    
    // Fully restore health, stamina, and mana on level up
    player->stats.health = player->stats.maxHealth;
    player->stats.stamina = player->stats.maxStamina;
    player->stats.mana = player->stats.maxMana;
    
    // Increase experience required for next level
    player->stats.experienceToNextLevel = 100 * player->stats.level;
    
    // Reset current experience
    player->stats.experience = 0;
    
    // Update attack damage based on new stats
    player->attackDamage = 10 + (player->stats.strength / 2);
}

// Add experience points
void PlayerAddExperience(Player* player, int amount) {
    player->stats.experience += amount;
    
    // Check for level up
    if (player->stats.experience >= player->stats.experienceToNextLevel) {
        PlayerLevelUp(player);
    }
}

// Add item to inventory
void PlayerAddItem(Player* player, Item item) {
    if (player->inventory.itemCount < player->inventory.maxItems) {
        player->inventory.items[player->inventory.itemCount] = item;
        player->inventory.itemCount++;
    }
}

// Equip an item from inventory
void PlayerEquipItem(Player* player, int itemIndex) {
    if (itemIndex >= 0 && itemIndex < player->inventory.itemCount) {
        Item* item = &player->inventory.items[itemIndex];
        
        // Check item type and equip accordingly
        switch (item->type) {
            case ITEM_WEAPON:
                // Unequip previous weapon if any
                if (player->inventory.equippedWeapon >= 0) {
                    player->inventory.items[player->inventory.equippedWeapon].isEquipped = false;
                }
                
                // Equip new weapon
                item->isEquipped = true;
                player->inventory.equippedWeapon = itemIndex;
                
                // Update player attack damage based on weapon
                player->attackDamage = 10 + (player->stats.strength / 2) + GetItemDamage(item);
                break;
                
            case ITEM_ARMOR:
                // Unequip previous armor if any
                if (player->inventory.equippedArmor >= 0) {
                    player->inventory.items[player->inventory.equippedArmor].isEquipped = false;
                }
                
                // Equip new armor
                item->isEquipped = true;
                player->inventory.equippedArmor = itemIndex;
                break;
                
            case ITEM_ACCESSORY:
                // Unequip previous accessory if any
                if (player->inventory.equippedAccessory >= 0) {
                    player->inventory.items[player->inventory.equippedAccessory].isEquipped = false;
                }
                
                // Equip new accessory
                item->isEquipped = true;
                player->inventory.equippedAccessory = itemIndex;
                break;
                
            default:
                break;
        }
    }
}

// Use an item from inventory
void PlayerUseItem(Player* player, int itemIndex) {
    if (itemIndex >= 0 && itemIndex < player->inventory.itemCount) {
        Item* item = &player->inventory.items[itemIndex];
        
        // Check if item is usable
        if (item->type == ITEM_POTION) {
            switch (item->subType) {
                case POTION_HEALTH:
                    // Health potion
                    PlayerHeal(player, item->effectValue);
                    break;
                    
                case POTION_MANA:
                    // Mana potion
                    player->stats.mana += item->effectValue;
                    if (player->stats.mana > player->stats.maxMana) {
                        player->stats.mana = player->stats.maxMana;
                    }
                    break;
                    
                case POTION_STRENGTH:
                    // Strength potion (temporary buff)
                    player->stats.strength += item->effectValue;
                    // In a real implementation, you'd need to track buff duration
                    break;
                    
                case POTION_SPEED:
                    // Speed potion (temporary buff)
                    player->speed += item->effectValue;
                    // In a real implementation, you'd need to track buff duration
                    break;
                    
                default:
                    break;
            }
            
            // Remove item from inventory after use
            // This is a simple implementation - in a real game, you'd shift the array
            for (int i = itemIndex; i < player->inventory.itemCount - 1; i++) {
                player->inventory.items[i] = player->inventory.items[i + 1];
            }
            player->inventory.itemCount--;
            
            // Update equipped item indices if necessary
            if (player->inventory.equippedWeapon > itemIndex) {
                player->inventory.equippedWeapon--;
            } else if (player->inventory.equippedWeapon == itemIndex) {
                player->inventory.equippedWeapon = -1;
            }
            
            if (player->inventory.equippedArmor > itemIndex) {
                player->inventory.equippedArmor--;
            } else if (player->inventory.equippedArmor == itemIndex) {
                player->inventory.equippedArmor = -1;
            }
            
            if (player->inventory.equippedAccessory > itemIndex) {
                player->inventory.equippedAccessory--;
            } else if (player->inventory.equippedAccessory == itemIndex) {
                player->inventory.equippedAccessory = -1;
            }
        }
    }
}

// Draw player
void DrawPlayer(Player* player) {
    // In first-person mode, the player model is typically not visible
    // This function would be used in third-person mode or for shadows
    
    // Draw player model
    DrawModel(player->model, player->position, 1.0f, WHITE);
}

// Unload player resources
void UnloadPlayer(Player* player) {
    // Unload model
    UnloadModel(player->model);
    
    // Unload textures
    if (player->healthBar.id != 0) UnloadTexture(player->healthBar);
    if (player->staminaBar.id != 0) UnloadTexture(player->staminaBar);
    if (player->manaBar.id != 0) UnloadTexture(player->manaBar);
    if (player->itemSlot.id != 0) UnloadTexture(player->itemSlot);
    if (player->unknownItem.id != 0) UnloadTexture(player->unknownItem);
    
    // Free inventory
    free(player->inventory.items);
}
