#include <stdio.h>
#include <ncurses.h>
#include <string.h>

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

int main() {
    // init ncurses
    initscr();
    halfdelay(10);
    noecho();
    
    init_game();

    int x = 0, y = 0;    
    bool gaming = true;
    while (gaming) {
        clear();
        draw_field();
        mvaddch_into_field(y, x, 'o');

        y = (y + FALLSPEED) % FIELDHEIGHT;

        switch (getch()) {
            case 'a':
                if (x > 0) --x;
                break;
            case 'd':
                if (x < FIELDWIDTH - 1) ++x;
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
