#include "raylib.h"
#include "grid/grid.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_SIZE 750
#define GRID_SLICES 50
#define ACCUMULATOR_PROB 0.025f

int main() {
    srand(time(NULL));

    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Filamenta");
    init_grid(WINDOW_SIZE, GRID_SLICES);
    scatter_accumulators(ACCUMULATOR_PROB);

    bool show_grid = true;
    bool color_accumulators = true;
    bool show_connections = true;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_R)) {
            free_grid();
            init_grid(WINDOW_SIZE, GRID_SLICES);
            scatter_accumulators(ACCUMULATOR_PROB);
        }

        if (IsKeyPressed(KEY_H)) {
            show_grid = !show_grid;
        }

        if (IsKeyPressed(KEY_N)) {
            color_accumulators = !color_accumulators;
        }

        if (IsKeyPressed(KEY_C)) {
            show_connections = !show_connections;
        }

        if (IsKeyPressed(KEY_I)) {
            expunge_gaussian();
            create_accumulators();
        }

        if (IsKeyPressed(KEY_E)) {
            expunge_gaussian();
        }

        if (show_grid) {
            color_grid(color_accumulators);
        }

        if (show_connections) {
            connect_accumulators();
        }

        const char* num_accumulators_text = TextFormat("%zu", get_num_accumulators());
        DrawText(num_accumulators_text, 10, 10, 50, GREEN);

        EndDrawing();
    }

    free_grid();
    CloseWindow();
    return 0;
}
