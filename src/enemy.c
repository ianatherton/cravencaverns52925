#include "../include/enemy.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

// Initialize enemy with given type, position, and level
void InitEnemy(Enemy* enemy, EnemyType type, Vector3 position, int level) {
    enemy->type = type;
    enemy->state = ENEMY_IDLE;
    enemy->position = position;
    enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
    enemy->direction = (Vector3){0.0f, 0.0f, 1.0f};
    enemy->spawnPosition = position;
    enemy->rotationAngle = 0.0f;
    enemy->level = level;
    enemy->isAlive = true;
    
    // Configure enemy properties based on type
    switch (type) {
        case ENEMY_SKELETON_BASIC:
            enemy->speed = 2.0f + (level * 0.1f);
            enemy->health = 20 + (level * 5);
            enemy->maxHealth = enemy->health;
            enemy->attackDamage = 5 + (level * 2);
            enemy->attackRange = 1.2f;
            enemy->detectionRange = 8.0f;
            enemy->patrolRadius = 5.0f;
            enemy->experienceValue = 10 + (level * 3);
            enemy->attackCooldown = 1.5f;
            enemy->height = 1.7f;
            enemy->radius = 0.4f;
            enemy->dropChance = 0.2f + (level * 0.03f);
            enemy->minGold = 1;
            enemy->maxGold = 5 + level;
            break;
            
        case ENEMY_SKELETON_WARRIOR:
            enemy->speed = 1.5f + (level * 0.1f);
            enemy->health = 40 + (level * 8);
            enemy->maxHealth = enemy->health;
            enemy->attackDamage = 8 + (level * 3);
            enemy->attackRange = 1.5f;
            enemy->detectionRange = 7.0f;
            enemy->patrolRadius = 4.0f;
            enemy->experienceValue = 20 + (level * 5);
            enemy->attackCooldown = 2.0f;
            enemy->height = 1.8f;
            enemy->radius = 0.5f;
            enemy->dropChance = 0.3f + (level * 0.04f);
            enemy->minGold = 3;
            enemy->maxGold = 10 + (level * 2);
            break;
            
        case ENEMY_SKELETON_ARCHER:
            enemy->speed = 2.5f + (level * 0.15f);
            enemy->health = 15 + (level * 4);
            enemy->maxHealth = enemy->health;
            enemy->attackDamage = 6 + (level * 2);
            enemy->attackRange = 8.0f; // Ranged attack
            enemy->detectionRange = 12.0f;
            enemy->patrolRadius = 6.0f;
            enemy->experienceValue = 15 + (level * 4);
            enemy->attackCooldown = 3.0f;
            enemy->height = 1.7f;
            enemy->radius = 0.4f;
            enemy->dropChance = 0.25f + (level * 0.03f);
            enemy->minGold = 2;
            enemy->maxGold = 8 + level;
            break;
            
        case ENEMY_SKELETON_MAGE:
            enemy->speed = 1.8f + (level * 0.1f);
            enemy->health = 12 + (level * 3);
            enemy->maxHealth = enemy->health;
            enemy->attackDamage = 10 + (level * 4);
            enemy->attackRange = 6.0f; // Ranged attack
            enemy->detectionRange = 10.0f;
            enemy->patrolRadius = 5.0f;
            enemy->experienceValue = 25 + (level * 6);
            enemy->attackCooldown = 4.0f;
            enemy->height = 1.7f;
            enemy->radius = 0.4f;
            enemy->dropChance = 0.4f + (level * 0.05f);
            enemy->minGold = 5;
            enemy->maxGold = 15 + (level * 3);
            break;
            
        default:
            break;
    }
    
    // Reset timers
    enemy->attackTimer = 0.0f;
    enemy->pathfindTimer = 0.0f;
    enemy->pathfindInterval = 1.0f;
    enemy->stateTimer = 0.0f;
    enemy->animTimer = 0.0f;
    enemy->isAttacking = false;
    
    // Create a placeholder model based on enemy type
    switch (type) {
        case ENEMY_SKELETON_BASIC:
            enemy->model = LoadModelFromMesh(GenMeshCube(0.8f, 1.7f, 0.8f));
            break;
        case ENEMY_SKELETON_WARRIOR:
            enemy->model = LoadModelFromMesh(GenMeshCube(1.0f, 1.8f, 1.0f));
            break;
        case ENEMY_SKELETON_ARCHER:
            enemy->model = LoadModelFromMesh(GenMeshCube(0.8f, 1.7f, 0.8f));
            break;
        case ENEMY_SKELETON_MAGE:
            enemy->model = LoadModelFromMesh(GenMeshCube(0.8f, 1.7f, 0.8f));
            break;
        default:
            enemy->model = LoadModelFromMesh(GenMeshCube(0.8f, 1.7f, 0.8f));
            break;
    }
    
    // Apply different colors to different enemy types
    Color enemyColor = WHITE;
    switch (type) {
        case ENEMY_SKELETON_BASIC:
            enemyColor = GRAY;
            break;
        case ENEMY_SKELETON_WARRIOR:
            enemyColor = DARKGRAY;
            break;
        case ENEMY_SKELETON_ARCHER:
            enemyColor = BROWN;
            break;
        case ENEMY_SKELETON_MAGE:
            enemyColor = PURPLE;
            break;
        default:
            break;
    }
    
    // Set model material color
    enemy->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = enemyColor;
}

