# Battleship — Spécifications du projet

## 1. Présentation

Battleship est une adaptation électronique et moins boring du jeu de société de bataille navale. Chaque joueur dispose d’une grille physique de **8 × 8 cases**, sur laquelle il place ses bateaux avant le début de la partie.

Les joueurs sélectionnent ensuite une case de la grille adverse à l’aide des commandes, puis valident leur choix en appuyant sur le bouton **FEU**. Le système détermine alors si le tir est raté, touché ou s’il coule un bateau. L'afficheur LED neopixel affiche apres le résultat du tir.

## 2. Périmètre et choix envisagés

| Élément | Spécifications |
| --- | --- |
| Nombre de joueurs | 2 |
| Grille par joueur | 8 × 8 cases |
| Dimensions d'une grille | Environ 30 × 30 cm ; dimensions finales **(à développer)** |
| Types de PCB | Plateau, bateau, pcb capa (changeable) |
| Microcontrôleurs | STM32 ; L476 |
| Communication plateau/bateaux | I²C, plateau maître/bateaux esclaves |

## 3. Architecture matérielle

### 3.1. PCB plateau

Le PCB plateau constitue la grille de jeu et la carte principale de chaque côté. Il doit accueillir les bateaux et assurer :

- la lecture des commandes du joueur ;
- la gestion de la grille et des règles du jeu ;
- l’identification des bateaux et de leurs cases occupées ;
- les communications avec les PCB bateau ;
- la gestion de l’affichage du neopixel et des résultats des tirs ;
- la vérification de la sécurité plexiglass ;

Les dimensions totales sont **(à développer)**.

### 3.2. PCB bateau

Chaque bateau est constitué d’un PCB allongé, équipé d’un STM32 et installé sur le plateau. Sa longueur correspond au nombre de cases qu’il occupe.

| Format envisagé | Emprise sur la grille |
| --- | --- |
| Petit bateau | 1 × 2 cases |
| Bateau moyen | 1 × 3 cases |
| Grand bateau | 1 × 4 cases |

Le PCB bateau doit permettre son identification et l’état de ses cases.Il porte les pcb capa.

### 3.3. PCB capa

Le troisième type de PCB est un module monté sur le PCB bateau. Il conserve le principe d’un élément facilement remplaçable.

On a donc l’empilement suivant : **PCB plateau → PCB bateau → PCB capa**.

## 4. Communication et détection des bateaux

Une liaison I²C entre chaque plateau et les bateaux qu’il accueille. Le plateau joue le rôle de maître et les bateaux celui d’esclaves.

Avant une partie, le système doit établir une représentation de la flotte comportant l’identité de chaque bateau, sa longueur, son orientation et les coordonnées des cases occupées.

**La présence d’un bateau sur le bus et sa position sur la grille constituent deux informations distinctes.** La méthode de localisation reste **(à développer URGEMMENT)**.

Les points suivants restent déterminé :

- attribution des adresses et distinction entre plusieurs bateaux identiques ;
- méthode de localisation des bateaux sur les 64 cases ;
- connectique et orientation des raccordements ;
- contenu des messages et gestion des erreurs de communication ;
- coordination des deux plateaux et autorité sur l’état de la partie.

## 5. Interface utilisateur et affichage

### 5.1. Commandes

Chaque joueur doit pouvoir sélectionner une ligne et une colonne, puis confirmer le tir avec le bouton FEU. La réalisation exacte des deux axes analogiques est **(à développer)**.

La case sélectionnée doit être visible avant validation. Son mode de représentation, distinct du résultat des tirs précédents.

### 5.2. Afficheur LED neopixel

Un afficheur 8 × 8 de type neopixel au centre du jeu, entre les grilles. Il fait office d'obstacle visuelle afin de masquer la flotte adverse, ainsi qu'a représentee les résultats des tirs sur la grille adverse.

| Couleur | Signification |
| --- | --- |
| Bleu | Case non encore ciblée ; état par défaut |
| Vert | Tir raté : aucun bateau sur cette case |
| Rouge | Tir touché : case appartenant à un bateau encore à flot |
| Noir, LED éteinte | Case appartenant à un bateau coulé |

## 6. Déroulement d’une partie

1. **Initialisation** : le système initialise les commandes, les communications et l’affichage.
2. **Placement** : chaque joueur installe sa flotte sur sa grille.
3. **Fermeture** : les joueurs abaissent les vitre plexi. La détection/vérification.
4. **Vérification** : après détection de la fermeture des deux capots, le système identifie les bateaux et vérifie leur placement.
6. **Sélection et tir** : le joueur actif choisit une case adverse et appuie sur FEU.
7. **Résolution** : le système mémorise le résultat, actualise l’affichage et, en cas de touche, fait éclater la capacité.
8. **Tour suivant** : Apres chaque tir le joueur passe son tour.
9. **Fin** : la partie se termine lorsque tous les bateaux d’un joueur sont coulés.

## 7. Système mise à feu

**(à définir)**

## 8. Système localisation

<img width="374" height="383" alt="SmartSelect_20260921_102907_Samsung Notes" src="https://github.com/user-attachments/assets/851674c3-1733-448f-876e-e9550717cb7c" />

**(à définir)**

## 9. Alimentation

**(à développer)**

## 10. Références Matériel

### 10.1 Capacité

| Élément | Spécifications |
| --- | --- |
|Référence| **(àdéfinir)**|
|Tension nominal explosion| **(à définir)**|
|Pic de courant explosion| **(à définir)**|

## 11. Références Logiciel

### 11.1 Modèle IA

| Élément | Spécifications |
| --- | --- |
|Chatgpt-6 (astra) :| mise en page git|

### 11.2 Logiciel conception

**(à définir)**
