# Snake

Un jeu Snake en C qui se joue dans la console Windows.

```
------------------------------------------
#                                        #
#        *                               #
#                   Ooooo                #
#                                  *     #
#             *                          #
#                                        #
------------------------------------------
Score: 30
Controles : Z, Q, S, D | P pour sauvegarder | X pour quitter
```

## Fonctionnalités

- **Deux modes de jeu** : vous dirigez le serpent (mode joueur), ou l'ordinateur joue tout seul (mode ordinateur).
- **Deux vitesses** : lente ou rapide.
- **Plusieurs pommes** : trois pommes sont posées en même temps sur le plateau. Chaque pomme mangée rapporte 10 points. Quand les trois sont mangées, le serpent grandit d'un segment et trois nouvelles pommes apparaissent.
- **Sauvegarde** : la partie en cours peut être enregistrée, puis reprise au lancement suivant.

## Prérequis

- Windows : le jeu utilise `conio.h` et `windows.h`.
- Un compilateur C11 (MinGW, MSVC ou le compilateur fourni avec CLion).
- CMake 3.20 ou plus récent.

## Compilation

```sh
cmake -S . -B build
cmake --build build
```

L'exécutable `snake.exe` se trouve ensuite dans `build/` (ou `build/Debug/` avec MSVC).

Sans CMake, avec MinGW :

```sh
gcc -std=c11 -o snake.exe snake.c
```

Le projet s'ouvre aussi directement dans CLion.

## Lancer une partie

Au démarrage, un menu propose :

1. **Nouvelle partie**, ou **reprendre la partie sauvegardée**
2. Le mode : **Joueur** ou **Ordinateur**
3. La vitesse : **Lente** ou **Rapide**

Si vous reprenez une partie, le mode et la vitesse sont ceux de la partie sauvegardée.

## Commandes

| Touche | Action                     |
|--------|----------------------------|
| `Z`    | Haut                       |
| `Q`    | Gauche                     |
| `S`    | Bas                        |
| `D`    | Droite                     |
| `P`    | Sauvegarder la partie      |
| `X`    | Quitter                    |

Les touches sont pensées pour un clavier AZERTY. Le serpent ne peut pas faire demi-tour d'un coup. En mode ordinateur, seules `P` et `X` sont actives.

## Règles

- Le serpent reste immobile jusqu'à la première touche.
- La partie s'arrête si le serpent touche un mur ou sa propre queue.
- La queue fait au plus 100 segments.

## Affichage

| Caractère | Signification       |
|-----------|---------------------|
| `O`       | Tête du serpent     |
| `o`       | Corps du serpent    |
| `*`       | Pomme               |
| `#` / `-` | Murs                |

## Sauvegarde

Avec `P`, la partie est écrite dans `sauvegarde.txt`, dans le dossier depuis lequel le jeu est lancé. C'est un fichier texte qui contient la position du serpent, sa direction, le score, le mode, la vitesse, les pommes et les segments de la queue. Si ce fichier est absent ou abîmé au moment de la reprise, une nouvelle partie démarre.

## Structure du code

Tout le jeu tient dans `snake.c` :

| Fonction                                 | Rôle                                                   |
|------------------------------------------|--------------------------------------------------------|
| `menu()`                                 | Écran de départ (reprise, mode, vitesse)               |
| `setup()`                                | Initialise une nouvelle partie                         |
| `draw()`                                 | Dessine le plateau d'un seul bloc, sans clignotement   |
| `input()`                                | Lit le clavier sans bloquer le jeu                     |
| `choisirDirection()`                     | Choisit la direction en mode ordinateur                |
| `logic()`                                | Déplacement, collisions, pommes mangées                |
| `sauvegarderPartie()` / `chargerPartie()` | Écrit et relit `sauvegarde.txt`                       |

Les dimensions du plateau (`WIDTH`, `HEIGHT`), la longueur maximale de la queue (`MAX_TAIL`) et le nombre de pommes (`NB_POMMES`) se règlent avec les `#define` en haut du fichier.