// Update enemy behavior
void UpdateEnemy(Enemy* enemy, Vector3 playerPosition, float deltaTime, bool canSeePlayer) {
    if (!enemy->isAlive) return;
    
    // Update timers
    enemy->attackTimer += deltaTime;
    enemy->pathfindTimer += deltaTime;
    enemy->stateTimer += deltaTime;
    enemy->animTimer += deltaTime;
    
    // Calculate distance to player
    Vector3 toPlayer = Vector3Subtract(playerPosition, enemy->position);
    float distanceToPlayer = Vector3Length(toPlayer);
    
    // Decide state based on player position and visibility
    if (distanceToPlayer <= enemy->attackRange && canSeePlayer) {
        // If player is in attack range and visible
        enemy->state = ENEMY_ATTACKING;
    } else if (distanceToPlayer <= enemy->detectionRange && canSeePlayer) {
        // If player is detected but not in attack range
        enemy->state = ENEMY_CHASING;
    } else if (enemy->state == ENEMY_CHASING) {
        // If was chasing but lost sight of player
        if (enemy->stateTimer > 5.0f) {
            // After 5 seconds, go back to patrolling
            enemy->state = ENEMY_PATROLLING;
            enemy->stateTimer = 0.0f;
        }
    } else if (enemy->state == ENEMY_IDLE) {
        // If idle for too long, start patrolling
        if (enemy->stateTimer > 3.0f) {
            enemy->state = ENEMY_PATROLLING;
            enemy->stateTimer = 0.0f;
        }
    } else if (enemy->state == ENEMY_PATROLLING) {
        // Continue patrolling
        if (enemy->stateTimer > 10.0f) {
            // After patrolling for a while, take a rest
            enemy->state = ENEMY_IDLE;
            enemy->stateTimer = 0.0f;
        }
    }
    
    // Handle behavior based on state
    switch (enemy->state) {
        case ENEMY_IDLE:
            // No movement in idle state
            enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
            break;
            
        case ENEMY_PATROLLING:
            // Patrol around spawn position
            if (enemy->pathfindTimer >= enemy->pathfindInterval) {
                // Choose a random direction within patrol radius
                float patrolAngle = GetRandomValue(0, 360) * DEG2RAD;
                float patrolDistance = GetRandomValue(0, (int)(enemy->patrolRadius * 100)) / 100.0f;
                
                Vector3 patrolTarget = {
                    enemy->spawnPosition.x + cosf(patrolAngle) * patrolDistance,
                    0.0f,
                    enemy->spawnPosition.z + sinf(patrolAngle) * patrolDistance
                };
                
                // Calculate direction to patrol target
                Vector3 toTarget = Vector3Subtract(patrolTarget, enemy->position);
                if (Vector3Length(toTarget) > 0.1f) {
                    enemy->direction = Vector3Normalize(toTarget);
                }
                
                enemy->pathfindTimer = 0.0f;
            }
            
            // Move in the patrol direction
            enemy->velocity.x = enemy->direction.x * enemy->speed * 0.5f;
            enemy->velocity.z = enemy->direction.z * enemy->speed * 0.5f;
            break;
            
        case ENEMY_CHASING:
            // Calculate direction to player
            if (distanceToPlayer > 0.1f) {
                enemy->direction = Vector3Normalize(toPlayer);
            }
            
            // Move towards player
            enemy->velocity.x = enemy->direction.x * enemy->speed;
            enemy->velocity.z = enemy->direction.z * enemy->speed;
            break;
            
        case ENEMY_ATTACKING:
            // Face the player
            if (distanceToPlayer > 0.1f) {
                enemy->direction = Vector3Normalize(toPlayer);
            }
            
            // Stop moving while attacking
            enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
            
            // Attempt to attack if cooldown has passed
            if (enemy->attackTimer >= enemy->attackCooldown) {
                enemy->isAttacking = true;
                enemy->attackTimer = 0.0f;
            } else {
                enemy->isAttacking = false;
            }
            break;
            
        case ENEMY_STUNNED:
            // No movement while stunned
            enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
            
            // Recover from stun after a while
            if (enemy->stateTimer > 1.0f) {
                enemy->state = ENEMY_IDLE;
                enemy->stateTimer = 0.0f;
            }
            break;
            
        case ENEMY_DEAD:
            // No updates for dead enemies
            enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
            break;
            
        default:
            break;
    }
    
    // Update position based on velocity
    enemy->position.x += enemy->velocity.x * deltaTime;
    enemy->position.z += enemy->velocity.z * deltaTime;
    
    // Update rotation to face direction of movement
    if (Vector3Length(enemy->velocity) > 0.1f) {
        enemy->rotationAngle = atan2f(enemy->direction.z, enemy->direction.x) * RAD2DEG;
    }
}

