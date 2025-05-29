#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

// Enemy types
typedef enum {
    ENEMY_SKELETON_BASIC,
    ENEMY_SKELETON_WARRIOR,
    ENEMY_SKELETON_ARCHER,
    ENEMY_SKELETON_MAGE,
    ENEMY_COUNT
} EnemyType;

// Enemy states
typedef enum {
    ENEMY_IDLE,
    ENEMY_PATROLLING,
    ENEMY_CHASING,
    ENEMY_ATTACKING,
    ENEMY_STUNNED,
    ENEMY_DEAD
} EnemyState;

// Enemy structure
typedef struct Enemy {
    // Enemy type and state
    EnemyType type;
    EnemyState state;
    
    // Position and movement
    Vector3 position;
    Vector3 velocity;
    Vector3 direction;
    float speed;
    float patrolRadius;
    Vector3 spawnPosition;
    float rotationAngle;
    
    // Physical properties
    float height;
    float radius;
    
    // Stats
    int health;
    int maxHealth;
    int attackDamage;
    float attackRange;
    float detectionRange;
    int experienceValue;
    int level;
    
    // Combat properties
    float attackTimer;
    float attackCooldown;
    bool isAttacking;
    
    // Visual representation
    Model model;
    
    // AI properties
    float pathfindTimer;
    float pathfindInterval;
    float stateTimer;
    
    // Loot drop chances
    float dropChance;
    int minGold;
    int maxGold;
    
    // Animation properties
    float animTimer;
    bool isAlive;
} Enemy;

// Enemy functions
void InitEnemy(Enemy* enemy, EnemyType type, Vector3 position, int level);
void UpdateEnemy(Enemy* enemy, Vector3 playerPosition, float deltaTime, bool canSeePlayer);
void DrawEnemy(Enemy* enemy, Camera camera);
void UnloadEnemy(Enemy* enemy);
void EnemyTakeDamage(Enemy* enemy, int damage);
bool EnemyAttack(Enemy* enemy, Vector3 playerPosition, float playerRadius, int* damageDealt);
int GetRandomEnemyLoot(Enemy* enemy);

#endif // ENEMY_H
