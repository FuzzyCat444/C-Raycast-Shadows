#include "Map.h"

Map* Map_Create(int width, int height) {
    Map* map = (Map*) malloc(sizeof(Map));
    
    map->width = width;
    map->height = height;
    map->tiles = (Tile*) malloc(width * height * sizeof(Tile));
    
    return map;
}

void Map_Clear(Map* map, Tile tile) {
    Tile* tiles = map->tiles;
    int size = map->width * map->height;
    for (int i = 0; i < size; i++) {
        tiles[i] = tile;
    }        
}

void Map_HorizontalLine(Map* map, int x1, int x2, int y, Tile tile) {
    if (x1 < 0) x1 = 0;
    if (x2 >= map->width) x2 = map->width - 1;
    
    Tile* tiles = map->tiles;
    int idx = x1 + y * map->width;
    for (int x = x1; x <= x2; x++) {
        tiles[idx] = tile;
        idx++;
    }
}

void Map_VerticalLine(Map* map, int x, int y1, int y2, Tile tile) {
    if (y1 < 0) y1 = 0;
    if (y2 >= map->height) y2 = map->height - 1;
    
    Tile* tiles = map->tiles;
    int width = map->width;
    int idx = x + y1 * width;
    for (int y = y1; y <= y2; y++) {
        tiles[idx] = tile;
        idx += width;
    }
}

void Map_Borders(Map* map, Tile tile) {
    int width = map->width;
    int height = map->height;
    Map_HorizontalLine(map, 0, width - 1, 0, tile);
    Map_HorizontalLine(map, 0, width - 1, height - 1, tile);
    Map_VerticalLine(map, 0, 0, height - 1, tile);
    Map_VerticalLine(map, width - 1, 0, height - 1, tile);
}

void Map_Destroy(Map* map) {
    if (map == NULL)
        return;
    
    free(map->tiles);
    
    free(map);
}