// Draw enemy
void DrawEnemy(Enemy* enemy, Camera camera)
{
    if (enemy->isAlive) {
        // Draw the enemy model with appropriate rotation
        DrawModelEx(enemy->model, 
                   enemy->position, 
                   (Vector3){0.0f, 1.0f, 0.0f}, 
                   enemy->rotationAngle, 
                   (Vector3){1.0f, 1.0f, 1.0f}, 
                   WHITE);
    
        // Draw health bar above enemy (if not at full health)
        if (enemy->health < enemy->maxHealth && enemy->health > 0) {
            // Convert 3D position to 2D screen space
            Vector2 screenPos = GetWorldToScreen(
                (Vector3){enemy->position.x, enemy->position.y + enemy->height + 0.3f, enemy->position.z}, 
                camera);
            
            // Draw health bar
            float healthRatio = (float)enemy->health / (float)enemy->maxHealth;
            float barWidth = 30.0f;
            float barHeight = 5.0f;
            
            DrawRectangle(screenPos.x - barWidth/2, screenPos.y, barWidth, barHeight, DARKGRAY);
            DrawRectangle(screenPos.x - barWidth/2, screenPos.y, barWidth * healthRatio, barHeight, RED);
            DrawRectangleLines(screenPos.x - barWidth/2, screenPos.y, barWidth, barHeight, BLACK);
        }
    }
}

// Enemy takes damage
void EnemyTakeDamage(Enemy* enemy, int damage) {
    enemy->health -= damage;
    
    // Check if enemy died
    if (enemy->health <= 0) {
        enemy->health = 0;
        enemy->isAlive = false;
        enemy->state = ENEMY_DEAD;
    } else {
        // Enemy is stunned briefly when hit
        enemy->state = ENEMY_STUNNED;
        enemy->stateTimer = 0.0f;
    }
}

// Enemy attacks player
bool EnemyAttack(Enemy* enemy, Vector3 playerPosition, float playerRadius, int* damageDealt) {
    if (!enemy->isAlive || !enemy->isAttacking) {
        return false;
    }
    
    // Reset attack flag
    enemy->isAttacking = false;
    
    // Calculate distance to player
    Vector3 toPlayer = Vector3Subtract(playerPosition, enemy->position);
    float distance = Vector3Length(toPlayer);
    
    // Check if player is in attack range
    if (distance <= enemy->attackRange + playerRadius) {
        // For ranged enemies (archer, mage), we might want additional checks
        // like line of sight, but we'll keep it simple for now
        
        // Attack hits!
        *damageDealt = enemy->attackDamage;
        return true;
    }
    
    return false;
}

// Get random loot from enemy
int GetRandomEnemyLoot(Enemy* enemy) {
    // Simple implementation returning gold amount
    return GetRandomValue(enemy->minGold, enemy->maxGold);
}

// Unload enemy resources
void UnloadEnemy(Enemy* enemy) {
    // Unload model
    UnloadModel(enemy->model);
}
