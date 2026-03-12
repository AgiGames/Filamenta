#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#include "../helper/helper.h"
#include "raylib.h"
#include "grid.h"
#include "../globals/globals.h"
#include "rlgl.h"

size_t window_size_g = 750;
size_t slices_g = 50;
float spacing_g = 15.0f;
size_t num_accumulators = 0;
size_t stddev = 4;

float *grid_values = NULL;
bool *is_accumulator = NULL;
Vector2DA accumulators = {0};
PointPairDA connections = {0};
Vector2 ref_point = {0};
bool changed = false;

Image heatmap_img;
Texture2D heatmap_tex;
RenderTexture2D target;

void init_grid(size_t window_size, size_t slices) {
    num_accumulators = 0;
    window_size_g = window_size;
    slices_g = slices;
    spacing_g = (float) window_size / slices;
    grid_values = (float*) calloc(slices * slices, sizeof(float));
    is_accumulator = (bool*) calloc(slices * slices, sizeof(bool));
    accumulators.count = 0;
    connections.count = 0;
    changed = false;
}

void scatter_accumulators(float accumulator_prob) {
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float random_number = (float)rand() / RAND_MAX;
            if (random_number > accumulator_prob) continue;

            GRID_ACCESS(is_accumulator, i, j) = true;
            DA_APPEND(accumulators, ((Vector2){(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            num_accumulators += 1;
        }
    }
}

void free_grid() {
    if (grid_values != NULL) {
        free(grid_values);
        grid_values = NULL;
    }
    if (is_accumulator != NULL) {
        free(is_accumulator);
        is_accumulator = NULL;
    }
}

void expunge_gaussian() {
    float* grid_values_copy = (float*) calloc(slices_g * slices_g, sizeof(float));
    COPY_ARR(grid_values_copy, grid_values, slices_g * slices_g);
    
    float* temp_array = (float*) calloc(slices_g * slices_g, sizeof(float));
    size_t halflen = 3 * stddev;
    float max_intensity = FLT_MIN;

    // first pass
    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            size_t l = FIND_MAX(0, (int) j - (int) halflen);
            size_t r = FIND_MIN(slices_g - 1, j + halflen);
            for (size_t k = l; k <= r; ++k) {
                GRID_ACCESS(temp_array, i, j) +=
                    GRID_ACCESS(is_accumulator, i, k) * gaussian1d(
                                                            (float) k,
                                                            (float) j,
                                                            stddev
                                                        );
            }
        }
    }

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            size_t l = FIND_MAX(0, (int) i - (int) halflen);
            size_t r = FIND_MIN(slices_g - 1, i + halflen);
            for (size_t k = l; k <= r; ++k) {
                GRID_ACCESS(grid_values, i, j) +=
                    GRID_ACCESS(temp_array, k, j) * gaussian1d(
                                                            (float) k,
                                                            (float) i,
                                                            stddev
                                                        );
            }
            max_intensity = fmax(max_intensity, GRID_ACCESS(grid_values, i, j));
        }
    }

    free(temp_array);

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            GRID_ACCESS(grid_values, i, j) /= max_intensity;
            if (!float_equal(GRID_ACCESS(grid_values, i, j), GRID_ACCESS(grid_values_copy, i, j))) {
                changed = true;
            }
        }
    }
    
    free(grid_values_copy);
}

bool is_maxima(size_t i, size_t j) {
    bool up_good = 0, down_good = 0, left_good = 0, right_good = 0;
    float cur_value = GRID_ACCESS(grid_values, i, j);
    if (float_equal(cur_value, 0.0f)) return false;

    up_good =
        (i == 0) ||
        (GRID_ACCESS(grid_values, i - 1, j) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i - 1, j)));

    down_good =
        (i + 1 >= slices_g) ||
        (GRID_ACCESS(grid_values, i + 1, j) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i + 1, j)));

    left_good =
        (j == 0) ||
        (GRID_ACCESS(grid_values, i, j - 1) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i, j - 1)));

    right_good =
        (j + 1 >= slices_g) ||
        (GRID_ACCESS(grid_values, i, j + 1) < cur_value) ||
        (float_equal(cur_value, GRID_ACCESS(grid_values, i, j + 1)));

    return up_good && down_good && left_good && right_good;
}

