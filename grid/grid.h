#ifndef GRID_H
#define GRID_H

#include "raylib.h"
#include <stddef.h>

void initGrid(size_t window_size, size_t slices);
void freeGrid();
void colorGrid(bool color_accumulators);
void scatterAccumulators(float accumulator_prob);
void expungeGaussian();
void createAccumulators();
void zeroGridValues();
size_t numNewAccumulators();
void connectAccumulators();

#endif
