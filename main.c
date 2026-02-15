#include "raylib.h"
#include "grid/grid.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WINDOW_SIZE 750
#define GRID_SLICES 50
#define NEURON_PROB 0.025f

int main() {
    srand(time(NULL));

    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Neural Network Structure Simulator");
    initGrid(WINDOW_SIZE, GRID_SLICES);
    scatterNeurons(NEURON_PROB);

    bool show_grid = true;

    while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (IsKeyPressed(KEY_R)) {
        freeGrid();
        initGrid(WINDOW_SIZE, GRID_SLICES);
        scatterNeurons(NEURON_PROB);
    }

    if (IsKeyPressed(KEY_H)) {
        show_grid = !show_grid;
    }

    if (IsKeyPressed(KEY_I)) {
        expungeGaussian();
    }
    
    if (show_grid) {
        colorGrid();
    }
    
    EndDrawing();
    }

    freeGrid();
    CloseWindow();
    return 0;
}