void create_accumulators() {
    bool *new_is_accumulator = (bool *)calloc(slices_g * slices_g, sizeof(bool));

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            GRID_ACCESS(new_is_accumulator, i, j) =
                is_maxima(i, j) || GRID_ACCESS(is_accumulator, i, j);

            if (is_maxima(i, j) && !GRID_ACCESS(is_accumulator, i, j)) {
                num_accumulators++;
                changed = true;
                DA_APPEND(accumulators, ((Vector2){(j * spacing_g) + (spacing_g / 2.0f), (i * spacing_g) + (spacing_g / 2.0f)}));
            }
        }
    }

    free(is_accumulator);
    is_accumulator = new_is_accumulator;
}

void color_grid(bool color_accumulators) {
    Color *pixels = (Color *)heatmap_img.data;

    for (size_t i = 0; i < slices_g; ++i) {
        for (size_t j = 0; j < slices_g; ++j) {
            float v = GRID_ACCESS(grid_values, i, j);
            GRID_ACCESS(pixels, i, j) = heatmap_cmap(v);
            if (GRID_ACCESS(is_accumulator, i, j) && color_accumulators) {
                GRID_ACCESS(pixels, i, j) = WHITE;
            }
        }
    }

    UpdateTexture(heatmap_tex, pixels);

    // draw the texture scaled to grid size
    DrawTexturePro(
            heatmap_tex,
            (Rectangle) {0, 0, slices_g, slices_g},
            (Rectangle) {0, 0, slices_g * spacing_g, slices_g * spacing_g},
            (Vector2) {0 ,0},
            0,
            WHITE
    );
}

void draw_accumulator_connections() {
    rlBegin(RL_LINES);
    rlColor4ub(255, 255, 255, 255); // ~20% opacity

    for (size_t i = 0; i < connections.count; ++i) {
        PointPair pp = connections.items[i];

        rlVertex2f(pp.p1.x, pp.p1.y);
        rlVertex2f(pp.p2.x, pp.p2.y);
    }

    rlEnd();
}

void connect_accumulators() {
    double start = now();

    Vector2 *accumulators_coord_copy = NULL;
    COPY_ARR(accumulators_coord_copy, accumulators.items, accumulators.count);
    connections.count = 0;

    for (size_t i = 0; i < accumulators.count; ++i) {
        ref_point = accumulators.items[i];

        size_t row = (size_t)((ref_point.y - (spacing_g / 2.0f)) / spacing_g);
        size_t col = (size_t)((ref_point.x - (spacing_g / 2.0f)) / spacing_g);
        float point_intensity = GRID_ACCESS(grid_values, row, col);

        size_t num_connections = floor(point_intensity * accumulators.count);
        if (num_connections < 2) num_connections = 2;
        qsort(accumulators_coord_copy, accumulators.count, sizeof(Vector2), points_compar);

        for (size_t j = 0; j < num_connections; ++j) {
            if (j + 1 < accumulators.count) {
                Vector2 j_closest_point = accumulators_coord_copy[j + 1];

                size_t j_row = (size_t)((j_closest_point.y - (spacing_g / 2.0f)) / spacing_g);
                size_t j_col = (size_t)((j_closest_point.x - (spacing_g / 2.0f)) / spacing_g);

                if (gaussian2d(j_col, j_row, col, row, stddev) < 0.1f) continue;
                
                DA_APPEND(connections, ((PointPair) {ref_point, accumulators_coord_copy[j + 1]}));
            }
        }
    }

    free(accumulators_coord_copy);

    double end = now();
    printf("connect_accumulators: %.6f seconds\n", end - start);
}

void do_n_iterations() {
    while (true) {
        changed = false;
        expunge_gaussian();
        create_accumulators();
        connect_accumulators();
        if (changed == false) break;
    }
}

size_t get_num_accumulators() {
    return num_accumulators;
}
