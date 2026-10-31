#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// =================== КОНСТАНТЫ =======================
#define INVENTORY_SIZE  10
#define MAX_ITEMS       10
#define MAX_NAME_LEN    32
#define HOURS_PER_DAY   24
#define MAX_LINE_LEN    512
#define MAX_OUTPUT_LINE_LEN 1024

// --- Константы для ЛР4 (Подземелье) ---
#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30

// Символы карты
#define TILE_FLOOR   '.'
#define TILE_WALL    '#'
#define TILE_TREE    'T'
#define TILE_ROCK    'R'
#define TILE_HERO    '@'
#define TILE_DOWN    'D'
#define TILE_ICE     '~'   // Скользкий лед

// ID предметов
#define EMPTY_SLOT  0
#define WOOD        1
#define STONE       2
#define SEEDS       3
#define IRON        4
#define GOLD        5
#define WOOL        6
#define MILK        7
#define BREAD       8
#define FISH        9
#define MAX_ITEM_ID FISH

// Имена файлов
#define ITEMS_FILE   "items.txt"
#define DIARY_FILE   "diary.txt"
#define INPUT_FILE   "input.txt"
#define OUTPUT_FILE  "output.txt"

// Теги шаблонизатора NPC
#define TAG_NAME     "<NAME>"
#define TAG_DAY      "<DAY>"
#define TAG_ITEM     "<ITEM>"

// ===================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ====================
char farmer_name[MAX_NAME_LEN] = "Фермер";
int  current_day  = 1;
int  current_time = 8;
int  inventory[INVENTORY_SIZE] = {WOOD, STONE, WOOD, SEEDS, STONE, WOOD, IRON, STONE, SEEDS, EMPTY_SLOT};
char item_names[MAX_ITEMS][MAX_NAME_LEN];

// --- Переменные для ЛР4 (Подземелье) ---
char **dungeon_map = NULL;
int map_height = 0;
int map_width = 0;
int hero_y = 0;
int hero_x = 0;
int dungeon_depth = 1;

// ==================== СЛУЖЕБНЫЕ ФУНКЦИИ =====================
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int getValidatedInput(void) {
    int value;
    while (scanf("%d", &value) != 1) {
        while (getchar() != '\n');
        printf("Ошибка ввода! Пожалуйста, введите число: ");
    }
    while (getchar() != '\n');
    return value;
}

void getValidatedString(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

// ==================== КАТАЛОГ ПРЕДМЕТОВ =======================
void setDefaultItemNames(void) {
    strcpy(item_names[EMPTY_SLOT], "Пусто");
    strcpy(item_names[WOOD],       "Дерево");
    strcpy(item_names[STONE],      "Камень");
    strcpy(item_names[SEEDS],      "Семена");
    strcpy(item_names[IRON],       "Железо");
    strcpy(item_names[GOLD],       "Золото");
    strcpy(item_names[WOOL],       "Шерсть");
    strcpy(item_names[MILK],       "Молоко");
    strcpy(item_names[BREAD],      "Хлеб");
    strcpy(item_names[FISH],       "Рыба");
}

void loadItemNames(void) {
    FILE* file = fopen(ITEMS_FILE, "r");
    if (file == NULL) {
        printf("Предупреждение: файл %s не найден!\n", ITEMS_FILE);
        printf("Загружены названия по умолчанию.\n");
        setDefaultItemNames();
        return;
    }
    for (int i = 0; i < MAX_ITEMS; i++) item_names[i][0] = '\0';
    char line[MAX_LINE_LEN];
    int loaded = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        int id;
        char name[MAX_NAME_LEN];
        if (sscanf(line, "%d %31s", &id, name) == 2) {
            if (id >= 0 && id < MAX_ITEMS) {
                strncpy(item_names[id], name, MAX_NAME_LEN - 1);
                item_names[id][MAX_NAME_LEN - 1] = '\0';
                loaded++;
            }
        }
    }
    fclose(file);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (item_names[i][0] == '\0') {
            snprintf(item_names[i], MAX_NAME_LEN, "Предмет_%d", i);
        }
    }
}

const char* getItemName(int id) {
    if (id < 0 || id >= MAX_ITEMS) return "???";
    return item_names[id];
}

int findItemIdByName(const char* name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], name) == 0) return i;
    }
    return -1;
}

