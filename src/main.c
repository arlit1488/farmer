#include "map.h"
#include <time.h>

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
    ACT_ADVENTURE = 11,
    ACT_BUY_OIL = 12
};

void timeMenu(int *day, int *hour, struct Player *p) {
    printf("\n1. Посмотреть на часы\n2. Ждать\n3. Спать\nВыбор: ");
    int choice = getValidatedInput();
    
    if (choice == 1) {
        printf("Текущее время: День %d, %02d:00\n", *day, *hour);
    } else if (choice == 2) {

    } else if (choice == 3) {

        if (p == NULL) {
            printf("Ошибка: спать некому! Сначала создайте персонажа.\n");
            return;
        }
        
        (*day)++; *hour = 8;
        p->hp = p->max_hp; 
        p->stamina = p->max_stamina; 
        printf("Вы отдохнули. HP и Выносливость восстановлены!\n"); 
    }
}

int main(void) {
    srand(time(NULL));
    int day = 1, hour = 8;
    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    loadItemNames(item_names);

    struct Player *player = NULL; 
    int action = -1;

    while (action != ACT_EXIT) {
        clearScreen();
        printf("=== ВЕСЁЛЫЙ ФЕРМЕР | День %d, %02d:00 ===\n", day, hour);
        printf("[%d] Создать / пересоздать персонажа\n", ACT_CREATE);
        printf("[%d] Карточка персонажа\n", ACT_CARD);
        printf("[%d] Инвентарь\n", ACT_INVENTORY);
        printf("[%d] Положить предмет\n", ACT_PUT);
        printf("[%d] Выбросить предмет\n", ACT_THROW);
        printf("[%d] Инверсия карманов\n", ACT_INVERT);
        printf("[%d] Поиск по названию\n", ACT_SEARCH);
        printf("[%d] Время и сон\n", ACT_TIME_MENU);
        printf("[%d] Записать дневник\n", ACT_DIARY);
        printf("[%d] Расшифровать записи\n", ACT_NPC);
        printf("[%d] Отправиться в приключение\n", ACT_ADVENTURE);
        printf("[%d] Купить масло для фонаря\n", ACT_BUY_OIL);
        printf("[%d] Выход\nВыберите действие: ", ACT_EXIT);
        
        action = getValidatedInput();

        if (action >= 2 && action <= 12 && action != 8 && player == NULL) {
            printf("Сначала создайте персонажа!\n");
            printf("Нажмите Enter..."); getchar();
            continue;
        }

        switch(action) {
            case ACT_CREATE:
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
            case ACT_CARD: printPlayerCard(player, day, hour, item_names); break;
            case ACT_INVENTORY: showInventory(player, item_names); break;
            case ACT_PUT: putItemInSlot(player, item_names); break;
            case ACT_THROW: throwItemFromSlot(player, item_names); break;
            case ACT_INVERT: invertInventory(player, item_names); break;
            case ACT_SEARCH: searchItemByName(player, item_names); break;
            case ACT_TIME_MENU: timeMenu(&day, &hour, player); break;
            case ACT_DIARY: writeDiary(player->name, day, hour, player->inventory, item_names); break;
            case ACT_NPC: processNpcTemplate(player->name, day, player->inventory, item_names); break;
            case ACT_ADVENTURE: playAdventure(player, &day, &hour); break;
            case ACT_BUY_OIL: buyLanternOil(player); break;
            case ACT_EXIT: break;
            default: printf("Неверный пункт.\n"); break;
        }
        if (action != 0) { printf("\nНажмите Enter для продолжения..."); getchar(); }
    }

    destroyPlayer(&player);
    return 0;
}
