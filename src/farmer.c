#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // для strcmp, strcpy, strlen, strncpy

// =================== КОНСТАНТЫ =======================
#define INVENTORY_SIZE  10
#define MAX_ITEMS       10
#define MAX_NAME_LEN    32
#define HOURS_PER_DAY   24
#define MAX_LINE_LEN    256

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

// ===================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ====================
char farmer_name[MAX_NAME_LEN] = "Фермер";
int  current_day  = 1;
int  current_time = 8;
int  inventory[INVENTORY_SIZE] = {WOOD, STONE, WOOD, SEEDS, STONE,
                                  WOOD, IRON, STONE, SEEDS, EMPTY_SLOT};
// Каталог названий предметов: item_names[ID] = "Название"
char item_names[MAX_ITEMS][MAX_NAME_LEN];

// ==================== СЛУЖЕБНЫЕ ФУНКЦИИ =====================
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Безопасный ввод целого числа
int getValidatedInput(void) {
    int value;
    while (scanf("%d", &value) != 1) {
        while (getchar() != '\n');
        printf("Ошибка ввода! Пожалуйста, введите число: ");
    }
    while (getchar() != '\n');   // съесть '\n'
    return value;
}

// Безопасный ввод строки (с пробелами)
void getValidatedString(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';   // убрать '\n'
        } else {
            // строка длиннее буфера — дренируем остаток
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

// ==================== КАТАЛОГ ПРЕДМЕТОВ =======================
// Значения по умолчанию (если items.txt не найден)
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

// Загрузка каталога из items.txt
void loadItemNames(void) {
    FILE* file = fopen(ITEMS_FILE, "r");
    if (file == NULL) {
        printf("Предупреждение: файл %s не найден!\n", ITEMS_FILE);
        printf("Загружены названия по умолчанию.\n");
        setDefaultItemNames();
        return;
    }

    // Обнуляем массив
    for (int i = 0; i < MAX_ITEMS; i++) item_names[i][0] = '\0';

    char line[MAX_LINE_LEN];
    int loaded = 0;

    // Формат: "ID Название" построчно
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

    // Дозаполняем пропущенные ID
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (item_names[i][0] == '\0') {
            snprintf(item_names[i], MAX_NAME_LEN, "Предмет_%d", i);
        }
    }

    printf("Каталог предметов загружен из %s (%d записей).\n",
           ITEMS_FILE, loaded);
}

// Получить название предмета по ID
const char* getItemName(int id) {
    if (id < 0 || id >= MAX_ITEMS) return "???";
    return item_names[id];
}

// Найти ID по названию (через strcmp). -1 если не найден
int findItemIdByName(const char* name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], name) == 0) {
            return i;
        }
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
        printf("Ошибка: индекс слота должен быть от 0 до %d!\n",
               INVENTORY_SIZE - 1);
        return;
    }

    printf("Введите ID предмета (0-%d): ", MAX_ITEM_ID);
    int item_id = getValidatedInput();
    if (item_id < 0 || item_id > MAX_ITEM_ID) {
        printf("Ошибка: ID предмета должен быть от 0 до %d!\n",
               MAX_ITEM_ID);
        return;
    }

    inventory[slot] = item_id;
    printf("Предмет \"%s\" (ID: %d) помещён в слот %d.\n",
           getItemName(item_id), item_id, slot);
}

void throwItemFromSlot(void) {
    printf("Введите индекс слота для выбрасывания (0-%d): ",
           INVENTORY_SIZE - 1);
    int slot = getValidatedInput();
    if (slot < 0 || slot >= INVENTORY_SIZE) {
        printf("Ошибка: индекс слота должен быть от 0 до %d!\n",
               INVENTORY_SIZE - 1);
        return;
    }

    if (inventory[slot] != EMPTY_SLOT) {
        printf("Предмет \"%s\" (ID: %d) выброшен из слота %d.\n",
               getItemName(inventory[slot]), inventory[slot], slot);
        inventory[slot] = EMPTY_SLOT;
    } else {
        printf("В этом слоте и так пусто!\n");
    }
}

// Функция варианта "Инверсия карманов"
void invertInventory(void) {
    printf("\nИНВЕРСИЯ КАРМАНОВ\n");
    
    // 1. Выводим массив ДО преобразования
    printf("\nИнвентарь ДО\n");
    showInventory();
    
    // 2. Переворачиваем массив 
    for (int i = 0, j = INVENTORY_SIZE - 1; i < j; i++, j--) {
        int temp = inventory[i];   // временная переменная для обмена
        inventory[i] = inventory[j];
        inventory[j] = temp;
    }
    
    // 3. Выводим массив ПОСЛЕ преобразования
    printf("\nИнвентарь ПОСЛЕ\n");
    showInventory();
    
    printf("Инвентарь перевёрнут задом наперёд!\n");
}

// ====================== MAIN ====================
int main(void)
{
    int action = -1;
    int time;

    loadItemNames();
    
    while (action != 0)
    {
        if (current_time >= 24)
        {
            current_day = current_day + current_time / HOURS_PER_DAY;
            current_time = current_time % HOURS_PER_DAY;
        }

        clearScreen(); // очищаем экран каждый раз перед выводом меню

        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время (Поработать)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Инверсия карманов\n");
        printf("Выберите действие: ");
        action = getValidatedInput();

        switch(action)
        {
            // первое действие
            case 1:
                printf("Текущее время: День %d, %02d:00\n", current_day, current_time);
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;

            // второе действие
            case 2:
                printf("Сколько часов вы хотите поработать? ");
                time = getValidatedInput();
                if (time < 0) {
                    printf("Ошибка: нельзя поработать отрицательное количество часов!\n");
                } else {
                    current_time = time + current_time;
                    printf("Прошло %d часов.\n", time);
                }
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;

            // третье действие - инвентарь
            case 3:
                showInventory();
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;

            // четвёртое действие - положить предмет
            case 4:
                putItemInSlot();
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;

            // пятое действие - выбросить предмет
            case 5:
            {
                throwItemFromSlot();
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;
            }

            // шестое действие - вариант "Инверсия карманов"
            case 6:
                invertInventory();
                printf("Нажмите Enter для продолжения...");
                getchar();
                break;
        }
    }
    return 0;
}
