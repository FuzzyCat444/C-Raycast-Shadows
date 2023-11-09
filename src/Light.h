#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"
#include "Map.h"
#include "Raster.h"
#include "View.h"

typedef struct Light {
    Vector position;
    Color color;
    double* distanceMap;
    int distanceMapSize;
} Light;

Light* Light_Create(Vector position, Color color);

void Light_Illuminate(Light* light, const Vector* lightRays, 
    int numRays, const Map* map);

void Light_Destroy(Light* light);

Vector* createLightRays(int numRays);

void renderLights(Raster* screen, const Light* const lights[], int numLights, 
    const View* view, int blurRadius, int blurStep);

#endif