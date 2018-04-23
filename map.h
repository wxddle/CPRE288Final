/*
 * map.h
 *
 *  Created on: Apr 22, 2018
 *      Author: tilden
 */

#ifndef MAP_H
#define MAP_H
#define PI 3.14159265359

#include <stdint.h>

//Object detection constants
#define MAX_RANGE 90

//MAP CONSTANTS
#define MAP_X 80
#define MAP_Y 40
#define CENTER_X 39
#define CENTER_Y 39

typedef enum {IR, P} terrain;

typedef struct map {
  char map[MAP_Y][MAP_X];
} map_t;


void init_map(map_t *m);

void place_point(map_t *m, terrain t, float distance, int angle);

#endif
