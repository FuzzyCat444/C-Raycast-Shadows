#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdint.h>

typedef uint8_t Tile;

typedef struct Map {
    int width;
    int height;
    Tile* tiles;
} Map;

Map* Map_Create(int width, int height);

void Map_Clear(Map* map, Tile tile);

void Map_HorizontalLine(Map* map, int x1, int x2, int y, Tile tile);

void Map_VerticalLine(Map* map, int x, int y1, int y2, Tile tile);

void Map_Borders(Map* map, Tile tile);

void Map_Destroy(Map* map);

#endif