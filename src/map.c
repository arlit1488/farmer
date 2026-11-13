#include "map.h"
#include <time.h>
#include <stdbool.h>

struct Map* createMap(int depth) {
    struct Map *m = (struct Map*)malloc(sizeof(struct Map));
    m->width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    m->depth = depth;

    m->grid = (char**)malloc(m->height * sizeof(char*));
    for (int i = 0; i < m->height; i++) m->grid[i] = (char*)malloc(m->width * sizeof(char));

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (y == 0 || y == m->height - 1 || x == 0 || x == m->width - 1) {
                m->grid[y][x] = T_WALL;
            } else {
                int chance = rand() % 100;
                if (chance < 15) m->grid[y][x] = T_TREE;
                else if (chance < 30) m->grid[y][x] = T_ROCK;
                else if (chance < 45) m->grid[y][x] = T_ICE;
                else m->grid[y][x] = T_FLOOR;
            }
        }
    }
    return m;
}

void destroyMap(struct Map **m) {
    if (*m != NULL) {
        for (int i = 0; i < (*m)->height; i++) free((*m)->grid[i]);
        free((*m)->grid);
        printf("Память структуры по адресу %p освобождена\n", (void*)*m);
        free(*m);
        *m = NULL;
    }
}

void playAdventure(struct Player *p, int *day, int *hour) {
    // Проверки на допуск
    if (p->stamina <= 0) { printf("Вы слишком устали для похода!\n"); return; }
    if (*hour >= 2 && *hour < 8) { printf("Сейчас ночь, фермер должен спать!\n"); return; }

    struct Map *m = createMap(1);
    
    // Спавн героя
    do {
        p->x = 1 + rand() % (m->width - 2);
        p->y = 1 + rand() % (m->height - 2);
    } while (m->grid[p->y][p->x] != T_FLOOR);

    // Спавн лестницы
    int dx, dy;
    do {
        dy = 1 + rand() % (m->height - 2); dx = 1 + rand() % (m->width - 2);
    } while ((dy == p->y && dx == p->x) || m->grid[dy][dx] != T_FLOOR);
    m->grid[dy][dx] = T_DOWN;

    while (1) {
        clearScreen();
        printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n", 
               p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold, m->depth, *hour);
        
        for (int y = 0; y < m->height; y++) {
            for (int x = 0; x < m->width; x++) {
                if (y == p->y && x == p->x) printf("%c", T_HERO);
                else printf("%c", m->grid[y][x]);
            }
            printf("\n");
        }
        printf("W/A/S/D - движение, Q - выйти домой\nВаш ход: ");
        
        char move; scanf(" %c", &move); while (getchar() != '\n');
        
        if (move == 'Q' || move == 'q') {
            printf("Вы вернулись домой с %d золота!\n", p->gold);
            destroyMap(&m);
            return;
        }

        int old_y = p->y, old_x = p->x, new_y = p->y, new_x = p->x;
        if (move == 'W' || move == 'w') new_y--;
        else if (move == 'S' || move == 's') new_y++;
        else if (move == 'A' || move == 'a') new_x--;
        else if (move == 'D' || move == 'd') new_x++;
        else continue;

        if (new_y < 0 || new_y >= m->height || new_x < 0 || new_x >= m->width) continue;
        char target = m->grid[new_y][new_x];
        if (target == T_WALL || target == T_TREE || target == T_ROCK) continue;

        p->stamina--; // Трата выносливости за шаг
        
        // Скользкий лед
        if (target == T_ICE) {
            printf("Вы наступили на лед!\n");
            int dy = new_y - old_y, dx = new_x - old_x;
            bool stopped = false;
            p->y = new_y; p->x = new_x; // первый шаг
            
            while (!stopped) {
                int next_y = p->y + dy, next_x = p->x + dx;
                if (next_y < 0 || next_y >= m->height || next_x < 0 || next_x >= m->width) break;
                
                char next_tile = m->grid[next_y][next_x];
                if (next_tile == T_WALL || next_tile == T_TREE || next_tile == T_ROCK) {
                    stopped = true;
                } else {
                    p->y = next_y; p->x = next_x;
                    if (next_tile != T_ICE) stopped = true;
                }
            }
        } else {
            p->y = new_y; p->x = new_x;
        }

        // Переход на новый этаж
        if (m->grid[p->y][p->x] == T_DOWN) {
            printf("\nВы спускаетесь глубже...\n");
            p->stamina -= 5;
            (*hour)++;
            p->gold += BASE_GOLD_REWARD * m->depth;
            destroyMap(&m);
            m = createMap(m->depth + 1);
            // спавн героя и лестницы на новом этаже
            do { p->x = 1 + rand() % (m->width - 2); p->y = 1 + rand() % (m->height - 2); } while (m->grid[p->y][p->x] != T_FLOOR);
            do { dy = 1 + rand() % (m->height - 2); dx = 1 + rand() % (m->width - 2); } while ((dy == p->y && dx == p->x) || m->grid[dy][dx] != T_FLOOR);
            m->grid[dy][dx] = T_DOWN;
            printf("Нажмите Enter..."); getchar();
        }

        // Экстренная эвакуация
        if (p->stamina <= 0 || (*hour >= 2 && *hour < 8)) {
            clearScreen();
            printf("\n!!! Экстренная эвакуация !!!\n");
            if (p->stamina <= 0) printf("Вы упали без сил.\n");
            else printf("Наступила ночь, вы потеряли сознание.\n");
            p->gold /= 2;
            p->stamina = 0;
            printf("Спасатели забрали половину золота. Остаток: %d\n", p->gold);
            destroyMap(&m);
            return;
        }
    }
}