// ======================= ИНВЕНТАРЬ =====================
void showInventory(void) {
    printf("\n=== ИНВЕНТАРЬ ===\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: [%d] - %s\n",i, inventory[i], getItemName(inventory[i]));
    }
    printf("=================\n");
}

void putItemInSlot(void) {
    printf("Введите индекс слота (0-%d): ", INVENTORY_SIZE - 1);
    int slot = getValidatedInput();
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: индекс слота должен быть от 0 до %d!\n", INVENTORY_SIZE - 1);
        return;
    }
    printf("Введите ID предмета (0-%d): ", MAX_ITEM_ID);
    int item_id = getValidatedInput();
    if (item_id < 0 || item_id > MAX_ITEM_ID) {
        printf("Ошибка: ID предмета должен быть от 0 до %d!\n", MAX_ITEM_ID);
        return;
    }
    inventory[slot] = item_id;
    printf("Предмет \"%s\" (ID: %d) помещён в слот %d.\n", getItemName(item_id), item_id, slot);
}

void throwItemFromSlot(void) {
    printf("Введите индекс слота для выбрасывания (0-%d): ", INVENTORY_SIZE - 1);
    int slot = getValidatedInput();
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: индекс слота должен быть от 0 до %d!\n", INVENTORY_SIZE - 1);
        return;
    }
    if (inventory[slot] != EMPTY_SLOT) {
        printf("Предмет \"%s\" (ID: %d) выброшен из слота %d.\n", getItemName(inventory[slot]), inventory[slot], slot);
        inventory[slot] = EMPTY_SLOT;
    } else {
        printf("В этом слоте и так пусто!\n");
    }
}

void invertInventory(void) {
    printf("\nИНВЕРСИЯ КАРМАНОВ\n\nИнвентарь ДО\n");
    showInventory();
    for (int i = 0, j = INVENTORY_SIZE - 1; i < j; i++, j--) {
        int temp = inventory[i];
        inventory[i] = inventory[j];
        inventory[j] = temp;
    }
    printf("\nИнвентарь ПОСЛЕ\n");
    showInventory();
    printf("Инвентарь перевёрнут задом наперёд!\n");
}

void searchItemByName(void) {
    char name[MAX_NAME_LEN];
    getValidatedString(name, MAX_NAME_LEN, "Введите название предмета для поиска: ");
    int id = findItemIdByName(name);
    if (id < 0) {
        printf("Предмет \"%s\" не найден в каталоге.\n", name);
        return;
    }
    printf("Предмет \"%s\" имеет ID = %d.\n", name, id);
    int found = 0;
    printf("Найден в слотах: ");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == id) { printf("%d ", i); found++; }
    }
    if (found == 0) printf("нет (предмет отсутствует в рюкзаке)\n");
    else printf("\nВсего найдено: %d шт.\n", found);
}

void writeDiary(void) {
    FILE* file = fopen(DIARY_FILE, "a");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть %s для записи!\n", DIARY_FILE);
        return;
    }
    fprintf(file, "\n=== Запись дневника ===\nФермер: %s\nВремя:  День %d, %02d:00\nИнвентарь:\n", farmer_name, current_day, current_time);
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        fprintf(file, "  Слот %d: [%d] - %s\n", i, inventory[i], getItemName(inventory[i]));
    }
    fprintf(file, "======================\n");
    fclose(file);
    printf("Состояние записано в файл %s\n", DIARY_FILE);
}

void appendToBuffer(char* dest, size_t destSize, size_t* pos, const char* src) {
    while (*src != '\0' && *pos < destSize - 1) {
        dest[*pos] = *src; (*pos)++; src++;
    }
    dest[*pos] = '\0';
}

void replaceNpcTags(const char* src, char* dest, size_t destSize) {
    size_t pos = 0;
    dest[0] = '\0';
    for (size_t i = 0; src[i] != '\0'; ) {
        if (strncmp(&src[i], TAG_NAME, strlen(TAG_NAME)) == 0) {
            appendToBuffer(dest, destSize, &pos, farmer_name); i += strlen(TAG_NAME);
        } else if (strncmp(&src[i], TAG_DAY, strlen(TAG_DAY)) == 0) {
            char dayBuf[16]; snprintf(dayBuf, sizeof(dayBuf), "%d", current_day);
            appendToBuffer(dest, destSize, &pos, dayBuf); i += strlen(TAG_DAY);
        } else if (strncmp(&src[i], TAG_ITEM, strlen(TAG_ITEM)) == 0) {
            appendToBuffer(dest, destSize, &pos, getItemName(inventory[0])); i += strlen(TAG_ITEM);
        } else {
            if (pos < destSize - 1) { dest[pos] = src[i]; pos++; dest[pos] = '\0'; }
            i++;
        }
    }
}

