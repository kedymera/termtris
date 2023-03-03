#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "tetromino.c"

typedef uint8_t u8;
typedef int8_t i8;

#define FIELDWIDTH 10
#define FIELDHEIGHT 22
#define EMPTYCHAR '~'
#define FALLSPEED 1
#define CLEARCOLOUR 8

char FIELD[FIELDWIDTH][FIELDHEIGHT];
char FIELDCOLOUR[FIELDWIDTH][FIELDHEIGHT];
int score = 0;

struct Tetromino {
    u8 type;
    i8 x;
    i8 y;
    u8 rot;
};

void init_game() {
    memset(FIELD, EMPTYCHAR, FIELDWIDTH * FIELDHEIGHT);
    memset(FIELDCOLOUR, CLEARCOLOUR, FIELDWIDTH * FIELDHEIGHT);
}

void mvaddch_into_field(int y, int x, char c) {
    mvaddch(y + 1, x + 1, c);
}

void draw_field() {
    int i, j;

    attron(COLOR_PAIR(CLEARCOLOUR));
    // walls
    for (i = 0; i < FIELDHEIGHT + 2; ++i) {
        mvaddch(i, 0, '|');
        mvaddch(i, FIELDWIDTH + 1, '|');
    }
    for (i = 1; i < FIELDWIDTH + 1; ++i) {
        mvaddch(0, i, '-');
        mvaddch(FIELDHEIGHT + 1, i, '-');
    }

    // blocks in playspace
    for (i = 0; i < FIELDWIDTH; ++i) {
        for (j = 0; j < FIELDHEIGHT; ++j) {
            attron(COLOR_PAIR(FIELDCOLOUR[i][j]));
            mvaddch_into_field(j, i, FIELD[i][j]);
        }
    }
}

void get_field_coords_for_part(int *field_x, int *field_y, struct Tetromino tt, int i) {
    switch (tt.rot) {
        default:
        case 0:
            *field_x = tt.x + TETROMINOSHAPES[tt.type][i][0];
            *field_y = tt.y + TETROMINOSHAPES[tt.type][i][1];
            break;
        case 1:
            *field_x = tt.x - TETROMINOSHAPES[tt.type][i][1];
            *field_y = tt.y + TETROMINOSHAPES[tt.type][i][0];
            break;
        case 2:
            *field_x = tt.x - TETROMINOSHAPES[tt.type][i][0];
            *field_y = tt.y - TETROMINOSHAPES[tt.type][i][1];
            break;
        case 3:
            *field_x = tt.x + TETROMINOSHAPES[tt.type][i][1];
            *field_y = tt.y - TETROMINOSHAPES[tt.type][i][0];
            break;
    }
}

void draw_tetromino(struct Tetromino tt) {
    attron(COLOR_PAIR(tt.type + 1));
    int x = tt.x, y = tt.y;
    mvaddch_into_field(y, x, '#');
    for (int i = 0; i < 3; ++i) {
        get_field_coords_for_part(&x, &y, tt, i);
        mvaddch_into_field(y, x, '#');
    }
}

void clear_complete_lines() {
    int complete_count = 0;
    for (int j = FIELDHEIGHT - 1; j >= 0; --j) {
        bool is_complete = true;
        for (int i = 0; i < FIELDWIDTH; ++i)
            if (FIELD[i][j] == EMPTYCHAR) {
                is_complete = false;
                break;
            }
        if (!is_complete)
            continue;

        // move lines above down
        for (int j2 = j - 1; j2 >= 0; --j2)
            for (int i = 0; i < FIELDWIDTH; ++i) {
                FIELD[i][j2 + 1] = FIELD[i][j2];
                FIELDCOLOUR[i][j2 + 1] = FIELDCOLOUR[i][j2];
            }
        ++complete_count;
        ++j;
    }
    if (complete_count == 4) complete_count *= 2;
    score += complete_count * 100;
    // consecutive tetrises?
}

void solidify(struct Tetromino tt) {
    int x = tt.x, y = tt.y;
    FIELD[x][y] = '#';
    FIELDCOLOUR[x][y] = tt.type + 1;
    for (int i = 0; i < 3; ++i) {
        get_field_coords_for_part(&x, &y, tt, i);
        FIELD[x][y] = '#';
        FIELDCOLOUR[x][y] = tt.type + 1;
    }
    clear_complete_lines();
}

int move_horiz(int amt, struct Tetromino tt) {
    // project tt by amt in x
    tt.x += amt;
    int x = tt.x, y = tt.y;

    // and check for collisions
    if (x < 0 || x >= FIELDWIDTH || FIELD[x][y] != EMPTYCHAR)
        return 0;
    for (int i = 0; i < 3; ++i) {
        get_field_coords_for_part(&x, &y, tt, i);
        if (x < 0 || x >= FIELDWIDTH || FIELD[x][y] != EMPTYCHAR)
            return 0;
    }
    return amt;
}

int move_down(struct Tetromino tt) {
    ++tt.y;
    int x = tt.x, y = tt.y;
    if (y >= FIELDHEIGHT || FIELD[x][y] != EMPTYCHAR)
        return 0;
    for (int i = 0; i < 3; ++i) {
        get_field_coords_for_part(&x, &y, tt, i);
        if (y >= FIELDHEIGHT || FIELD[x][y] != EMPTYCHAR)
            return 0;
    }
    return 1;
}

int rotate(struct Tetromino tt) {
    int old_rot = tt.rot;
    tt.rot = (tt.rot + 1) % 4;
    // no need to check (0, 0) square
    for (int i = 0; i < 3; ++i) {
        int x, y;
        get_field_coords_for_part(&x, &y, tt, i);
        if (x < 0 || x >= FIELDWIDTH || y < 0 || y >= FIELDHEIGHT)
            return old_rot;
    }
    return tt.rot;
}

int main() {
    // init ncurses
    initscr();
    start_color();
    curs_set(0);
    halfdelay(10);
    noecho();
    time_t t;
    srand(time(&t));
    
    init_game();

    for (int i = 0; i < 7; ++i) {
        init_pair(i + 1, COLOR_BLACK, TETROMINOCOLOURS[i]);
    }
    init_pair(8, COLOR_WHITE, COLOR_BLACK);

    struct Tetromino tt = {.type = 0, .x = 0, .y = 0, .rot = 0};
    bool gaming = true;
    bool new_tetromino_please = true;
    while (gaming) {
        if (new_tetromino_please) {
            tt.type = rand() % 7;
            tt.x = FIELDWIDTH / 2 - 2;
            tt.y = 0;
            tt.rot = 0;
            new_tetromino_please = false;
        }

        clear();
        draw_field();
        draw_tetromino(tt);
        refresh();

        if (move_down(tt))
            ++tt.y;
        else {
            solidify(tt);
            new_tetromino_please = true;
        }

        switch (getch()) {
            case 'a':
                tt.x += move_horiz(-1, tt);
                break;
            case 'd':
                tt.x += move_horiz(+1, tt);
                break;
            case 'w':
                tt.rot = rotate(tt);
                break;
            case 'q':
                gaming = false;
                break;
        }
    }

    getch();
    endwin();
    printf("%d\n", score);

    return 0;
}
