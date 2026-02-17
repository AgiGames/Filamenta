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

    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Filamenta: Density-Driven Structure Simulator");
    initGrid(WINDOW_SIZE, GRID_SLICES);
    scatterAccumulators(ACCUMULATOR_PROB);

    bool show_grid = true;
    bool color_accumulators = true;
    bool show_connections = true;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_R)) {
            freeGrid();
            initGrid(WINDOW_SIZE, GRID_SLICES);
            scatterAccumulators(ACCUMULATOR_PROB);
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
            expungeGaussian();
            createAccumulators();
        }

        if (IsKeyPressed(KEY_E)) {
            expungeGaussian();
        }

        if (show_grid) {
            colorGrid(color_accumulators);
        }

        if (show_connections) {
            connectAccumulators();
        }

        const char* accumulators_created_text = TextFormat("%zu", numNewAccumulators());
        DrawText(accumulators_created_text, 10, 10, 50, GREEN);

        EndDrawing();
    }

    freeGrid();
    CloseWindow();
    return 0;
}
