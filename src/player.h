#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

struct Player {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x, y; // Координаты для карты
    int inventory[INVENTORY_SIZE];
};

struct Player* createPlayer(void);
void destroyPlayer(struct Player **p);
void printPlayerCard(struct Player *p, int day, int hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void showInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItemInSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void throwItemFromSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void invertInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void searchItemByName(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif // PLAYER_H