void processNpcTemplate(void) {
    FILE* in = fopen(INPUT_FILE, "r");
    if (in == NULL) {
        printf("Ошибка: не удалось открыть %s!\n", INPUT_FILE); return;
    }
    FILE* out = fopen(OUTPUT_FILE, "w");
    if (out == NULL) {
        printf("Ошибка: не удалось открыть %s!\n", OUTPUT_FILE); fclose(in); return;
    }
    char line[MAX_LINE_LEN]; char result[MAX_OUTPUT_LINE_LEN];
    printf("\n=== Персонализированное письмо NPC ===\n");
    while (fgets(line, sizeof(line), in) != NULL) {
        replaceNpcTags(line, result, sizeof(result));
        printf("%s", result); fputs(result, out);
    }
    printf("\n=== Конец письма ===\n");
    fclose(in); fclose(out);
    printf("Результат сохранён в файл %s\n", OUTPUT_FILE);
}

// ==================== ЛР4: ПОДЗЕМЕЛЬЕ (СКОЛЬЗКИЙ ЛЕД) ====================

void freeDungeonMap(void) {
    if (dungeon_map != NULL) {
        for (int i = 0; i < map_height; i++) free(dungeon_map[i]);
        free(dungeon_map);
        dungeon_map = NULL;
    }
    map_height = 0;
    map_width = 0;
}

void generateDungeonFloor(void) {
    freeDungeonMap();

    map_height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    map_width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);

    dungeon_map = (char**)malloc(map_height * sizeof(char*));
    for (int i = 0; i < map_height; i++) {
        dungeon_map[i] = (char*)malloc(map_width * sizeof(char));
    }

    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            if (y == 0 || y == map_height - 1 || x == 0 || x == map_width - 1) {
                dungeon_map[y][x] = TILE_WALL;
            } else {
                int chance = rand() % 100;
                if (chance < 15) dungeon_map[y][x] = TILE_TREE;
                else if (chance < 30) dungeon_map[y][x] = TILE_ROCK;
                else if (chance < 45) dungeon_map[y][x] = TILE_ICE; // 15% шанс генерации льда
                else dungeon_map[y][x] = TILE_FLOOR;
            }
        }
    }

    // Ставим героя на свободный пол (не на лед, не на препятствие)
    do {
        hero_y = 1 + rand() % (map_height - 2);
        hero_x = 1 + rand() % (map_width - 2);
    } while (dungeon_map[hero_y][hero_x] != TILE_FLOOR);

    // Ставим лестницу вниз
    int dx, dy;
    do {
        dy = 1 + rand() % (map_height - 2);
        dx = 1 + rand() % (map_width - 2);
    } while ((dy == hero_y && dx == hero_x) || dungeon_map[dy][dx] != TILE_FLOOR);
    
    dungeon_map[dy][dx] = TILE_DOWN;
}

void drawDungeonMap(void) {
    printf("\n--- Этаж %d ---\n", dungeon_depth);
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            if (y == hero_y && x == hero_x) printf("%c", TILE_HERO);
            else printf("%c", dungeon_map[y][x]);
        }
        printf("\n");
    }
    printf("Управление: W (вверх), A (влево), S (вниз), D (вправо), Q (выйти домой)\n");
    printf("Символы: # - стена, T - дерево, R - камень, ~ - скользкий лед, D - спуск\n");
}

