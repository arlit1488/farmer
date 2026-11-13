#include "player.h"

struct Player* createPlayer(void) {
    struct Player *p = (struct Player*)malloc(sizeof(struct Player));
    if (!p) return NULL;

    getValidatedString(p->name, MAX_NAME_LEN, "Введите имя фермера: ");
    p->max_hp = 100; p->hp = 100;
    p->max_stamina = 50; p->stamina = 50;
    p->gold = 0;
    p->x = 0; p->y = 0;
    
    // Стартовый инвентарь
    int start_inv[INVENTORY_SIZE] = {1, 2, 1, 3, 2, 1, 4, 2, 3, 0};
    memcpy(p->inventory, start_inv, sizeof(start_inv));

    printf("Персонаж %s успешно создан!\n", p->name);
    return p;
}

void destroyPlayer(struct Player **p) {
    if (*p != NULL) {
        printf("Память структуры по адресу %p освобождена\n", (void*)*p);
        free(*p);
        *p = NULL;
    }
}

void printPlayerCard(struct Player *p, int day, int hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n=== КАРТОЧКА ПЕРСОНАЖА ===\n");
    printf("Имя: %s\n", p->name);
    printf("Время: День %d, %02d:00\n", day, hour);
    printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d\n", p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold);
    showInventory(p, item_names);
}

void showInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("\n=== ИНВЕНТАРЬ ===\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) printf("Слот %d: [%d] - %s\n", i, p->inventory[i], getItemName(p->inventory[i], item_names));
    printf("=================\n");
}

void putItemInSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("Индекс слота (0-%d): ", INVENTORY_SIZE - 1);
    int slot = getValidatedInput();
    if (slot < 0 || slot >= INVENTORY_SIZE) { printf("Ошибка!\n"); return; }
    
    printf("ID предмета (0-%d): ", MAX_ITEM_ID);
    int id = getValidatedInput();
    if (id < 0 || id > MAX_ITEM_ID) { printf("Ошибка!\n"); return; }
    
    p->inventory[slot] = id;
    printf("Предмет \"%s\" помещён в слот %d.\n", getItemName(id, item_names), slot);
}

void throwItemFromSlot(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    printf("Индекс слота (0-%d): ", INVENTORY_SIZE - 1);
    int slot = getValidatedInput();
    if (slot < 0 || slot >= INVENTORY_SIZE) { printf("Ошибка!\n"); return; }
    
    if (p->inventory[slot] != EMPTY_SLOT) {
        printf("Предмет \"%s\" выброшен.\n", getItemName(p->inventory[slot], item_names));
        p->inventory[slot] = EMPTY_SLOT;
    } else printf("Слот уже пуст.\n");
}

void invertInventory(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    for (int i = 0, j = INVENTORY_SIZE - 1; i < j; i++, j--) {
        int temp = p->inventory[i]; p->inventory[i] = p->inventory[j]; p->inventory[j] = temp;
    }
    printf("Инвентарь перевёрнут!\n");
    showInventory(p, item_names);
}

void searchItemByName(struct Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    char name[MAX_NAME_LEN];
    getValidatedString(name, MAX_NAME_LEN, "Введите название предмета: ");
    int id = findItemIdByName(name, item_names);
    if (id < 0) { printf("Предмет \"%s\" не найден в каталоге.\n", name); return; }
    
    printf("ID предмета = %d. Найден в слотах: ", id);
    int found = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (p->inventory[i] == id) { printf("%d ", i); found++; }
    }
    printf(found > 0 ? "\n" : "нет\n");
}