#include <conio.h>   // _kbhit() et _getch() : lire le clavier sans appuyer sur Entrée
#include <ctype.h>   // tolower() : accepter les touches en majuscules ou minuscules
#include <stdio.h>   // printf(), sprintf(), fwrite() : affichage
#include <stdlib.h>  // rand(), srand(), system()
#include <windows.h> // Fonctions de la console Windows (curseur, Sleep)
#include <time.h>    // time() : graine pour le générateur aléatoire

// Dimensions de l'aire de jeu (en cases) et longueur maximale de la queue
#define HEIGHT 20
#define WIDTH 40
#define MAX_TAIL 100

// Positions de chaque segment de la queue (X = colonne, Y = ligne)
int snakeTailX[MAX_TAIL], snakeTailY[MAX_TAIL];
int snakeTailLen;            // Nombre de segments actuels de la queue
int gameover, key, score;    // Fin de partie (0/1), direction actuelle, score
int x, y, fruitx, fruity;    // Position de la tête et position du fruit

// Place le curseur de la console en (x, y).
// On s'en sert pour redessiner par-dessus l'image précédente au lieu d'effacer l'écran.
void goToXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Cache le curseur clignotant de la console pour un affichage plus propre
void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// Initialise une nouvelle partie
void setup() {
    srand(time(NULL)); // Graine aléatoire différente à chaque lancement
    gameover = 0;
    score = 0;
    snakeTailLen = 0;

    // Le serpent commence au centre de l'aire de jeu
    x = WIDTH / 2;
    y = HEIGHT / 2;

    // Direction de départ : 0 = immobile jusqu'à la première touche
    key = 0;

    // Position aléatoire du premier fruit (à l'intérieur des murs)
    fruitx = rand() % (WIDTH - 2) + 1;
    fruity = rand() % (HEIGHT - 2) + 1;
}

// Dessine l'aire de jeu, le serpent, le fruit et le score
void draw() {
    // On construit toute l'image en mémoire, puis on l'affiche d'un seul coup :
    // afficher caractère par caractère ferait clignoter l'écran.
    static char frame[(WIDTH + 3) * (HEIGHT + 2) + 128];
    char *p = frame; // Pointeur vers la prochaine case libre du tampon

    // Bordure du haut
    for (int i = 0; i < WIDTH + 2; i++) *p++ = '-';
    *p++ = '\n';

    // Chaque ligne : mur gauche, contenu des cases, mur droit
    for (int i = 0; i < HEIGHT; i++) {
        *p++ = '#';
        for (int j = 0; j < WIDTH; j++) {
            char c = ' '; // Case vide par défaut
            if (i == y && j == x) {
                c = 'O'; // Tête du serpent
            } else if (i == fruity && j == fruitx) {
                c = '*'; // Fruit
            } else {
                // Cherche si un segment de la queue occupe cette case
                for (int k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == j && snakeTailY[k] == i) {
                        c = 'o'; // Corps du serpent
                        break;
                    }
                }
            }
            *p++ = c;
        }
        *p++ = '#';
        *p++ = '\n';
    }

    // Bordure du bas, puis score et commandes
    for (int i = 0; i < WIDTH + 2; i++) *p++ = '-';
    p += sprintf(p, "\nScore: %d\nControls: W, A, S, D | X to Quit\n", score);

    // Retour en haut à gauche (plus rapide que system("cls")), puis affichage de l'image
    goToXY(0, 0);
    fwrite(frame, 1, p - frame, stdout);
    fflush(stdout); // Force l'affichage immédiat
}

// Lit la touche pressée (s'il y en a une) et change la direction.
// Directions : 1 = gauche, 2 = droite, 3 = haut, 4 = bas.
// On interdit le demi-tour direct (ex. : aller à gauche quand on va à droite).
void input() {
    if (_kbhit()) { // Une touche a été pressée ? (ne bloque pas le jeu)
        switch (tolower(_getch())) {
            case 'a': if(key != 2) key = 1; break;
            case 'd': if(key != 1) key = 2; break;
            case 'w': if(key != 4) key = 3; break;
            case 's': if(key != 3) key = 4; break;
            case 'x': gameover = 1; break; // Quitter la partie
        }
    }
}

// Met à jour l'état du jeu à chaque tour
void logic() {
    // 1. La queue suit la tête : chaque segment prend la place de celui qui le précède,
    //    et le premier segment prend l'ancienne position de la tête.
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

    // 2. Déplacement de la tête selon la direction
    switch (key) {
        case 1: x--; break; // Gauche
        case 2: x++; break; // Droite
        case 3: y--; break; // Haut
        case 4: y++; break; // Bas
    }

    // 3. Collision avec un mur : fin de partie
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        gameover = 1;

    // 4. Collision avec sa propre queue : fin de partie
    for (int i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == x && snakeTailY[i] == y)
            gameover = 1;
    }

    // 5. Le serpent mange le fruit : +10 points, la queue grandit, nouveau fruit
    if (x == fruitx && y == fruity) {
        score += 10;
        if (snakeTailLen < MAX_TAIL) {
            // Le nouveau segment apparaît sur le dernier, pour éviter
            // un segment « fantôme » affiché en (0,0)
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
    // Efface l'écran une seule fois au démarrage
    system("cls");
    hideCursor();
    setup();

    // Boucle principale : afficher, lire le clavier, mettre à jour, attendre
    while (!gameover) {
        draw();
        input();
        logic();
        // Vitesse du jeu (en millisecondes) : plus lent en vertical,
        // car les caractères sont plus hauts que larges
        Sleep(key == 3 || key == 4 ? 150 : 100);
    }

    _getch(); // Attend une touche avant de fermer la fenêtre
    return 0;
}
