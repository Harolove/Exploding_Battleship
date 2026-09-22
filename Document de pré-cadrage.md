# Document de pré-cadrage

## 1. Objectif

(S) Battleship est une adaptation électronique et moins ennuyante du jeu de 
société de bataille navale. Les joueurs possèdent au départ la même configuration de jeu : grille, nombre et type de bateaux... Chaque joueur dispose d’une grille physique en PCB
de **8 × 8 cases**, sur laquelle il place ses bateaux avant le début de la partie. 
Il dispose également d'une grille **8 × 8 cases** en LED neopixel en face de lui pour visualiser l'état actuel de la carte du joueur adverse pendant la partie.

Pendant la partie, les joueurs sélectionnent chacun leur tour une case de la grille adverse à l’aide
 de deux commandes, une pour sélectionner la ligne et l'autre pour la colonne, puis valident leur choix en appuyant sur le bouton **FEU**. Le système détermine alors si le tir est raté, touché ou s’il coule un bateau.

En cas de tir réussi, la capacité associé à la case du bateau explose.

(M) Non connu encore

(A)/ (R) Nous sommes 6, afin de rendre le projet le plus réalisable possible.

(T) Nous avons jusqu'à avril pour rendre un prototype fonctionnel du projet. 
Mais il est fort probable que le professeur encadrant souhaite que le projet soit fonctionnel pour les journées portes ouvertes en février 2027.

## 2. Lots PBS

|Lot|Taches|
|----|----|
|Hardware||
|Software||

### 2.1. Compétences OBS

 6 personnes sur le projet. 

### 2.2. Taches WBS

| SEP                                                                                                       | OCT                                        | NOV                                        | DEC                                        | JAN                                        | FEV                                        | MAR                                        | AVR                       |
| --------------------------------------------------------------------------------------------------------- | ------------------------------------------ | ------------------------------------------ | ------------------------------------------ | ------------------------------------------ | ------------------------------------------ | ------------------------------------------ | ------------------------- |
| Mesure électrique lors de l'explosion des capacités (Tension, pic de courant, dégagement de gaz, etc...). | **Pas assez d'information pour le moment** | **Pas assez d'information pour le moment** | **Pas assez d'information pour le moment** | **Pas assez d'information pour le moment** | **Pas assez d'information pour le moment** | **Pas assez d'information pour le moment** | Livraision d'un prototype |
| Définition des spécifications du projet.                                                                  |                                            |                                            |                                            |                                            |                                            |                                            |                           |
| Création d'un document de pré-cadrage.                                                                    |                                            |                                            |                                            |                                            |                                            |                                            |                           |

## 3. Contraintes

| Contrainte                                               | Exigence                                                                                                         |
|:--------------------------------------------------------:| ---------------------------------------------------------------------------------------------------------------- |
| Sécurité                                                 | Assurer la sécurité physique des joueurs. Les protéger des risques électriques, des éclatements des capacités et les acides et gaz qui s'échappent. |
| Deadline                                                 | Une deadline connue en avril.                                                                                    |
| Système de mise à feu                                    | Trouver un moyen physique et/ou électrique de mettre les capacités sous tension.                                 |
| Matériel/ Technique                                      | 3 types de PCB différents + 2 PCB par bateau, le nombre de commandes à effectuer est élevé.                      |
| Système de communication entre le plateau et les bateaux | Obligation d'utiliser le bus I2C pour communiquer entre les différents éléments.                                      |
| Système de localisation                                  | Les bateaux peuvent être déployés en longueur ou en largeur et le système doit pouvoir les détecter.             |

## 4. Risques

Au vu des contraintes sécuritaires, il est fort probable que faire exploser des capacités soit interdit dans le futur, ou non réalisable avec le matériel disponible. Dans ce cas-là, il ne s'agira plus que d'une version électrique d'un jeu de société déja bien connu.
