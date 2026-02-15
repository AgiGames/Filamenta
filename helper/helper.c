#include <math.h>
#include <stdbool.h>

float gaussian2d_1std(float x, float y, float mean_x, float mean_y)
{
    float dx = x - mean_x;
    float dy = y - mean_y;

    float exponent = -0.05f * (dx*dx + dy*dy);
    return expf(exponent);
}

bool float_equal(float a, float b) {
    float diff = fabsf(a - b);
    float largest = fmaxf(fabsf(a), fabsf(b));
    return diff <= largest * 1e-6f;
}
