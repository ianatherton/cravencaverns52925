#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game.h"

// UI element types
typedef enum {
    UI_TEXT,
    UI_BUTTON,
    UI_SLIDER,
    UI_PROGRESS_BAR,
    UI_IMAGE,
    UI_PANEL
} UIElementType;

// UI element structure
typedef struct {
    UIElementType type;
    Rectangle bounds;
    Color color;
    Color textColor;
    char text[128];
    Texture2D texture;
    bool isActive;
    bool isHovered;
    bool isPressed;
    float value;        // For progress bars and sliders
    float minValue;     // For sliders
    float maxValue;     // For sliders
} UIElement;

// UI functions
void DrawUI(GameState* gameState);
void DrawHealthBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color);
void DrawStaminaBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color);
void DrawManaBar(GameState* gameState, Rectangle bounds, float value, float maxValue, Color color);
void DrawInventory(GameState* gameState);
void DrawEquipment(GameState* gameState);
void DrawStats(GameState* gameState);
void DrawMinimapUI(GameState* gameState);
void DrawGameMenu(GameState* gameState);
bool UIButtonPressed(UIElement button, Vector2 mousePosition, bool mousePressed);
void DrawUIElement(UIElement element);
void DrawTooltip(const char* text, Vector2 position);
void DrawItemSlot(Rectangle bounds, Texture2D texture, bool isSelected);
void CreateFallbackTextures(GameState* gameState);

#endif // UI_H
