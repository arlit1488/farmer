#include <stdio.h>
#include <stdlib.h>

// объявление переменных
#define INVENTORY_SIZE 10
#define EMPTY_SLOT 0
#define WOOD 1
#define STONE 2
#define SEEDS 3
#define IRON 4
#define GOLD 5
#define WOOL 6
#define MILK 7
#define BREAD 8
#define FISH 9
#define MAX_ITEM_ID FISH   // максимальный ID предмета

int current_day = 1;
int current_time = 8;

// инвентарь (статический массив на 10 слотов)
int inventory[INVENTORY_SIZE] = {WOOD, STONE, WOOD, SEEDS, STONE, WOOD, IRON, STONE, SEEDS, EMPTY_SLOT};

// Функция очистки экрана
void clearScreen(void) {
#ifdef _WIN32
    system("cls");   // Windows
#else
    system("clear"); // Linux / macOS
#endif
}

// Функция для проверки корректности ввода числа
int getValidatedInput() {
    int value;
    while (scanf("%d", &value) != 1) {
        // Очищаем буфер ввода
        while (getchar() != '\n');
        printf("Ошибка ввода! Пожалуйста, введите число: ");
    }
    // Очищаем буфер от лишних символов
    while (getchar() != '\n');
    return value;
}

// Функция для вывода названия предмета по ID
void printItemName(int id) {
    switch (id) {
        case EMPTY_SLOT: printf("пусто");       break;
        case WOOD:       printf("дерево");      break;
        case STONE:      printf("камень");      break;
        case SEEDS:      printf("семена");      break;
        case IRON:       printf("железо");      break;
        case GOLD:       printf("золото");      break;
        case WOOL:       printf("шерсть");      break;
        case MILK:       printf("молоко");      break;
        case BREAD:      printf("хлеб");        break;
        case FISH:       printf("рыба");        break;
        default:         printf("неизвестный"); break;
    }
}

// Функция вывода инвентаря
void showInventory(void) {
    printf("\nИНВЕНТАРЬ\n");
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        printf("Слот %d: [%d]", i, inventory[i]);
        printf(" (");
        printItemName(inventory[i]);
        printf(")");
        printf("\n");
    }
}

// Функция "Положить предмет в слот"
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
    
    printf("Предмет ");
    printItemName(item_id);
    printf(" (ID: %d) помещён в слот %d.\n", item_id, slot);
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

int main(void)
{
    int action = -1;
    int time;

    while (action != 0)
    {
        if (current_time >= 24)
        {
            current_day = current_day + current_time / 24;
            current_time = current_time % 24;
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
                printf("Введите индекс слота для выбрасывания (0-%d): ", INVENTORY_SIZE - 1);
                int slot = getValidatedInput();
                if (slot < 0 || slot >= INVENTORY_SIZE) {
                    printf("Ошибка: индекс слота должен быть от 0 до %d!\n", INVENTORY_SIZE - 1);
                    printf("Нажмите Enter для продолжения...");
                    getchar();
                    break;
                }
                
                if (inventory[slot] != EMPTY_SLOT) {
                    printf("Предмет ");
                    printItemName(inventory[slot]);
                    printf(" (ID: %d) выброшен из слота %d.\n", inventory[slot], slot);
                    inventory[slot] = EMPTY_SLOT;
                } else {
                    printf("В этом слоте и так пусто!\n");
                }
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

