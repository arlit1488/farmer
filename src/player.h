#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

enum LightMode {
    LIGHT_OFF = 0,
    LIGHT_NORMAL = 1,
    LIGHT_BRIGHT = 2
};

struct Lantern {
    int oil;
    int max_oil;
    enum LightMode mode;
};

struct Player {
    char name[MAX_NAME_LEN];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x, y; 
    int inventory[INVENTORY_SIZE];
    struct Lantern lantern; // Вложенная структура
};

struct Player* createPlayer(void);
void destroyPlayer(struct Player **p);
void printPlayerCard(struct Player *p, int day, int hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void showInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItemInSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void throwItemFromSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void invertInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void searchItemByName(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

void buyLanternOil(struct Player *p);

#endif // PLAYER_H
