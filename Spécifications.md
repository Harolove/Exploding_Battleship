# Règles du jeu

## Déroulement d'une partie 

Les règles du jeu sont les même que celles du jeu classique. L'objectif de chaque joueurs est de couler tous les bateaux de la flotte adverse. Le joueur qui a le plus de bateaux encore debout à la fin gagne la partie.

### But
Le **Joueur** doit couler toute la flotte du **Joueur Adverse** avant que toute sa flotte soit détruite.

### Mise en place
Chaque **Joueur** place ses bateaux sur la grille, abaisse la vitre en lexant pour actionner un interrupteur qui va servir de sécurité des deux côtés.

### Tour de jeu
Chaque **Joueur** sélectionne à son tour à l'aide des 2 codeurs rotatifs (un pour la colonne l'autre pour la ligne de la case de tir, pendant que la carte du **Joueur adverse** déjà existante est en arrière plan, quand le curseur sélectionne une case indiquée par l'écran de LED, la couleur de la case devient plus claire, puis à l'aide du bouton poussoir situé entre les codeurs il effectue son tir.

### Conséquence
Si une case désignée contient un des bateaux encore debout du **Joueur Adverse** la capacité associée explose. L'écran neoPixel du **Joueur** affiche la case en Rouge.
Si une case désignée ne contient aucune partie d'un bateau encore debout du **Joueur Adverse** l'écran neoPixel affiche la case en Vert. Lorsque un bateau entier est touché, toutes les cases correspondantes changent de couleur en noir.


## Fonctionnement

### Type et nombre de bateau
Il y a 4 type de bateaux, chaque bateau a une identité unique pour les reconnaître par le microprocesseur.
* Destroyer : Petit bateau de 2 de long, il y en a 3.
* Croiseur : Bateau de 3 de long, il y en a 2.
* Cuirasé : Grand bateau de 4 de long.
* Porte-avion : Très grand bateau de 5 de long.

<img width="484" height="484" alt="matrice-1" src="https://github.com/user-attachments/assets/1d71e5fc-27d1-4758-8202-37f381b97126" />

### Exemple de Partie

<img width="484" height="484" alt="matrice-1" src="https://github.com/user-attachments/assets/98c238f0-ef2d-4c64-9373-71ec208c5aef" />

### Explications des couleurs de l'afficheur neoPixels

<img width="401" height="118" alt="matrice-couleurs-1" src="https://github.com/user-attachments/assets/f3762051-f107-4a64-abf5-b0c051280189" />
Il y'a 4 couleurs afficher sur l'écran neoPixel.
* Rouge = Bateau touché
* Bleu = L'eau
* Vert = Tir dans l'eau (raté)
* Noir = Bateau détruit

<img width="401" height="118" alt="matrice-curseur-1" src="https://github.com/user-attachments/assets/bf32a5fe-39e7-4674-98f7-bf30141f6c9b" />
Lorsque le joueur utilise déplace le curseur sur l'afficheur les couleurs sont moins intense

