#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "../helper/helper.h"
#include "raymath.h"
#include "raylib.h"
#include "grid.h"

size_t window_size_g = 750;
size_t slices_g = 50;
float spacing_g = 15.0f;
size_t accumulators_created = 0;

float** grid_values = NULL;
bool** is_accumulator = NULL;
Vector2DA accumulators = {0};
Vector2 ref_point = {0};

void initGrid(size_t window_size, size_t slices) {
    accumulators_created = 0;
    window_size_g = window_size;
    slices_g = slices;
    spacing_g = (float) window_size / slices;
    grid_values = (float**) calloc(slices, sizeof(float*));
    is_accumulator = (bool**) calloc(slices, sizeof(bool*));
    accumulators.count = 0;

    for (size_t i = 0; i < slices; ++i) {
        grid_values[i] = (float*) calloc(slices, sizeof(float));
        is_accumulator[i] = (bool*) calloc(slices, sizeof(bool));
    }
}
 
void scatterAccumulators(float accumulator_prob) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float random_number = (float) rand() / RAND_MAX;
            if (random_number > accumulator_prob) continue;

            is_accumulator[i][j] = true;
            DA_APPEND(accumulators, ((Vector2) {(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            accumulators_created += 1;
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
    if (grid_values != NULL) {
        FREE_2D_ARR(grid_values, slices_g);
    }

    if (is_accumulator != NULL) {
        FREE_2D_ARR(is_accumulator, slices_g);
    }
}

void expungeGaussian() {
    Vector2DA means = {0};
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            if (is_accumulator[i][j]) {
                float mean_x = (float) j;
                float mean_y = (float) i;

                Vector2 mean_pair = {.x = mean_x, .y = mean_y};
                DA_APPEND(means, mean_pair);
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

    up_good =
        (i == 0) ||
        (grid_values[i - 1][j] < cur_value) ||
        (float_equal(cur_value, grid_values[i - 1][j]));

    down_good =
        (i + 1 >= slices_g) ||
        (grid_values[i + 1][j] < cur_value) ||
        (float_equal(cur_value, grid_values[i + 1][j]));

    left_good =
        (j == 0) ||
        (grid_values[i][j - 1] < cur_value) ||
        (float_equal(cur_value, grid_values[i][j - 1]));

    right_good =
        (j + 1 >= slices_g) ||
        (grid_values[i][j + 1] < cur_value) ||
        (float_equal(cur_value, grid_values[i][j + 1]));

    return up_good && down_good && left_good && right_good;
}

void createAccumulators() {
    bool** new_is_accumulator = (bool**) calloc(slices_g, sizeof(bool*));
    for (size_t i = 0; i < slices_g; ++i) {
        new_is_accumulator[i] = (bool*) calloc(slices_g, sizeof(bool));
    }

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            new_is_accumulator[i][j] = isMaxima(i, j) | is_accumulator[i][j];
            if (isMaxima(i, j) && !is_accumulator[i][j]) {
                accumulators_created++;
                DA_APPEND(accumulators, ((Vector2) {(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            }

        }
    }

    FREE_2D_ARR(is_accumulator, slices_g);
    is_accumulator = new_is_accumulator;
}

void colorGrid(bool color_accumulators) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            // i corresponds to y axis coordinate
            // j corresponds to x axis coordinate

            float x = j * spacing_g;
            float y = i * spacing_g;
            
            if (is_accumulator[i][j] && color_accumulators) {
                DrawRectangle(x, y, spacing_g, spacing_g, heatmapCmap(grid_values[i][j]));
                DrawCircle(x + (spacing_g / 2.0f), y + (spacing_g / 2.0f), (spacing_g / 2.0), WHITE);
            }
            else {
                DrawRectangle(x, y, spacing_g, spacing_g, heatmapCmap(grid_values[i][j]));
            }
        }
    }
}

int pointsCompar(const void *a, const void *b) {
    const Vector2 *pa = a;
    const Vector2 *pb = b;

    float da = Vector2Distance(ref_point, *pa);
    float db = Vector2Distance(ref_point, *pb);

    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

void connectAccumulators() {
    Vector2* accumulators_coord_copy = NULL;
    COPY_ARR(accumulators_coord_copy, accumulators.items, accumulators.count);
    for (size_t i = 0; i < accumulators.count; ++i) {
        ref_point = accumulators.items[i];
        qsort(accumulators_coord_copy, accumulators.count, sizeof(Vector2), pointsCompar);
        size_t row = (size_t) ((ref_point.y - (spacing_g / 2.0f)) / spacing_g);
        size_t col = (size_t) ((ref_point.x - (spacing_g / 2.0)) / spacing_g);
        float point_intensity = grid_values[row][col];

        size_t num_connections = floor(point_intensity * accumulators.count);
        if (num_connections < 2) num_connections = 2;

        for (size_t j = 0; j < num_connections; ++j) {
            if (j + 1 < accumulators.count) {
                Vector2 j_closest_point = accumulators_coord_copy[j + 1];
                size_t j_row = (size_t) ((j_closest_point.y - (spacing_g / 2.0f)) / spacing_g);
                size_t j_col = (size_t) ((j_closest_point.x - (spacing_g / 2.0f)) / spacing_g);
                
                if (gaussian2d_1std(j_col, j_row, col, row) < 0.1) {
                    continue;
                }

                DrawLineV(ref_point, accumulators_coord_copy[j + 1], WHITE);
            }
        }
    }

    free(accumulators_coord_copy);
}

size_t numNewAccumulators() {
    return accumulators_created;
}
