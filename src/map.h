#ifndef MAP_H
#define MAP_H

#include "player.h"

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define BASE_GOLD_REWARD 10 // Базовый множитель награды

// Типы клеток (enum)
enum TileType {
    T_FLOOR = '.',
    T_WALL  = '#',
    T_TREE  = 'T',
    T_ROCK  = 'R',
    T_ICE   = '~',
    T_DOWN  = 'D',
    T_HERO  = '@'
};

struct Map {
    char **grid;
    int width;
    int height;
    int depth; // Этаж
};

struct Map* createMap(int depth);
void destroyMap(struct Map **m);
void playAdventure(struct Player *p, int *day, int *hour);

#endif // MAP_H 
