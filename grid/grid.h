#ifndef GRID_H
#define GRID_H

#include "raylib.h"
#include <stddef.h>

void initGrid(size_t window_size, size_t slices);
void freeGrid();
void drawGrid(Color color);
void colorGrid(bool color_neurons);
void scatterNeurons(float neuron_prob);
void expungeGaussian();
void createNeurons();
void zeroGridValues();

#endif
