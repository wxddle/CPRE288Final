/*
 * map.c
 *
 *  Created on: Apr 22, 2018
 *      Author: tilden
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <uart.h>
#include "map.h"

//utility deg to rad
  double deg_to_rad(int degrees){
    return (((double) degrees) * PI) / 180;
  }

//set all spaces to clear
  void init_map(map_t *m){
      int i, j;
      for(i=0; i<MAP_Y; i++){
          for (j=0; j< MAP_X; j++) {
              //Set marker for width of robo with '-'
              (i == CENTER_Y && j>=CENTER_X-4 && j<=CENTER_X+4) ?
               (m->map[i][j] = '-') : (m->map[i][j] = ' ');
          }
      }
      //Center Marker
      m->map[CENTER_Y][CENTER_X] = 'X';
      m->map[30][CENTER_X]         = '2';
      m->map[20][CENTER_X]         = '4';
      m->map[10][CENTER_X]         = '6';
      m->map[0][CENTER_X]          = '8';
  }

//populate the cartesian map. Fills from contact to edge of map
  void place_point(map_t *m, terrain t, float distance, int angle){
    int x, y;

    x = (int) (CENTER_X + ((int)(distance / 2) * cos(deg_to_rad(angle))));
    y = (int) (CENTER_Y - ((int)(distance / 2) * sin(deg_to_rad(angle))));

    if (x >= 0 && x < MAP_X && y < MAP_Y && y >= 0) {
        //Double Mark denoted by 'X'
        (m->map[y][x] != ' ') ? (m->map[y][x] = 'X') :
        //IR denoted '#' , PING denoted '*'
        (t == IR) ? (m->map[y][x] = '#') : (m->map[y][x] = '*') ;

    }
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            uart_sendStr(map[y][x]);
        }
    }


  }


