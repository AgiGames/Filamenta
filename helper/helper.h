#ifndef HELPER_H
#define HELPER H

#include "raylib.h"
#include <stddef.h>
#include <stdlib.h>

#define DA_APPEND(xs, x) \
    do { \
        if (xs.count >= xs.capacity) { \
            if (xs.capacity == 0) xs.capacity = 256; \
            else xs.capacity *= 2; \
            xs.items = realloc(xs.items, xs.capacity * sizeof(*xs.items)); \
        } \
        xs.items[xs.count++] = x; \
    } while(0)

#define COPY_ARR(dest, source, source_len) \
    do { \
        dest = calloc(source_len, sizeof(*source)); \
        for (size_t copy_arr_i = 0; copy_arr_i < source_len; ++copy_arr_i) { \
            dest[copy_arr_i] = source[copy_arr_i]; \
        } \
    } while(0)

#define FREE_2D_ARR(arr, arr_len) \
    do { \
        for (size_t free_2d_arr_i = 0; free_2d_arr_i < arr_len; ++free_2d_arr_i) { \
            free(arr[free_2d_arr_i]); \
        } \
        free(arr); \
        arr = NULL; \
    } while(0)

#define ZERO_2D_ARR(arr, rows, cols) \
    do { \
        for (size_t zero_2d_arr_i = 0; zero_2d_arr_i < rows; ++zero_2d_arr_i) { \
            for (size_t zero_2d_arr_j = 0; zero_2d_arr_j < cols; ++zero_2d_arr_j) { \
                arr[zero_2d_arr_i][zero_2d_arr_j] = 0; \
            } \
        } \
    } while(0)

typedef struct {
    Vector2* items;
    size_t count;
    size_t capacity;
} Vector2DA;

float gaussian2d_1std(float x, float y, float mean_x, float mean_y);
bool float_equal(float a, float b);
Color heatmap_cmap(float intensity);

#endif
