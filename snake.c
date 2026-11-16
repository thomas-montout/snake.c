#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define HEIGHT 20
#define WIDTH 40
#define MAX_TAIL 100

int snakeTailX[MAX_TAIL], snakeTailY[MAX_TAIL];
int snakeTailLen;
int gameover, key, score;
int x, y, fruitx, fruity;

// Function to move cursor to (0,0) to prevent flickering
void goToXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Hide the blinking console cursor
void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void setup() {
    srand(time(NULL));
    gameover = 0;
    score = 0;
    snakeTailLen = 0;
    
    // Initial snake position
    x = WIDTH / 2;
    y = HEIGHT / 2;
    
    // Initial direction (0 = stopped)
    key = 0; 

    // Initial fruit position
    fruitx = rand() % (WIDTH - 2) + 1;
    fruity = rand() % (HEIGHT - 2) + 1;
}

void draw() {
    // Build the whole frame in memory, then write it in one go to avoid flicker
    static char frame[(WIDTH + 3) * (HEIGHT + 2) + 128];
    char *p = frame;

    for (int i = 0; i < WIDTH + 2; i++) *p++ = '-';
    *p++ = '\n';

    for (int i = 0; i < HEIGHT; i++) {
        *p++ = '#';
        for (int j = 0; j < WIDTH; j++) {
            char c = ' ';
            if (i == y && j == x) {
                c = 'O'; // Head
            } else if (i == fruity && j == fruitx) {
                c = '*'; // Fruit
            } else {
                for (int k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == j && snakeTailY[k] == i) {
                        c = 'o'; // Body
                        break;
                    }
                }
            }
            *p++ = c;
        }
        *p++ = '#';
        *p++ = '\n';
    }

    for (int i = 0; i < WIDTH + 2; i++) *p++ = '-';
    p += sprintf(p, "\nScore: %d\nControls: W, A, S, D | X to Quit\n", score);

    goToXY(0, 0); // Reset cursor instead of system("cls")
    fwrite(frame, 1, p - frame, stdout);
    fflush(stdout);
}

void input() {
    if (_kbhit()) {
        switch (tolower(_getch())) {
            case 'a': if(key != 2) key = 1; break;
            case 'd': if(key != 1) key = 2; break;
            case 'w': if(key != 4) key = 3; break;
            case 's': if(key != 3) key = 4; break;
            case 'x': gameover = 1; break;
        }
    }
}

void logic() {
    // 1. Logic for tail following
    int prevX = snakeTailX[0];
    int prevY = snakeTailY[0];
    int prev2X, prev2Y;
    snakeTailX[0] = x;
    snakeTailY[0] = y;

    for (int i = 1; i < snakeTailLen; i++) {
        prev2X = snakeTailX[i];
        prev2Y = snakeTailY[i];
        snakeTailX[i] = prevX;
        snakeTailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // 2. Direction movement
    switch (key) {
        case 1: x--; break;
        case 2: x++; break;
        case 3: y--; break;
        case 4: y++; break;
    }

    // 3. Wall Collision
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        gameover = 1;

    // 4. Tail Collision
    for (int i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == x && snakeTailY[i] == y)
            gameover = 1;
    }

    // 5. Eating Fruit
    if (x == fruitx && y == fruity) {
        score += 10;
        if (snakeTailLen < MAX_TAIL) {
            // New segment starts on the last one to avoid a ghost at (0,0)
            if (snakeTailLen > 0) {
                snakeTailX[snakeTailLen] = snakeTailX[snakeTailLen - 1];
                snakeTailY[snakeTailLen] = snakeTailY[snakeTailLen - 1];
            }
            snakeTailLen++;
        }
        fruitx = rand() % (WIDTH - 2) + 1;
        fruity = rand() % (HEIGHT - 2) + 1;
    }
}

int main() {
    // Clear screen once at the start
    system("cls");
    hideCursor();
    setup();

    while (!gameover) {
        draw();
        input();
        logic();
        // Controls game speed (vertical moves are slower since characters are taller than wide)
        Sleep(key == 3 || key == 4 ? 150 : 100);
    }
    
    _getch(); // Wait for user input before closing window
    return 0;
}