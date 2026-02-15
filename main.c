#include "raylib.h"

int main() {
    InitWindow(800, 600, "Hello, World!");

    while (!WindowShouldClose()) {
	BeginDrawing();
	ClearBackground(RAYWHITE);
	DrawText("Hello, World!", 400, 300, 20, BLACK);
	EndDrawing();
    }

    CloseWindow();
    return 0;
}
