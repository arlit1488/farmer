#include "map.h"
#include <time.h>

// Перечисление для пунктов меню
enum MenuAction {
    ACT_EXIT = 0,
    ACT_CREATE = 1,
    ACT_CARD = 2,
    ACT_INVENTORY = 3,
    ACT_PUT = 4,
    ACT_THROW = 5,
    ACT_INVERT = 6,
    ACT_SEARCH = 7,
    ACT_TIME_MENU = 8,
    ACT_DIARY = 9,
    ACT_NPC = 10,
    ACT_ADVENTURE = 11
};

void timeMenu(int *day, int *hour, struct Player *p) {
    printf("\n1. Посмотреть на часы\n2. Ждать\n3. Спать\nВыбор: ");
    int choice = getValidatedInput();
    if (choice == 1) {
        printf("Текущее время: День %d, %02d:00\n", *day, *hour);
    } else if (choice == 2) {
        printf("Сколько часов ждать? ");
        int h = getValidatedInput();
        if (h <= 0) return;
        // Нельзя пройти через 02:00 ночи
        int target = *hour + h;
        if (*hour >= 2 && target >= 26) { printf("Нельзя ждать до утра, нужно спать!\n"); return; }
        if (*hour < 2 && target >= 2) { printf("Нельзя ждать до утра, нужно спать!\n"); return; }
        
        *hour += h;
        if (*hour >= 24) { *day += *hour / 24; *hour %= 24; }
        printf("Время продвинулось.\n");
    } else if (choice == 3) {
        (*day)++; *hour = 8;
        if (p) { p->hp = p->max_hp; p->stamina = p->max_stamina; printf("Вы отдохнули. HP и Выносливость восстановлены!\n"); }
    }
}

int main(void) {
    srand(time(NULL));
    int day = 1, hour = 8;
    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    loadItemNames(item_names);

    struct Player *player = NULL; // Изначально игрока нет
    int action = -1;

    while (action != ACT_EXIT) {
        clearScreen();
        printf("=== ВЕСЁЛЫЙ ФЕРМЕР | День %d, %02d:00 ===\n", day, hour);
        printf("[1] Создать / пересоздать персонажа\n");
        printf("[2] Карточка персонажа\n");
        printf("[3] Инвентарь\n");
        printf("[4] Положить предмет\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Инверсия карманов\n");
        printf("[7] Поиск по названию\n");
        printf("[8] Время и сон\n");
        printf("[9] Записать дневник\n");
        printf("[10] Расшифровать записи\n");
        printf("[11] Отправиться в приключение\n");
        printf("[0] Выход\nВыберите действие: ");
        
        action = getValidatedInput();

        // Защита: пункты 2-7, 9-11 требуют наличия игрока
        if (action >= 2 && action <= 11 && action != 8 && player == NULL) {
            printf("Сначала создайте персонажа!\n");
            printf("Нажмите Enter..."); getchar();
            continue;
        }

        switch(action) {
            case 1:
                if (player != NULL) {
                    printf("Персонаж уже существует. Точно удалить? (1 - Да, 0 - Нет): ");
                    if (getValidatedInput() == 1) {
                        destroyPlayer(&player);
                        player = createPlayer();
                    }
                } else {
                    player = createPlayer();
                }
                break;
            case 2: printPlayerCard(player, day, hour, item_names); break;
            case 3: showInventory(player, item_names); break;
            case 4: putItemInSlot(player, item_names); break;
            case 5: throwItemFromSlot(player, item_names); break;
            case 6: invertInventory(player, item_names); break;
            case 7: searchItemByName(player, item_names); break;
            case 8: timeMenu(&day, &hour, player); break;
            case 9: writeDiary(player->name, day, hour, player->inventory, item_names); break;
            case 10: processNpcTemplate(player->name, day, player->inventory, item_names); break;
            case 11: playAdventure(player, &day, &hour); break;
            case 0: break;
            default: printf("Неверный пункт.\n"); break;
        }
        if (action != 0) { printf("\nНажмите Enter для продолжения..."); getchar(); }
    }

    destroyPlayer(&player);
    return 0;
}