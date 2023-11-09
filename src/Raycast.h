#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdint.h>

#include "Vector.h"
#include "Map.h"

double raycast(const Map* map, Vector position, Vector ray);

#endif