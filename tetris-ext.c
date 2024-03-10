#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "tetris-ext.h"

const int tetrominoes[7][16] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // O
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
    {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};

int arena[A_HEIGHT][A_WIDTH];

uint32_t score = 0;
bool gameOver = false;
int currTetrominoIdx;
int currRotation = 0;
int currX = A_WIDTH / 2;
int currY = 0;

int main() {
    printf("\e[2J\e[H");  // clear screen
    memset(arena, 0, sizeof(arena[0][0]) * A_HEIGHT * A_WIDTH);
    newTetromino();

    const int targetFrameTime = 500;
    clock_t lastTime = clock();

    while (!gameOver) {
        clock_t now = clock();
        clock_t elapsed = getMs(now, lastTime);
        processInputs();

        if (elapsed >= targetFrameTime) {
            if (!moveDown()) {
                addToArena();
                checkLines();
                newTetromino();
            }
            lastTime = now;
        }

        drawArena();
        Sleep(10);
    }

    printf("\e[2J\e[H");  // clear screen
    printf("\e[6;37m");   // set style
    printf("GAME OVER\nScore: %d\n", score);
    printf("\e[0m");  // reset style
    return 0;
}

clock_t getMs(clock_t start, clock_t end) {
    return (start - end) * 1000 / CLOCKS_PER_SEC;
}

BufferBuilder *bbCreate(size_t initialSize) {
    BufferBuilder *bb = malloc(sizeof(BufferBuilder));
    if (!bb) {
        return NULL;
    }

    bb->buffer = malloc(initialSize);
    if (!bb->buffer) {
        free(bb);
        return NULL;
    }

    bb->buffer[0] = '\0';
    bb->buffLen = initialSize;
    bb->byteLen = 0;
    return bb;
}

int bbAppend(BufferBuilder *bb, const char *str) {
    if (!bb || !str) {
        return -1;
    }

    size_t strByteLen = strlen(str);
    if (bb->byteLen + strByteLen + 1 > bb->buffLen) {
        size_t newSize = bb->buffLen;
        while (bb->byteLen + strByteLen + 1 > newSize) {
            newSize *= 2;
        }

        char *newBuffer = realloc(bb->buffer, newSize);
        if (!newBuffer) {
            return -1;
        }

        bb->buffer = newBuffer;
        bb->buffLen = newSize;
    }

    strcat(bb->buffer, str);
    bb->byteLen += strByteLen;
    return 0;
}

void bbFree(BufferBuilder *bb) {
    if (!bb) {
        return;
    }
    free(bb->buffer);
    free(bb);
}

void newTetromino() {
    currTetrominoIdx = rand() % 7;
    currRotation = 0;
    currX = (A_WIDTH / 2) - (T_WIDTH / 2);
    currY = 0;
    gameOver = !validPos(currTetrominoIdx, currRotation, currX, currY);
}

