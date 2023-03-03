#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "tetromino.c"

#define FIELDWIDTH 10
#define FIELDHEIGHT 22
#define EMPTYCHAR '~'
#define FALLSPEED 1
#define CLEARCOLOUR 8

char FIELD[FIELDWIDTH][FIELDHEIGHT];
char FIELDCOLOUR[FIELDWIDTH][FIELDHEIGHT];
int score = 0;

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

void draw_tetromino(int tetromino_type, int tetromino_x, int tetromino_y) {
    attron(COLOR_PAIR(tetromino_type + 1));
    mvaddch_into_field(tetromino_y, tetromino_x, tetromino_y+'0');
    for (int i = 0; i < 3; ++i) {
        mvaddch_into_field(tetromino_y + TETROMINOSHAPES[tetromino_type][i][1], tetromino_x + TETROMINOSHAPES[tetromino_type][i][0], '#');
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

void solidify(int tetromino_type, int tetromino_x, int tetromino_y) {
    FIELD[tetromino_x][tetromino_y] = '#';
    FIELDCOLOUR[tetromino_x][tetromino_y] = tetromino_type + 1;
    for (int i = 0; i < 3; ++i) {
        FIELD[tetromino_x + TETROMINOSHAPES[tetromino_type][i][0]][tetromino_y + TETROMINOSHAPES[tetromino_type][i][1]] = '#';
        FIELDCOLOUR[tetromino_x + TETROMINOSHAPES[tetromino_type][i][0]][tetromino_y + TETROMINOSHAPES[tetromino_type][i][1]] = tetromino_type + 1;
    }
    clear_complete_lines();
}

int move_horiz(int amt, int tetromino_type, int tetromino_x, int tetromino_y) {
    int x = tetromino_x + amt;
    int y = tetromino_y;
    if (x < 0 || x >= FIELDWIDTH || FIELD[x][y] != EMPTYCHAR)
        return 0;
    for (int i = 0; i < 3; ++i) {
        x = tetromino_x + amt + TETROMINOSHAPES[tetromino_type][i][0];
        y = tetromino_y + TETROMINOSHAPES[tetromino_type][i][1];
        if (x < 0 || x >= FIELDWIDTH || FIELD[x][y] != EMPTYCHAR)
            return 0;
    }
    return amt;
}

int move_down(int tetromino_type, int tetromino_x, int tetromino_y) {
    int x = tetromino_x;
    int y = tetromino_y + 1;
    if (y >= FIELDHEIGHT || FIELD[x][y] != EMPTYCHAR) {
        solidify(tetromino_type, tetromino_x, tetromino_y);
        return 0;
    }
    for (int i = 0; i < 3; ++i) {
        x = tetromino_x + TETROMINOSHAPES[tetromino_type][i][0];
        y = tetromino_y + 1 + TETROMINOSHAPES[tetromino_type][i][1];
        if (y >= FIELDHEIGHT || FIELD[x][y] != EMPTYCHAR) {
            solidify(tetromino_type, tetromino_x, tetromino_y);
            return 0;
        }
    }
    return 1;
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

    int tetromino_type = 0;
    int tetromino_x, tetromino_y;
    bool gaming = true;
    bool new_tetromino_please = true;
    while (gaming) {
        if (new_tetromino_please) {
            tetromino_type = rand() % 7;
            tetromino_x = FIELDWIDTH / 2 - 2;
            tetromino_y = 0;
            new_tetromino_please = false;
        }

        clear();
        draw_field();
        draw_tetromino(tetromino_type, tetromino_x, tetromino_y);
        refresh();

        if (move_down(tetromino_type, tetromino_x, tetromino_y))
            ++tetromino_y;
        else
            new_tetromino_please = true;

        switch (getch()) {
            case 'a':
                tetromino_x += move_horiz(-1, tetromino_type, tetromino_x, tetromino_y);
                break;
            case 'd':
                tetromino_x += move_horiz(+1, tetromino_type, tetromino_x, tetromino_y);
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
