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
#define NB_POMMES 3   // Nombre de pommes affichées en même temps

// Positions de chaque segment de la queue (X = colonne, Y = ligne)
int snakeTailX[MAX_TAIL], snakeTailY[MAX_TAIL];
int snakeTailLen;
int gameover, key, score;
int x, y;
int fruitx[NB_POMMES], fruity[NB_POMMES]; // Positions des pommes
int fruitActif[NB_POMMES];                // 1 = pomme encore à manger
int pommesRestantes;                      // Pommes restant à manger dans la série
int modeOrdi;   // 0 = joueur, 1 = ordinateur
int vitesse;    // pause entre deux images, en millisecondes

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

// Renvoie 1 si la case (px, py) est occupée par la tête, la queue ou une pomme active
int caseOccupee(int px, int py) {
    if (px == x && py == y) return 1;
    for (int i = 0; i < snakeTailLen; i++)
        if (snakeTailX[i] == px && snakeTailY[i] == py) return 1;
    for (int i = 0; i < NB_POMMES; i++)
        if (fruitActif[i] && fruitx[i] == px && fruity[i] == py) return 1;
    return 0;
}

// Génère une nouvelle série de trois pommes sur des cases libres
void genererPommes() {
    for (int i = 0; i < NB_POMMES; i++) fruitActif[i] = 0;
    for (int i = 0; i < NB_POMMES; i++) {
        int px, py;
        do {
            px = rand() % (WIDTH - 2) + 1;
            py = rand() % (HEIGHT - 2) + 1;
        } while (caseOccupee(px, py));
        fruitx[i] = px;
        fruity[i] = py;
        fruitActif[i] = 1;
    }
    pommesRestantes = NB_POMMES;
}

// Renvoie 1 s'il y a une pomme active en (px, py)
int pommeEn(int px, int py) {
    for (int i = 0; i < NB_POMMES; i++)
        if (fruitActif[i] && fruitx[i] == px && fruity[i] == py) return 1;
    return 0;
}

// Initialise une nouvelle partie
void setup() {
    srand(time(NULL)); // Graine aléatoire différente à chaque lancement
    gameover = 0;
    score = 0;
    snakeTailLen = 0;

    // Initial snake position
    x = WIDTH / 2;
    y = HEIGHT / 2;

    // Initial direction (0 = stopped)
    key = 0;

    // Direction de départ : 0 = immobile jusqu'à la première touche
    key = 0;

    // Première série de trois pommes
    genererPommes();
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
            } else if (pommeEn(j, i)) {
                c = '*'; // Pomme
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
    if (modeOrdi)
        p += sprintf(p, "\nScore: %d\nMode ordinateur | X pour quitter\n", score);
    else
        p += sprintf(p, "\nScore: %d\nControles : Z, Q, S, D | X pour quitter\n", score);

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
            case 'q': if(key != 2) key = 1; break;
            case 'd': if(key != 1) key = 2; break;
            case 'z': if(key != 4) key = 3; break;
            case 's': if(key != 3) key = 4; break;
            case 'x': gameover = 1; break; // Quitter la partie
        }
    }
}

// Distance (Manhattan) entre (px, py) et la pomme active la plus proche
int distancePomme(int px, int py) {
    int best = WIDTH + HEIGHT;
    for (int i = 0; i < NB_POMMES; i++) {
        if (!fruitActif[i]) continue;
        int d = abs(fruitx[i] - px) + abs(fruity[i] - py);
        if (d < best) best = d;
    }
    return best;
}

// Mode ordinateur : essaie les 4 directions et garde celle qui rapproche le plus d'une pomme
void choisirDirection() {
    int dx[5] = {0, -1, 1, 0, 0};   // deplacement en x pour key = 1, 2, 3, 4
    int dy[5] = {0, 0, 0, -1, 1};   // deplacement en y pour key = 1, 2, 3, 4
    int meilleure = 0;
    int meilleureDistance = WIDTH + HEIGHT + 1;

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

        // Garder la direction la plus proche de la pomme la plus proche
        int distance = distancePomme(nx, ny);
        if (distance < meilleureDistance) {
            meilleureDistance = distance;
            meilleure = d;
        }
    }

    if (meilleure != 0) key = meilleure;
}

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

    // 5. Le serpent mange une pomme : +10 points. Une fois les trois pommes mangées,
    //    la queue grandit d'un segment et trois nouvelles pommes apparaissent.
    for (int i = 0; i < NB_POMMES; i++) {
        if (fruitActif[i] && x == fruitx[i] && y == fruity[i]) {
            fruitActif[i] = 0;
            pommesRestantes--;
            score += 10;
        }
    }

    if (pommesRestantes == 0 && !gameover) {
        if (snakeTailLen < MAX_TAIL) {
            // Le nouveau segment apparaît sur le dernier, pour éviter
            // un segment « fantôme » affiché en (0,0)
            if (snakeTailLen > 0) {
                snakeTailX[snakeTailLen] = snakeTailX[snakeTailLen - 1];
                snakeTailY[snakeTailLen] = snakeTailY[snakeTailLen - 1];
            } else {
                snakeTailX[0] = x;
                snakeTailY[0] = y;
            }
            snakeTailLen++;
        }
        genererPommes();
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
