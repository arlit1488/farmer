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
    printf("\n--- ВРЕМЯ И СОН ---\n");
    printf("1. Посмотреть на часы\n");
    printf("2. Ждать\n");
    printf("3. Спать\n");
    printf("Выбор: ");
    
    int choice = getValidatedInput();
    
    switch (choice) {
        case 1:
            printf("Текущее время: День %d, %02d:00\n", *day, *hour);
            break;
            
        case 2: {
            printf("Сколько часов ждать? ");
            int h = getValidatedInput();
            
            if (h <= 0) {
                printf("Ошибка: нужно ждать хотя бы 1 час!\n");
                break;
            }
            
            int target = *hour + h;
            
            // Проверка: нельзя промотать время через 02:00 ночи
            if ((*hour < 2 && target >= 2) || (*hour >= 2 && target >= 26)) { 
                printf("Нельзя ждать до утра, нужно спать!\n"); 
                break; 
            }
            
            *hour += h;
            if (*hour >= 24) { 
                *day += *hour / 24; 
                *hour %= 24; 
            }
            printf("Время продвинулось. Теперь День %d, %02d:00\n", *day, *hour);
            break;
        }
            
        case 3:
            (*day)++; 
            *hour = 8;
            if (p) { 
                p->hp = p->max_hp; 
                p->stamina = p->max_stamina; 
                printf("Вы отдохнули. HP и Выносливость полностью восстановлены!\n"); 
            }
            break;
            
        default:
            // Сюда попадают все числа: 0, 4, 5, -10 и т.д.
            printf("Ошибка: неверный пункт меню! Введите 1, 2 или 3.\n");
            break;
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
        printf("[12] Купить масло для фонаря\n");
        printf("[0] Выход\nВыберите действие: ");
        
        action = getValidatedInput();

        if (action >= 2 && action <= 12 && action != 8 && player == NULL) {
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
            case 12: buyLanternOil(player); break;
            case 0: break;
            default: printf("Неверный пункт.\n"); break;
        }
        if (action != 0) { printf("\nНажмите Enter для продолжения..."); getchar(); }
    }

    destroyPlayer(&player);
    return 0;
}
