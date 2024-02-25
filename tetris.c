#include <conio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "tetris.h"

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

int score = 0;
bool gameOver = false;
int currTetrominoIdx;
int currRotation = 0;
int currX = A_WIDTH / 2;
int currY = 0;

int main() {
    system("cls");
    memset(arena, 0, sizeof(arena[0][0]) * A_HEIGHT * A_WIDTH);
    newTetromino();

    const int targetFrameTime = 300;
    clock_t lastTime = clock();

    while (!gameOver) {
        clock_t now = clock();
        clock_t elapsed = (now - lastTime) * 1000 / CLOCKS_PER_SEC;

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

        clock_t frameEnd = clock();
        clock_t currFrameTime = (frameEnd - now) * 1000 / CLOCKS_PER_SEC;

        int delay = targetFrameTime - currFrameTime;
        if (delay > 0) {
            Sleep(delay);
        }
    }

    system("cls");
    printf("Game over!\n");
    printf("Score: %d\n", score);
    return 0;
}

void newTetromino() {
    currTetrominoIdx = rand() % 7;
    currRotation = 0;
    currX = (A_WIDTH / 2) - (T_WIDTH / 2);
    currY = 0;
    gameOver = !isValidPosition(currTetrominoIdx, currRotation, currX, currY);
}

bool isValidPosition(int tetromino, int rotation, int posX, int posY) {
    for (int x = 0; x < T_WIDTH; x++) {
        for (int y = 0; y < T_HEIGHT; y++) {
            int index = rotate(x, y, rotation);
            if (tetrominoes[tetromino][index] != 1) {
                continue;
            }

            int arenaX = x + posX;
            int arenaY = y + posY;
            if (arenaX < 0 || arenaX >= A_WIDTH || arenaY >= A_HEIGHT) {
                return false;
            }

            int arenaXY = arena[arenaY][arenaX];
            if (arenaY >= 0 && arenaXY == 1) {
                return false;
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
    case 0:
        return x + y * 4;
    case 1:
        return 12 + y - (x * 4);
    case 2:
        return 15 - (y * 4) - x;
    case 3:
        return 3 - y + (x * 4);
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
            if (isValidPosition(currTetrominoIdx, nextRotation, currX, currY)) {
                currRotation = nextRotation;
            }
            break;
        case 75:  // Left arrow key
            if (isValidPosition(currTetrominoIdx, currRotation, currX - 1,
                                currY)) {
                currX--;
            }
            break;
        case 77:  // Right arrow key
            if (isValidPosition(currTetrominoIdx, currRotation, currX + 1,
                                currY)) {
                currX++;
            }
            break;
        case 80:  // Down arrow key
            if (isValidPosition(currTetrominoIdx, currRotation, currX,
                                currY + 1)) {
                currY++;
            }
            break;
        }
    }
}

bool moveDown() {
    if (isValidPosition(currTetrominoIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void addToArena() {
    for (int y = 0; y < T_HEIGHT; y++) {
        for (int x = 0; x < T_WIDTH; x++) {
            int index = rotate(x, y, currRotation);
            if (tetrominoes[currTetrominoIdx][index] != 1) {
                continue;
            }

            int arenaX = currX + x;
            int arenaY = currY + y;
            if (arenaX >= 0 && arenaX < A_WIDTH && arenaY >= 0 &&
                arenaY < A_HEIGHT) {
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
            if (arena[y][x] == 0) {
                lineFull = false;
                break;
            }
        }

        if (lineFull) {
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
    }

    if (clearedLines > 0) {
        score += 100 * clearedLines;
    }
}

void gotoXy(int x, int y) {
    COORD pos = {x, y};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

void drawArena() {
    system("cls");

    char buffer[1024];

    for (int y = 0; y < A_HEIGHT; y++) {
        int bufferIndex = 0;
        buffer[bufferIndex++] = '|';

        for (int x = 0; x < A_WIDTH; x++) {
            int rotatedPos = rotate(x - currX, y - currY, currRotation);
            if (arena[y][x] == 1) {
                buffer[bufferIndex++] = '#';
            } else if (x >= currX && x < currX + T_WIDTH && y >= currY &&
                       y < currY + T_HEIGHT &&
                       tetrominoes[currTetrominoIdx][rotatedPos] == 1) {
                buffer[bufferIndex++] = '#';
            } else {
                buffer[bufferIndex++] = ' ';
            }
        }

        buffer[bufferIndex++] = '|';
        buffer[bufferIndex] = '\0';

        gotoXy(0, y);
        printf("%s\n", buffer);
    }

    gotoXy(0, A_HEIGHT + 1);
    printf("Score: %d\n", score);
}