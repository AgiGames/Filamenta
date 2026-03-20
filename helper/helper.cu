#include "helper.h"
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "../globals/globals.h"

float gaussian1d(float x, float mean, size_t stddev) {
    float dx = x - mean;
    float exponent = (-0.5f * (dx * dx)) / (float) (stddev * stddev);
    return expf(exponent);
}

float gaussian2d(float x, float y, float mean_x, float mean_y, size_t stddev)
{
    float dx = x - mean_x;
    float dy = y - mean_y;

    float exponent = (-0.5f * (dx*dx + dy*dy)) / (float) (stddev * stddev);
    return expf(exponent);
}

bool float_equal(float a, float b) {
    float diff = fabsf(a - b);
    float largest = fmaxf(fabsf(a), fabsf(b));
    return diff <= largest * 1e-6f;
}

Color heatmap_cmap(float intensity) {
    if (intensity == 1.0f) {
        return WHITE;
    }

    if (intensity < 0.5f) {
        return (Color) {(unsigned char) (255 * (intensity * 2)), 0, 0, 255};
    }
    return (Color) {255, (unsigned char) (255 * ((intensity - 0.5) * 2)), 0, 255};
}

__device__ float euclidean_distance(const Vector2 a, const Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return dx*dx + dy*dy;
}

int points_compar(const void *a, const void *b) {
    const Vector2 *pa = (Vector2*) a;
    const Vector2 *pb = (Vector2*) b;

    float dx_a = pa->x - ref_point.x;
    float dy_a = pa->y - ref_point.y;
    float da = dx_a*dx_a + dy_a*dy_a;

    float dx_b = pb->x - ref_point.x;
    float dy_b = pb->y - ref_point.y;
    float db = dx_b*dx_b + dy_b*dy_b;

    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

void void_swap(void* a, void* b, size_t size) {
    char* cpa = (char*) a;
    char* cpb = (char*) b;
    for (size_t i = 0; i < size; ++i) {
        char temp = cpa[i];
        cpa[i] = cpb[i];
        cpb[i] = temp;
    }
}

size_t partition(void* base, size_t l, size_t r, size_t element_size, int (*cmp)(const void*, const void*)) {
    void* pivot = MEMB_SELECT(base, r, element_size);
    size_t i = l;
    for (size_t j = l; j <= r - 1; ++j) {
        void* j_element = MEMB_SELECT(base, j, element_size);
        if (cmp(j_element, pivot) <= 0) {
            void* i_element = MEMB_SELECT(base, i, element_size);
            void_swap(i_element, j_element, element_size);
            ++i;
        }
    }
    void* i_element = MEMB_SELECT(base, i, element_size);
    void_swap(i_element, pivot, element_size);
    return i;
}

size_t qselect_recur(void* base, size_t l, size_t r, size_t k, 
    size_t num_elements, size_t element_size, int (*cmp)(const void*, const void*)) {
    if (k > 0 && k <= num_elements) {
        int partition_index = partition(base, l, r, element_size, cmp);

        if (partition_index == k - 1) {
            return partition_index;
        }

        if (partition_index > k - 1) {
            return qselect_recur(base, l, partition_index - 1, k, num_elements, element_size, cmp);
        }

        return qselect_recur(base, partition_index + 1, r, k, num_elements, element_size, cmp);
    }

    return INT_MAX;
}

size_t qselect(void* base, size_t k, size_t num_elements, size_t element_size, int (*cmp)(const void*, const void*)) {
    return qselect_recur(base, 0, num_elements - 1, k, num_elements, element_size, cmp);
}


double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}
