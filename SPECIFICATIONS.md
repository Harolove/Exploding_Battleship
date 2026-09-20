# Battleship — Spécifications du projet

> Projet de deuxième année du cycle ingénieur — version préliminaire.
> Les éléments marqués **(à développer)** restent à définir ou à valider.
> Ce document propose une variante à effets non destructifs. Elle remplace l’effet initialement envisagé d’éclatement de condensateurs par une signalisation lumineuse ou sonore. Cette modification reste une proposition à valider par l’équipe.

## 1. Présentation

Battleship est une adaptation électronique du jeu de société de bataille navale, destinée à deux joueurs. Chaque joueur dispose d’une grille physique de **8 × 8 cases**, soit **64 cases**, sur laquelle il place ses bateaux avant le début de la partie.

Les joueurs sélectionnent une case de la grille adverse à l’aide de commandes analogiques, puis valident leur choix avec un bouton **FEU**. Le système détermine automatiquement si le tir est raté, touché ou s’il coule un bateau. Une matrice de LED affiche les résultats des tirs.

Le projet associe conception de circuits imprimés, programmation de microcontrôleurs STM32, communication entre cartes et réalisation d’une interface utilisateur.

## 2. Périmètre et choix envisagés

| Élément | Spécification préliminaire |
| --- | --- |
| Nombre de joueurs | 2 |
| Grille par joueur | 8 × 8 cases |
| Dimensions visées par grille | Environ 30 × 30 cm ; dimensions finales **(à développer)** |
| Types de PCB | Plateau, bateau, module d’effet amovible |
| Microcontrôleurs | Famille STM32 ; références **(à développer)** |
| Répartition envisagée | Un STM32 par plateau et un STM32 par bateau ; coordination des deux plateaux **(à développer)** |
| Communication plateau–bateaux | I²C envisagé, plateau maître et bateaux esclaves |
| Sélection d’une case | Deux axes analogiques, réalisés par potentiomètres ou joystick ; choix **(à développer)** |
| Validation | Bouton FEU |
| Affichage des tirs | Matrice de LED RGB adressables de type NeoPixel, 8 × 8 |
| Séparation physique | Écran central et capots transparents en plexiglas envisagés |
| Alimentation de la variante proposée | Alimentation externe isolée, délivrant la basse tension nécessaire ; caractéristiques **(à développer)** |

## 3. Architecture matérielle

### 3.1. PCB plateau

Le PCB plateau constitue la grille de jeu et la carte principale de chaque côté. Il doit accueillir les bateaux et assurer :

- la lecture des commandes du joueur ;
- la gestion de la grille et des règles du jeu ;
- l’identification des bateaux et de leurs cases occupées ;
- les échanges avec les PCB bateau ;
- la gestion de l’affichage et des résultats des tirs ;
- la lecture de l’état du capot ;
- les échanges nécessaires avec le plateau adverse.

Les connecteurs, les fixations, le nombre de couches et les dimensions exactes sont **(à développer)**.

### 3.2. PCB bateau

Chaque bateau est constitué d’un PCB allongé, équipé d’un STM32 et installé sur le plateau. Sa longueur correspond au nombre de cases qu’il occupe.

| Format envisagé | Emprise sur la grille |
| --- | --- |
| Petit bateau | 1 × 2 cases |
| Bateau moyen | 1 × 3 cases |
| Autres formats | **(à développer)** |

Le PCB bateau doit permettre son identification et la gestion de l’état de chacune de ses cases. Dans la variante proposée, il porte des modules d’effet non destructifs amovibles.

La composition de la flotte, le nombre de bateaux de chaque taille, les orientations autorisées et les contraintes de placement sont **(à développer)**.

### 3.3. PCB de module d’effet

Le troisième type de PCB est un module monté sur le PCB bateau. Il conserve le principe d’un élément facilement remplaçable, distinct de l’électronique principale du bateau.

Dans cette variante, le module signale une touche par un effet lumineux ou sonore. La technologie retenue, sa référence, sa tension nominale et sa consommation sont **(à développer)**.

L’empilement mécanique est le suivant : **PCB plateau → PCB bateau → PCB de module d’effet**.

## 4. Communication et détection des bateaux

Une liaison I²C est envisagée entre chaque plateau et les bateaux qu’il accueille. Le plateau joue le rôle de maître et les bateaux celui d’esclaves.

Avant une partie, le système doit établir une représentation de la flotte comportant l’identité de chaque bateau, sa longueur, son orientation et les coordonnées des cases occupées.

**La présence d’un bateau sur le bus et sa position sur la grille constituent deux informations distinctes.** La méthode de localisation reste **(à développer)** ; la proposition d’interroger les cases ne définit pas encore sa réalisation matérielle.

Les points suivants restent à préciser :

- attribution des adresses et distinction entre plusieurs bateaux identiques ;
- méthode de localisation des bateaux sur les 64 cases ;
- connectique et orientation des raccordements ;
- contenu des messages et gestion des erreurs de communication ;
- coordination des deux plateaux et autorité sur l’état de la partie.

## 5. Interface utilisateur et affichage

### 5.1. Commandes

