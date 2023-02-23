#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "tetromino.c"

#define FIELDWIDTH 10
#define FIELDHEIGHT 22
#define FALLSPEED 1

char FIELD[FIELDWIDTH][FIELDHEIGHT];

void init_game() {
    memset(FIELD, '~', FIELDWIDTH * FIELDHEIGHT);
}

void mvaddch_into_field(int y, int x, char c) {
    mvaddch(y + 1, x + 1, c);
}

void draw_field() {
    int i, j;

    attron(COLOR_PAIR(8));
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
            mvaddch_into_field(j, i, FIELD[i][j]);
        }
    }
}

void draw_tetromino(int tetromino_type, int tetromino_x, int tetromino_y) {
    attron(COLOR_PAIR(tetromino_type + 1));
    mvaddch_into_field(tetromino_y, tetromino_x, '#');
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

    int tetromino_type = -1;
    int tetromino_x, tetromino_y;
    bool gaming = true;
    while (gaming) {
        if (tetromino_type < 0) {
            tetromino_type = rand() % 7;
            tetromino_x = tetromino_type;
            tetromino_y = 0;
        }

        clear();
        draw_field();
        draw_tetromino(tetromino_type, tetromino_x, tetromino_y);
        refresh();

        tetromino_y = (tetromino_y + FALLSPEED) % FIELDHEIGHT;

        switch (getch()) {
            case 'a':
                if (tetromino_x > 0) --tetromino_x;
                break;
            case 'd':
                if (tetromino_x < FIELDWIDTH - 1) ++tetromino_x;
                break;
            case 'q':
                gaming = false;
                break;
        }
    }

    getch();
    endwin();

    return 0;
}
