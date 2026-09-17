# Règle du jeu

## Objectifs
Le but est identique au jeu de bataille navale classique. Deux joueurs placent leur flotte de bateaux sur leur grille respective et, à tour de rôle, désignent une case de la grille du joueur adverse dans l'objectif de toucher un bateau ennemi. Un bateau est considéré comme détruit quand toutes les cases qu'il occupe ont été touchées. Le joueur gagnant est celui qui a détruit toute la flotte adverse.

## Mise en place
Les participants décident du nombre et du type de bateaux (par exemple 10 bateaux de 2 ou encore 2 bateaux de 5, etc.). Ils placent ensuite leur flotte sur la grille, en longueur ou en largeur (le grand PCB). Quand tous leurs bateaux ont été positionnés, ils abaissent ensuite la vitre en lexan. (IMPORTANT) La partie ne peut démarrer que lorsque la vitre est abaissée pour des raisons de sécurité.

## Tour de jeu
Chaque Joueur sélectionne à son tour à l'aide des 2 codeurs rotatifs (un pour la colonne l'autre pour la ligne de la case de tir), pendant que la carte du Joueur adverse déjà existante est en arrière plan sur l'écran de LED, quand le curseur sélectionne une case indiquée par l'écran de LED, la couleur de la case devient plus claire, puis à l'aide du bouton poussoir situé entre les codeurs il effectue son tir.

## Conséquences
Si une case désignée contient un des bateaux encore debout du Joueur adverse la capacité associée explose.

# Fonctionnement

## Bateau
Il y a 4 type de bateaux, chaque bateau a une identité unique et sont de taille différente.
  * Destroyer, un petit bateau de 2 de longueur.
  * Croiseur, un bateau de 3 de longueur.
  * Cuirasé, un grand bateau de 4 de longueur.
  * Porte-avion, le plus grand type disponible de 5 de longueur.

## Afficheur neoPixel
Il y a 4 couleurs affichable :
  * Bleu = l'état par défaut (l'eau), une case qui n'a pas encore frappé
  * Vert = un tir raté, la case frappée ne contenait aucun bateau
  * Rouge = touché, la case frappée contenait un bateau
  * Noir = le bateau est entièrement détruit

## Échappement des gaz SUPER mortel
À l'aide d'une petite soufflante on évacue tout le gaz des capacités explosées pour la sécurité.

# Dimensionnement
## Grille de jeu
8 cases de long pour 8 cases de largueur, chaque case est espacée l'une de l'autre selon la taille d'une LED

## Afficheur neoPixel
8 LEDs de long pour 8 LEDs de largeur (64), chaque LED est espacée l'une de l'autre de (à déterminer selon les dimensions de la LED). L'intensité (à déterminer) est diffusé par du papier diffusant et dépend de l'état du jeu.

## Capacité
### Réference
(à déterminer)

### Tension de mise à feu
(à déterminer)

### Pic de courant
(à déterminer)

### Gaz EXTRÊMEMENT mortel
(à déterminer)