void playDungeon(void) {
    generateDungeonFloor();
    dungeon_depth = 1;

    while (1) {
        clearScreen();
        drawDungeonMap();

        printf("Ваш ход: ");
        char move;
        scanf(" %c", &move);
        while (getchar() != '\n');

        if (move == 'Q' || move == 'q') {
            printf("Вы сбежали из подземелья!\n");
            freeDungeonMap();
            return;
        }

        int old_y = hero_y;
        int old_x = hero_x;
        int new_y = hero_y;
        int new_x = hero_x;

        if (move == 'W' || move == 'w') new_y--;
        else if (move == 'S' || move == 's') new_y++;
        else if (move == 'A' || move == 'a') new_x--;
        else if (move == 'D' || move == 'd') new_x++;
        else continue;

        if (new_y < 0 || new_y >= map_height || new_x < 0 || new_x >= map_width) continue;

        char target = dungeon_map[new_y][new_x];
        if (target == TILE_WALL || target == TILE_TREE || target == TILE_ROCK) continue;

        // Делаем первый шаг
        hero_y = new_y;
        hero_x = new_x;

        // --- МЕХАНИКА СКОЛЬЗКОГО ЛЬДА ---
        if (target == TILE_ICE) {
            printf("\nВы наступили на скользкий лед и покатились!\n");
            int dy = new_y - old_y;
            int dx = new_x - old_x;
            bool stopped = false;

            while (!stopped) {
                int next_y = hero_y + dy;
                int next_x = hero_x + dx;

                // Проверка границ (на всякий случай)
                if (next_y < 0 || next_y >= map_height || next_x < 0 || next_x >= map_width) {
                    break;
                }

                char next_tile = dungeon_map[next_y][next_x];
                
                if (next_tile == TILE_WALL || next_tile == TILE_TREE || next_tile == TILE_ROCK) {
                    printf("Вы врезались в препятствие и остановились.\n");
                    stopped = true; // Останавливаемся, не заходя на клетку с препятствием
                } else if (next_tile == TILE_ICE) {
                    // Скользим дальше
                    hero_y = next_y;
                    hero_x = next_x;
                } else {
                    // Съехали на обычный пол или лестницу — движение прекращается
                    hero_y = next_y;
                    hero_x = next_x;
                    printf("Вы съехали на обычный пол и остановились.\n");
                    stopped = true;
                }
            }
            printf("Нажмите Enter для продолжения...");
            getchar();
        }

        // Проверка на лестницу (после всех перемещений, включая скольжение)
        if (dungeon_map[hero_y][hero_x] == TILE_DOWN) {
            dungeon_depth++;
            printf("\nВы спускаетесь глубже... Этаж %d\n", dungeon_depth);
            printf("Нажмите Enter...");
            getchar();
            generateDungeonFloor();
        }
    }
}


// ====================== MAIN ====================
int main(void)
{
    srand(time(NULL));

    int action = -1;
    int time;

    clearScreen();
    printf("=== Добро пожаловать в игру «Весёлый фермер» ===\n\n");
    getValidatedString(farmer_name, MAX_NAME_LEN,"Как вас зовут, фермер? ");

    loadItemNames();

    printf("\nНажмите Enter, чтобы продолжить...");
    getchar();

    while (action != 0)
    {
        if (current_time >= 24) {
            current_day = current_day + current_time / HOURS_PER_DAY;
            current_time = current_time % HOURS_PER_DAY;
        }

        clearScreen(); 

        printf("Фермер: %s | День %d, %02d:00\n", farmer_name, current_day, current_time);
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Инверсия карманов\n");
        printf("[7] Поиск предмета по названию\n");
        printf("[8] Записать состояние в дневник\n");
        printf("[9] Расшифровать старые записи\n");
        printf("[10] Отправиться в приключение (Подземелье)\n");
        printf("Выберите действие: ");
        action = getValidatedInput();

        switch(action)
        {
            case 1:
                printf("Текущее время: День %d, %02d:00\n", current_day, current_time);
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 2:
                printf("Сколько часов вы хотите поработать? "); time = getValidatedInput();
                if (time < 0) printf("Ошибка: нельзя поработать отрицательное количество часов!\n");
                else { current_time = time + current_time; printf("Прошло %d часов.\n", time); }
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 3:
                showInventory();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 4:
                putItemInSlot();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 5:
                throwItemFromSlot();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 6:
                invertInventory();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 7:
                searchItemByName();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 8:
                writeDiary();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 9:
                processNpcTemplate();
                printf("Нажмите Enter для продолжения..."); getchar(); break;
            case 10:
                playDungeon();
                printf("Нажмите Enter для возврата в меню..."); getchar(); break;
            case 0:
                printf("Выход из игры...\n");
                freeDungeonMap(); 
                break;
            default:
                printf("Неверный пункт меню!\n");
                printf("Нажмите Enter для продолжения..."); getchar(); break;
        }
    }
    return 0;
}