Chaque joueur doit pouvoir sélectionner une ligne et une colonne, puis confirmer le tir avec le bouton FEU. La réalisation exacte des deux axes analogiques est **(à développer)**.

La case sélectionnée doit être visible avant validation. Son mode de représentation, distinct du résultat des tirs précédents, est **(à développer)**.

### 5.2. Matrice de LED

Un afficheur 8 × 8 de type NeoPixel est envisagé au centre du dispositif, entre les grilles. Il contribue à masquer les flottes et représente les résultats des tirs sur la grille adverse.

| Couleur | Signification |
| --- | --- |
| Bleu | Case non encore ciblée ; état par défaut |
| Vert | Tir raté : aucun bateau sur cette case |
| Rouge | Tir touché : case appartenant à un bateau encore à flot |
| Noir, LED éteinte | Case appartenant à un bateau coulé |

Dans la variante proposée, un bateau est coulé lorsque toutes ses cases ont été touchées. Toutes les cases de ce bateau passent alors au noir.

Le nombre de matrices et leur disposition sont **(à développer)** : chaque joueur doit disposer d’une vue lisible de ses propres tirs, sans révéler la flotte adverse. Les dimensions de la séparation visuelle sont également **(à développer)**.

## 6. Déroulement d’une partie

1. **Initialisation** : le système initialise les commandes, les communications et l’affichage.
2. **Placement** : chaque joueur installe sa flotte sur sa grille.
3. **Fermeture** : les joueurs abaissent les capots transparents. La détection par boutons poussoirs est envisagée ; implantation **(à développer)**.
4. **Vérification** : après détection de la fermeture des deux capots, le système identifie les bateaux et vérifie leur placement. La logique nécessaire à cette détection doit déjà être alimentée.
5. **Démarrage** : lorsque les deux configurations sont valides, la partie commence. Le choix du premier joueur est **(à développer)**.
6. **Sélection et tir** : le joueur actif choisit une case adverse et appuie sur FEU.
7. **Résolution** : le système mémorise le résultat, actualise l’affichage et, en cas de touche, produit l’effet non destructif prévu.
8. **Tour suivant** : la règle d’alternance et l’éventuelle possibilité de rejouer après une touche sont **(à développer)**.
9. **Fin** : la partie se termine lorsque tous les bateaux d’un joueur sont coulés. L’annonce du gagnant et la procédure de nouvelle partie sont **(à développer)**.

## 7. Comportements à préciser

Les exigences suivantes sont proposées pour compléter le fonctionnement :

- Un tir sur une case déjà ciblée ne doit pas déclencher de nouvel effet ; son incidence sur le tour est **(à développer)**.
- Les commandes du joueur inactif ne doivent pas valider de tir.
- L’ouverture d’un capot pendant la partie doit suspendre le jeu et désactiver les effets ; les conditions de reprise sont **(à développer)**.
- Une flotte incomplète, un placement invalide ou une erreur de communication doit empêcher le démarrage ou provoquer une pause signalée.
- La perte de communication ne doit pas être interprétée comme une case vide.
- Le comportement après une coupure d’alimentation est **(à développer)**.

## 8. Alimentation et intégration mécanique

Pour cette variante, l’alimentation secteur éventuelle est assurée par un bloc externe isolé. Les PCB de jeu reçoivent uniquement les tensions basses nécessaires à leur fonctionnement.

Les tensions des STM32, des matrices et des modules d’effet, le bilan de consommation, le courant maximal et les protections électriques sont **(à développer)**. Aucun budget de puissance n’est considéré comme validé à ce stade.

L’objectif d’encombrement est d’environ **30 × 30 cm par grille**. Les dimensions du dispositif complet, des cases, des connecteurs, des bateaux et des capots sont **(à développer)**. L’espace central et les commandes doivent être pris en compte dans l’encombrement total.

Les capots décrits ici sont des éléments d’intégration de la variante non destructive ; ce document ne les qualifie pas comme protections contre l’éclatement de composants.

## 9. Validation fonctionnelle prévue

| Vérification | Résultat attendu |
| --- | --- |
| Sélection | Chacune des 64 cases peut être sélectionnée sans ambiguïté |
| Placement | Les bateaux, leurs orientations et leurs cases sont correctement reconnus |
| Tir raté | La case correspondante passe au vert |
| Tir touché | La case passe au rouge et son état est mémorisé |
| Bateau coulé | Toutes les cases du bateau passent au noir |
| Tir répété | Aucun nouvel effet n’est déclenché |
| Capot ouvert | Le démarrage est bloqué ou la partie est suspendue |
| Communication interrompue | Un défaut est signalé et aucun résultat de tir n’est inventé |
| Fin de partie | Le gagnant est identifié lorsque la flotte adverse est entièrement coulée |

## 10. Livrables envisagés

- Schémas électroniques et fichiers de conception des trois types de PCB.
- Nomenclature des composants.
- Firmware STM32 des plateaux et des bateaux.
- Documentation du protocole de communication et des règles du jeu.
- Plans mécaniques et instructions d’assemblage de la variante retenue.
- Procédure de mise en service et compte rendu des essais fonctionnels.

Les outils de développement, l’organisation du dépôt, le calendrier et la répartition du travail sont **(à développer)**.
