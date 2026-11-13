#include "utils.h"

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
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        else { int c; while ((c = getchar()) != '\n' && c != EOF); }
    }
}

void loadItemNames(char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    FILE* file = fopen(ITEMS_FILE, "r");
    if (file == NULL) {
        printf("Предупреждение: файл %s не найден. Используем стандартные названия.\n", ITEMS_FILE);
        const char* def_names[] = {"Пусто", "Дерево", "Камень", "Семена", "Железо", "Золото", "Шерсть", "Молоко", "Хлеб", "Рыба"};
        for (int i = 0; i < MAX_ITEMS; i++) strcpy(item_names[i], def_names[i]);
        return;
    }
    for (int i = 0; i < MAX_ITEMS; i++) item_names[i][0] = '\0';
    char line[MAX_LINE_LEN]; int id; char name[MAX_NAME_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d %31s", &id, name) == 2 && id >= 0 && id < MAX_ITEMS) {
            strncpy(item_names[id], name, MAX_NAME_LEN - 1);
        }
    }
    fclose(file);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (item_names[i][0] == '\0') snprintf(item_names[i], MAX_NAME_LEN, "Предмет_%d", i);
    }
}

const char* getItemName(int id, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    if (id < 0 || id >= MAX_ITEMS) return "???";
    return item_names[id];
}

int findItemIdByName(const char* name, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], name) == 0) return i;
    }
    return -1;
}

void writeDiary(const char* name, int day, int time, int inventory[INVENTORY_SIZE], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    FILE* file = fopen(DIARY_FILE, "a");
    if (!file) { printf("Ошибка открытия %s\n", DIARY_FILE); return; }
    fprintf(file, "\n=== Запись дневника ===\nФермер: %s\nВремя: День %d, %02d:00\nИнвентарь:\n", name, day, time);
    for (int i = 0; i < INVENTORY_SIZE; i++) fprintf(file, "  Слот %d: [%d] - %s\n", i, inventory[i], getItemName(inventory[i], item_names));
    fprintf(file, "======================\n");
    fclose(file);
    printf("Состояние записано в файл %s\n", DIARY_FILE);
}

// Вспомогательные функции для NPC (оставлены здесь для краткости)
void appendToBuffer(char* dest, size_t destSize, size_t* pos, const char* src) {
    while (*src != '\0' && *pos < destSize - 1) { dest[*pos] = *src; (*pos)++; src++; }
    dest[*pos] = '\0';
}

void processNpcTemplate(const char* name, int day, int inventory[INVENTORY_SIZE], char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    FILE* in = fopen(INPUT_FILE, "r");
    if (!in) { printf("Ошибка открытия %s\n", INPUT_FILE); return; }
    FILE* out = fopen(OUTPUT_FILE, "w");
    if (!out) { printf("Ошибка открытия %s\n", OUTPUT_FILE); fclose(in); return; }

    char line[MAX_LINE_LEN], result[MAX_OUTPUT_LINE_LEN];
    char dayBuf[16]; snprintf(dayBuf, sizeof(dayBuf), "%d", day);
    printf("\n=== Письмо NPC ===\n");
    
    while (fgets(line, sizeof(line), in) != NULL) {
        size_t pos = 0; result[0] = '\0';
        for (size_t i = 0; line[i] != '\0'; ) {
            if (strncmp(&line[i], "<NAME>", 6) == 0) { appendToBuffer(result, sizeof(result), &pos, name); i += 6; }
            else if (strncmp(&line[i], "<DAY>", 5) == 0) { appendToBuffer(result, sizeof(result), &pos, dayBuf); i += 5; }
            else if (strncmp(&line[i], "<ITEM>", 6) == 0) { appendToBuffer(result, sizeof(result), &pos, getItemName(inventory[0], item_names)); i += 6; }
            else { if (pos < sizeof(result) - 1) { result[pos++] = line[i]; result[pos] = '\0'; } i++; }
        }
        printf("%s", result); fputs(result, out);
    }
    fclose(in); fclose(out);
    printf("\n=== Конец письма ===\n");
}