bool validPos(int tetromino, int rotation, int posX, int posY) {
    for (int x = 0; x < T_WIDTH; x++) {
        for (int y = 0; y < T_HEIGHT; y++) {
            int index = rotate(x, y, rotation);
            if (1 != tetrominoes[tetromino][index]) {
                continue;
            }

            int arenaX = x + posX;
            int arenaY = y + posY;
            if (0 > arenaX || A_WIDTH <= arenaX || A_HEIGHT <= arenaY) {
                return false;
            }

            int arenaXY = arena[arenaY][arenaX];
            if (0 <= arenaY && 1 == arenaXY) {
                return false;
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
    case 0:
        return x + y * T_WIDTH;
    case 1:
        return 12 + y - (x * T_WIDTH);
    case 2:
        return 15 - (y * T_WIDTH) - x;
    case 3:
        return 3 - y + (x * T_WIDTH);
    default:
        return 0;
    }
}

void processInputs() {
    if (!_kbhit()) {
        return;
    }

    while (_kbhit()) {
        int key = _getch();
        switch (key) {
        case 32:  // Spacebar
            int nextRotation = (currRotation + 1) % 4;
            if (validPos(currTetrominoIdx, nextRotation, currX, currY)) {
                currRotation = nextRotation;
            }
            break;
        case 75:  // Left arrow key
            if (validPos(currTetrominoIdx, currRotation, currX - 1, currY)) {
                currX--;
            }
            break;
        case 77:  // Right arrow key
            if (validPos(currTetrominoIdx, currRotation, currX + 1, currY)) {
                currX++;
            }
            break;
        case 80:  // Down arrow key
            if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
                currY++;
            }
            break;
        }
    }
}

bool moveDown() {
    if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void addToArena() {
    for (int y = 0; y < T_HEIGHT; y++) {
        for (int x = 0; x < T_WIDTH; x++) {
            int index = rotate(x, y, currRotation);
            if (1 != tetrominoes[currTetrominoIdx][index]) {
                continue;
            }

            int arenaX = currX + x;
            int arenaY = currY + y;
            bool xInRange = (0 <= arenaX) && (arenaX < A_WIDTH);
            bool yInRange = (0 <= arenaY) && (arenaY < A_HEIGHT);
            if (xInRange && yInRange) {
                arena[arenaY][arenaX] = 1;
            }
        }
    }
}

void checkLines() {
    int clearedLines = 0;

    for (int y = A_HEIGHT - 1; y >= 0; y--) {
        bool lineFull = true;
        for (int x = 0; x < A_WIDTH; x++) {
            if (0 == arena[y][x]) {
                lineFull = false;
                break;
            }
        }

        if (!lineFull) {
            continue;
        }

        clearedLines++;
        for (int yy = y; yy > 0; yy--) {
            for (int xx = 0; xx < A_WIDTH; xx++) {
                arena[yy][xx] = arena[yy - 1][xx];
            }
        }

        for (int xx = 0; xx < A_WIDTH; xx++) {
            arena[0][xx] = 0;
        }
        y++;
    }

    if (0 < clearedLines) {
        score += 100 * clearedLines;
    }
}

void drawArena() {
    printf("\e[?25l");   // hide cursor
    printf("\e[H");      // reset cursor position
    printf("\e[0m");     // reset style
    printf("\e[2;37m");  // set style

    BufferBuilder *bb = bbCreate(512);
    if (!bb) {
        return;
    }

    bbAppend(bb, "\xda");
    for (int x = 0; x < (A_WIDTH * 2); x++) {
        bbAppend(bb, "\xc4");
    }
    bbAppend(bb, "\xbf\n");

    for (int y = 0; y < A_HEIGHT; y++) {
        bbAppend(bb, "\xb3");

        for (int x = 0; x < A_WIDTH; x++) {
            int rotatedPos = rotate(x - currX, y - currY, currRotation);
            bool validX = x >= currX && x < currX + T_WIDTH;
            bool validY = y >= currY && y < currY + T_HEIGHT;
            bool xyFilled = 1 == tetrominoes[currTetrominoIdx][rotatedPos];

            if (1 == arena[y][x]) {
                bbAppend(bb, "\e[0;37m");
                bbAppend(bb, "\xdb\xdb");
                bbAppend(bb, "\e[2;37m");
            } else if (validX && validY && xyFilled) {
                bbAppend(bb, "\e[0;37m");
                bbAppend(bb, "\xdb\xdb");
                bbAppend(bb, "\e[2;37m");
            } else {
                bbAppend(bb, ". ");
            }
        }

        bbAppend(bb, "\xb3");
        bbAppend(bb, "\n");
    }

    bbAppend(bb, "\xc0");
    for (int x = 0; x < (A_WIDTH * 2); x++) {
        bbAppend(bb, "\xc4");
    }
    bbAppend(bb, "\xd9\n");

    printf("%s", bb->buffer);

    printf("\e[0m");  // reset style
    printf("\e[1m");  // set style
    printf("\n\nScore: %d\n\n", score);

    bbFree(bb);
}
