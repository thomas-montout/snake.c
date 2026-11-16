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
int modeOrdi;   // 0 = joueur, 1 = ordinateur
int vitesse;    // pause entre deux images, en millisecondes

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

// Attend que l'utilisateur appuie sur '1' ou '2' et renvoie 1 ou 2
int choix() {
    int c;
    do {
        c = _getch();
    } while (c != '1' && c != '2');
    printf("%c\n", c);
    return c - '0';
}

// Ecran de depart ; renvoie 1 si le joueur veut reprendre la partie sauvegardee
int menu() {
    system("cls");
    printf("=== SNAKE ===\n\n");

    printf("1. Nouvelle partie\n");
    int reprise = (choix() == 2);

    printf("\n1. Joueur\n2. Ordinateur\n> ");
    modeOrdi = (choix() == 2);

    printf("\n1. Lente\n2. Rapide\n> ");
    vitesse = (choix() == 1) ? 100 : 50;

    return reprise;
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
    if (modeOrdi)
        p += sprintf(p, "\nScore: %d\nMode ordinateur | X pour quitter\n", score);
    else
        p += sprintf(p, "\nScore: %d\nControles : Z, Q, S, D | X pour quitter\n", score);

    goToXY(0, 0); // Reset cursor instead of system("cls")
    fwrite(frame, 1, p - frame, stdout);
    fflush(stdout);
}

void input() {
    if (_kbhit()) {
        switch (tolower(_getch())) {
            case 'q': if(key != 2) key = 1; break;
            case 'd': if(key != 1) key = 2; break;
            case 'z': if(key != 4) key = 3; break;
            case 's': if(key != 3) key = 4; break;
            case 'x': gameover = 1; break;
        }
    }
}

// Mode ordinateur : essaie les 4 directions et garde celle qui rapproche le plus de la pomme
void choisirDirection() {
    int dx[5] = {0, -1, 1, 0, 0};   // deplacement en x pour key = 1, 2, 3, 4
    int dy[5] = {0, 0, 0, -1, 1};   // deplacement en y pour key = 1, 2, 3, 4
    int meilleure = 0;
    int meilleureDistance = WIDTH + HEIGHT;

    for (int d = 1; d <= 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];

        // Ecarter les directions qui menent dans une paroi
        if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;

        // Ecarter les directions qui menent sur la queue
        int surQueue = 0;
        for (int i = 0; i < snakeTailLen; i++) {
            if (snakeTailX[i] == nx && snakeTailY[i] == ny) {
                surQueue = 1;
                break;
            }
        }
        if (surQueue) continue;

        // Garder la direction la plus proche de la pomme
        int distance = abs(fruitx - nx) + abs(fruity - ny);
        if (distance < meilleureDistance) {
            meilleureDistance = distance;
            meilleure = d;
        }
    }

    if (meilleure != 0) key = meilleure;
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
    hideCursor();

    // Start screen (the "reprise" answer is returned for the save feature, not done here)
    menu();
    setup();

    // Clear screen once before the game starts
    system("cls");

    while (!gameover) {
        draw();
        if (modeOrdi) {
            // The computer steers, but X still quits
            if (_kbhit() && tolower(_getch()) == 'x') gameover = 1;
            choisirDirection();
        } else {
            input();
        }
        logic();
        // Controls game speed (vertical moves are slower since characters are taller than wide)
        Sleep(key == 3 || key == 4 ? vitesse * 3 / 2 : vitesse);
    }

    _getch(); // Wait for user input before closing window
    return 0;
}
