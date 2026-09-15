<img width="484" height="484" alt="matrice-1" src="https://github.com/user-attachments/assets/1d71e5fc-27d1-4758-8202-37f381b97126" />
## Règles du jeu

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

![Matrice 8x8](matrice-1.png)

| N | B | N | B | N | B | N | B |
|---|---|---|---|---|---|---|---|
| N | B | N | B | N | B | N | B |
| N | B | B | B | N | B | N | B |
| N | B | N | B | B | B | N | B |
| B | B | N | B | N | B | N | B |
| B | B | B | B | N | B | B | B |
| B | B | N | B | N | B | B | B |
| B | B | N | B | B | B | B | B |
