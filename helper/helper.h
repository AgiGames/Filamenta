#ifndef HELPER_H
#define HELPER H

#include "raylib.h"
#include <stddef.h>
#include <stdlib.h>

#define da_append(xs, x) \
    do { \
    if (xs.count >= xs.capacity) { \
        if (xs.capacity == 0) xs.capacity = 256; \
        else xs.capacity *= 2; \
        xs.items = realloc(xs.items, xs.capacity * sizeof(*xs.items)); \
    } \
    xs.items[xs.count++] = x; \
    } while(0)

typedef struct {
    Vector2* items;
    size_t count;
    size_t capacity;
} Vector2DA;

float gaussian2d_1std(float x, float y, float mean_x, float mean_y);

#endif
