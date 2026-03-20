#ifndef GLOBALS_H
#define GLOBALS_H

#define WINDOW_SIZE 1000
#define GRID_SLICES 500
#define ACCUMULATOR_PROB 0.025f

#include "raylib.h"

extern Vector2 ref_point;
extern Image heatmap_img;
extern Texture2D heatmap_tex;
extern Color *pixels;
extern RenderTexture2D target;


#endif
