#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVENTORY_SIZE  10
#define MAX_ITEMS       10
#define MAX_NAME_LEN    32
#define MAX_LINE_LEN    512
#define MAX_OUTPUT_LINE_LEN 1024

// Имена файлов
#define ITEMS_FILE   "items.txt"
#define DIARY_FILE   "diary.txt"
#define INPUT_FILE   "input.txt"
#define OUTPUT_FILE  "output.txt"

// ID предметов
#define EMPTY_SLOT  0
#define MAX_ITEM_ID 9

void clearScreen(void);
int getValidatedInput(void);
void getValidatedString(char* buffer, int size, const char* prompt);

void loadItemNames(char item_names[MAX_ITEMS][MAX_NAME_LEN]);
const char* getItemName(int id, char item_names[MAX_ITEMS][MAX_NAME_LEN]);
int findItemIdByName(const char* name, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

void writeDiary(const char* name, int day, int time, int inventory[INVENTORY_SIZE], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void processNpcTemplate(const char* name, int day, int inventory[INVENTORY_SIZE], char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif // UTILS_H