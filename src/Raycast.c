#include "Raycast.h"

double raycast(const Map* map, Vector position, Vector ray) {
    Tile* tiles = map->tiles;
    int width = map->width;
    int height = map->height;
    
    int signDx = ray.x < 0.0 ? -1 : 1;
    double dydx = ray.y / ray.x * signDx;
    double dtdx = 1.0 / ray.x * signDx;
    int signDy = ray.y < 0.0 ? -1 : 1;
    double dxdy = ray.x / ray.y * signDy;
    double dtdy = 1.0 / ray.y * signDy;
    
    int tileX = 
        position.x < 0.0 ? 
        (int) position.x - 1 :
        (int) position.x;
    int tileY = 
        position.y < 0.0 ?
        (int) position.y - 1 :
        (int) position.y;
    Tile tile = 0;
    double t = 0.0;
    double xy = 0.0;
    int hitAxis = -1;
    double t1 = 
        signDx == -1 ? 
        (position.x - tileX) * dtdx :
        (tileX + 1 - position.x) * dtdx;
    double y = position.y + ray.y * t1;
    double t2 =
        signDy == -1 ?
        (position.y - tileY) * dtdy :
        (tileY + 1 - position.y) * dtdy;
    double x = position.x + ray.x * t2;
    
    while (
        tileX >= 0 && tileX < width &&
        tileY >= 0 && tileY < height &&
        (tile = tiles[tileX + tileY * width]) == 0
    ) {
        if (t1 < t2) {
            t = t1;
            xy = y;
            t1 += dtdx;
            y += dydx;
            tileX += signDx;
            hitAxis = 0;
        } else {
            t = t2;
            xy = x;
            t2 += dtdy;
            x += dxdy;
            tileY += signDy;
            hitAxis = 1;
        }
    }
    
    return t;
}