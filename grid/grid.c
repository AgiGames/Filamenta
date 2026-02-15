#include "grid.h"
#include "raylib.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "../helper/helper.h"
#include <float.h>

size_t window_size_g = 750;
size_t slices_g = 50;
float spacing_g = 15.0f;

float** grid_values = NULL;
bool** is_neuron = NULL;
Vector2DA neurons = {0};

void initGrid(size_t window_size, size_t slices) {
    window_size_g = window_size;
    slices_g = slices;
    spacing_g = (float) window_size / slices;
    grid_values = (float**) calloc(slices, sizeof(float*));
    is_neuron = (bool**) calloc(slices, sizeof(bool*));

    for (size_t i = 0; i < slices; ++i) {
        grid_values[i] = (float*) calloc(slices, sizeof(float));
        is_neuron[i] = (bool*) calloc(slices, sizeof(bool));
    }
}
 
void scatterNeurons(float neuron_prob) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float random_number = (float) rand() / RAND_MAX;
            if (random_number > neuron_prob) continue;

            is_neuron[i][j] = true;
            da_append(neurons, ((Vector2) {(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
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
    if (grid_values == NULL || is_neuron == NULL) {
        return;
    }

    for (size_t i = 0; i < slices_g; ++i) {
        free(grid_values[i]);
        free(is_neuron[i]);
    }

    free(grid_values);
    free(is_neuron);
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
            if (is_neuron[i][j]) {
                float mean_x = (float) j;
                float mean_y = (float) i;

                Vector2 mean_pair = {.x = mean_x, .y = mean_y};
                da_append(means, mean_pair);
            }
        }
    }

    float max_intensity = FLT_MIN;
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            for (size_t k = 0; k < means.count; ++k) {
                Vector2 mean_pair = means.items[k];
                grid_values[i][j] = 
                        grid_values[i][j] + gaussian2d_1std((float) j, (float) i, mean_pair.x, mean_pair.y);
                max_intensity = fmax(max_intensity, grid_values[i][j]);
            }
        }
    }

     for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            grid_values[i][j] /= max_intensity;
        }
     }
}

bool isMaxima(size_t i, size_t j) {
    bool up_good = 0, down_good = 0, left_good = 0, right_good = 0;
    float cur_value = grid_values[i][j];

    /* up */
    up_good =
        (i == 0) ||
        (grid_values[i - 1][j] < cur_value) ||
        (float_equal(cur_value, grid_values[i - 1][j]));

    /* down */
    down_good =
        (i + 1 >= slices_g) ||
        (grid_values[i + 1][j] < cur_value) ||
        (float_equal(cur_value, grid_values[i + 1][j]));

    /* left */
    left_good =
        (j == 0) ||
        (grid_values[i][j - 1] < cur_value) ||
        (float_equal(cur_value, grid_values[i][j - 1]));

    /* right */
    right_good =
        (j + 1 >= slices_g) ||
        (grid_values[i][j + 1] < cur_value) ||
        (float_equal(cur_value, grid_values[i][j + 1]));

    return up_good && down_good && left_good && right_good;
}

void createNeurons() {
    bool** new_is_neuron = (bool**) calloc(slices_g, sizeof(bool*));
    for (size_t i = 0; i < slices_g; ++i) {
        new_is_neuron[i] = (bool*) calloc(slices_g, sizeof(bool));
    }

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            new_is_neuron[i][j] = isMaxima(i, j) | is_neuron[i][j];
            if (isMaxima(i, j) && !is_neuron[i][j]) {
                da_append(neurons, ((Vector2) {(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            }

        }
    }

    for (size_t i = 0; i < slices_g; ++i) {
        free(is_neuron[i]);
    }
    free(is_neuron);
    is_neuron = new_is_neuron;
}

void colorGrid(bool color_neurons) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            // i corresponds to y axis coordinate
            // j corresponds to x axis coordinate

            float x = j * spacing_g;
            float y = i * spacing_g;
            
            if (is_neuron[i][j] && color_neurons) {
                DrawRectangle(x, y, spacing_g, spacing_g, WHITE);
            }
            else {
                DrawRectangle(x, y, spacing_g, spacing_g, heatmapCmap(grid_values[i][j]));
            }
        }
    }
}

void zeroGridValues() {

}

void connectNeurons() {
     for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            grid_values[i][j] = 0.0f;
        }
     }
}
