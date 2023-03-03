#include <stdint.h>
#include <ncurses.h>

// shapes defined as the relative coordinates of the other three squares that make up the tetromino (the square at 0, 0 is assumed)
// rotations are performed about (0, 0)
int TETROMINOSHAPES[7][3][2] = {
    { // 1x4
        {-1, 0},
        {1, 0},
        {2, 0},
    },
    { // 2x2
        {1, 0},
        {0, 1},
        {1, 1},
    },
    { // J
        {-1, 0},
        {1, 0},
        {1, 1},
    },
    { // L
        {-1, 0},
        {-1, 1},
        {1, 0},
    },
    { // S
        {1, 0},
        {0, 1},
        {-1, 1},
    },
    { // T
        {-1, 0},
        {1, 0},
        {0, 1},
    },
    { // Z
        {-1, 0},
        {0, 1},
        {1, 1},
    },
};

const short TETROMINOCOLOURS[7] = {
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_WHITE,
    COLOR_MAGENTA,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_CYAN,
};
