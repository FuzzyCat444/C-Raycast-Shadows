#include "Light.h"

#include <stdlib.h>
#include <math.h>

#include "MathUtil.h"
#include "Raycast.h"

Light* Light_Create(Vector position, Color color) {
    Light* light = (Light*) malloc(sizeof(Light));
    
    light->position = position;
    light->color = color;
    light->distanceMap = NULL;
    light->distanceMapSize = 0;
    
    return light;
}

void Light_Illuminate(Light* light, const Vector* lightRays, 
    int numRays, const Map* map) {
        
    double* distanceMap = light->distanceMap;
    int distanceMapSize = light->distanceMapSize;
    if (distanceMapSize != numRays) {
        distanceMapSize = numRays;
        distanceMap = (double*) realloc(distanceMap, 
            numRays * sizeof(double));
    }
    
    Vector pos = light->position;
    for (int i = 0; i < numRays; i++) {
        distanceMap[i] = raycast(map, pos, lightRays[i]);
    }
    
    light->distanceMap = distanceMap;
    light->distanceMapSize = distanceMapSize;
}

void Light_Destroy(Light* light) {
    if (light == NULL)
        return;
    
    free(light->distanceMap);
    
    free(light);
}

Vector* createLightRays(int numRays) {
    Vector* lightRays = (Vector*) malloc(numRays * sizeof(Vector));
    double thetaIncr = MU_2PI / numRays;
    for (int i = 0; i < numRays; i++) {
        double theta = i * thetaIncr;
        lightRays[i] = (Vector) {
            cos(theta),
            sin(theta)
        };
    }
    return lightRays;
}

void renderLights(Raster* screen, const Light* const lights[], int numLights, 
    const View* view, int blurRadius, int blurStep) {
    
    uint8_t* pixels = screen->pixels;
    int screenWidth = screen->width;
    int screenHeight = screen->height;
    View viewInv = View_Invert(view);
    
    const double angleNorm = 1.0 / MU_2PI;
    int blurDiameter = 1 + 2 * blurRadius;
    int blurRadiusStep = blurRadius * blurStep;
    
    for (int i = 0; i < numLights; i++) {
        const Light* light = lights[i];
        Vector lightPos = light->position;
        Color lightCol = light->color;
        double* distanceMap = light->distanceMap;
        int distanceMapSize = light->distanceMapSize;
        
        Vector pixelPos;
        int idx = 0;
        for (int y = 0; y < screenHeight; y++) {
            pixelPos.y = viewInv.offset.y + viewInv.scale.y * (y + 0.5);
            for (int x = 0; x < screenWidth; x++) {
                pixelPos.x = viewInv.offset.x + viewInv.scale.y * (x + 0.5);
                    
                Vector lightToPixel = {
                    pixelPos.x - lightPos.x,
                    pixelPos.y - lightPos.y
                };
                double lightToPixelDist2 = magnitudeSquared(lightToPixel);
                double lightToPixelDist = sqrt(lightToPixelDist2);
                double angle = fastAtan2(lightToPixel.y, lightToPixel.x);
                int rayIndex = (int) (angleNorm * angle * distanceMapSize);
                if (rayIndex < 0) 
                    rayIndex = 0;
                else if (rayIndex >= distanceMapSize) 
                    rayIndex = distanceMapSize - 1;
                double lightStopDist = distanceMap[rayIndex];
                
                int rayIndex2 = rayIndex - blurRadiusStep;
                while (rayIndex2 < 0) rayIndex2 += distanceMapSize;
                int illumCount = 0;
                for (int j = 0; j < blurDiameter; j++) {
                    if (lightToPixelDist < distanceMap[rayIndex2])
                        illumCount++;
                    rayIndex2 += blurStep;
                    while (rayIndex2 >= distanceMapSize) rayIndex2 -= distanceMapSize;
                }
                double intensityFromBlur = (double) illumCount / blurDiameter;
                
                double intensity = 1.0 / sqrt(1.0 + lightToPixelDist2);
                intensity *= intensityFromBlur;
                
                int idxR = idx + 0;
                int idxG = idx + 1;
                int idxB = idx + 2;
                int r = (int) (pixels[idxR] + intensity * lightCol.r);
                if (r > 255) r = 255;
                int g = (int) (pixels[idxG] + intensity * lightCol.g);
                if (g > 255) g = 255;
                int b = (int) (pixels[idxB] + intensity * lightCol.b);
                if (b > 255) b = 255;
                pixels[idxR] = r;
                pixels[idxG] = g;
                pixels[idxB] = b;
                
                idx += 3;
            }
        }
    }
}