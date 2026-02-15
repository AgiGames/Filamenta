#include "grid.h"
#include "raylib.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "../helper/helper.h"

size_t window_size_g = 750;
size_t slices_g = 50;
float spacing_g = 15.0f;

float** grid_values = NULL;

void initGrid(size_t window_size, size_t slices) {
    window_size_g = window_size;
    spacing_g = (float) window_size / slices;
    grid_values = (float**) calloc(slices, sizeof(float*));

    for (size_t i = 0; i < slices; ++i) {
        grid_values[i] = (float*) calloc(slices, sizeof(float));
    }
}

void scatterNeurons(float neuron_prob) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float random_number = (float) rand() / RAND_MAX;
            if (random_number > neuron_prob) continue;

            grid_values[i][j] = 1;
        }
    }
}

void drawGrid(Color color) {
    float x = 0;
    for (size_t j = 0; j <= slices_g; ++j, x += spacing_g) {
        Vector2 startPos = {.x = x, .y = 0};
        Vector2 endPos = {.x = x, .y = window_size_g};

        DrawLineV(startPos, endPos, color);
    }

    float y = 0;
    for (size_t i = 0; i <= slices_g; ++i, y += spacing_g) {
        Vector2 startPos = {.x = 0, .y = y};
        Vector2 endPos = {.x = window_size_g, .y = y};

        DrawLineV(startPos, endPos, color);
    }
}

void freeGrid() {
    if (grid_values == NULL) {
        return;
    }

    for (size_t i = 0; i < slices_g; ++i) {
        free(grid_values[i]);
    }

    free(grid_values);
}

Color heatmapCmap(float intensity) {
    if (intensity == 1.0f) {
        return WHITE;
    }

    if (intensity < 0.5f) {
        return (Color) {255 * (intensity * 2), 0, 0, 255};
    }
    return (Color) {255, 255 * ((intensity - 0.5) * 2), 0, 255};
}

void expungeGaussian() {
    Vector2DA means = {0};
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            if (grid_values[i][j] == 1.0f) {
                float mean_x = (float) j;
                float mean_y = (float) i;

                Vector2 mean_pair = {.x = mean_x, .y = mean_y};
                da_append(means, mean_pair);
            }
        }
    }

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            for (size_t k = 0; k < means.count; ++k) {
                Vector2 mean_pair = means.items[k];
                grid_values[i][j] = fmin(1.0f,
                        grid_values[i][j] + gaussian2d_1std((float) j, (float) i, mean_pair.x, mean_pair.y)
                        );
            }
        }
    }
}

void colorGrid() {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            // i corresponds to y axis coordinate
            // j corresponds to x axis coordinate

            float x = j * spacing_g;
            float y = i * spacing_g;

            DrawRectangle(x, y, spacing_g, spacing_g, heatmapCmap(grid_values[i][j]));
        }
    }
}
