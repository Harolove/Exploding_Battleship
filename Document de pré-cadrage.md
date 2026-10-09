# Document de pré-cadrage

## Projet Battleship

**Équipe :** Elouan LUCET et Arnaud BERNHARD

---

## 1. Objectif

### Spécifique (S)

Battleship est une adaptation électronique du jeu de société de bataille navale. Les joueurs disposent au départ de la même configuration de jeu : grille, nombre et types de bateaux.

Chaque joueur possède une grille physique sur PCB de **8 × 8 cases**, sur laquelle il place ses bateaux avant le début de la partie. Il dispose également d'une grille de **8 × 8 LED NeoPixel** pour visualiser l'état de la carte du joueur adverse pendant la partie.

À tour de rôle, les joueurs sélectionnent une case de la grille adverse à l'aide de deux commandes, l'une pour la ligne et l'autre pour la colonne, puis valident leur choix avec le bouton **FEU**. Le système indique alors si le tir est raté, touche un bateau ou coule un bateau.

Dans le concept initial, un tir réussi provoque l'éclatement du condensateur associé à la case touchée.

### Mesurable (M)

Les critères de mesure restent à définir.

### Atteignable / réaliste (A/R)

L'équipe compte six personnes. Le périmètre du prototype devra rester réalisable avec les moyens disponibles.

### Temporel (T)

Un prototype fonctionnel est attendu pour avril. Une présentation lors des journées portes ouvertes de février 2027 pourrait avancer cette échéance.

---

## 2. Lots PBS

| Lot | Tâches à préciser |
|---|---|
| Matériel | Conception des grilles, des bateaux, des PCB, des commandes et de l'affichage LED. |
| Logiciel | Détection de la position des bateaux, gestion des tirs et des états de jeu, communication I²C. |

### Compétences OBS

Six personnes participent au projet. La répartition des compétences et des responsabilités reste à définir.

### Tâches WBS et calendrier prévisionnel

| Période | Travail prévu ou état du cadrage |
|---|---|
| Septembre | Mesures électriques liées à l'éclatement des condensateurs : tension, pic de courant, dégagement de gaz, etc. Définition des spécifications et création du présent document de pré-cadrage. |
| Octobre à mars | Planification détaillée à établir : les informations disponibles ne permettent pas encore de répartir les tâches mois par mois. |
| Février 2027 | Éventuelle démonstration fonctionnelle aux journées portes ouvertes, sous réserve de confirmation par l'encadrant. |
| Avril | Livraison d'un prototype fonctionnel. |

---

## 3. Contraintes

| Contrainte | Exigence |
|---|---|
| Sécurité | Assurer la sécurité physique des joueurs : les protéger des risques électriques, des projections, des électrolytes et des gaz liés aux condensateurs. |
| Échéance | Livraison connue pour avril ; disponibilité possible dès février 2027 pour les journées portes ouvertes. |
| Système de mise à feu | Définir le principe physique et électrique permettant d'actionner les cases concernées, sous réserve de sa faisabilité et de sa sécurité. |
| Matériel et technique | Trois types de PCB différents et deux PCB par bateau sont envisagés ; le nombre de commandes à réaliser est élevé. |
| Communication | Utiliser le bus I²C entre le plateau et les bateaux. |
| Localisation | Détecter la position des bateaux, qu'ils soient placés à l'horizontale ou à la verticale. |

---

## 4. Cadrage initial du projet

| Point à définir | Situation actuelle |
|---|---|
| Contexte et intérêt | Créer une bataille navale physique et interactive, tout en mettant en pratique la conception électronique et la programmation embarquée. |
| Périmètre et livrable | Réaliser un prototype à deux joueurs capable de détecter les bateaux, de gérer les tirs et d'afficher les résultats par LED. |
| Parties prenantes | Les six membres de l'équipe, le professeur encadrant et les futurs joueurs. Le chef de projet et les rôles de chacun restent à définir. |
| Coûts et bénéfices | Le budget des PCB et des composants reste à estimer. Le prototype pourrait être présenté lors des journées portes ouvertes. |
| Approche et état de l'art | Comparer les solutions de détection et de communication existantes, puis valider le fonctionnement sur un prototype simple avant de concevoir les PCB définitifs. |
| Premières étapes | Répartir les rôles, faire valider le périmètre par l'encadrant, estimer le budget et établir un planning détaillé. |

---

## 5. Risques

Les risques sont évalués selon leur probabilité d'apparition (**P**) et leur impact (**I**), sur une échelle de 1 à 5.

La criticité est calculée par :

**C = P × I**

Les évaluations suivantes sont initiales et seront actualisées au cours du projet.

### Identification et réduction des risques

| ID | Risque | P | I | C | Action prévue |
|---|---|---:|---:|---:|---|
| R1 | Exposition du joueur aux risques électriques, aux projections ou aux gaz liés aux condensateurs. | 2 | 5 | 10 | Faire valider la sécurité par l'encadrant. Utiliser une simulation par LED tant que cette validation n'est pas acquise. |
| R2 | Éclatement des condensateurs interdit ou techniquement irréalisable. | 3 | 4 | 12 | Confirmer rapidement la faisabilité et prévoir une version du jeu avec retour visuel par LED. |
| R3 | Communication I²C défaillante entre le plateau et les bateaux. | 3 | 4 | 12 | Valider les échanges sur un montage simple avant l'intégration sur les PCB. |
| R4 | Erreur de détection de la position ou de l'orientation des bateaux. | 3 | 3 | 9 | Tester différents placements, horizontaux et verticaux, ainsi que les cases en bordure de grille. |
| R5 | Erreur de conception des PCB nécessitant une nouvelle fabrication. | 3 | 4 | 12 | Faire relire les schémas et tester les fonctions principales avant de commander les PCB. |
| R6 | Erreur dans la gestion des tirs, des commandes ou de l'affichage LED. | 2 | 3 | 6 | Tester les tirs ratés, touchés, répétés et les changements de tour sur un prototype. |
| R7 | Retard de livraison ou d'intégration empêchant de tenir l'échéance. | 4 | 4 | 16 | Commander tôt, répartir les tâches entre les six membres et prévoir du temps pour l'intégration et les essais. |
| R8 | Dépassement du budget lié au nombre de PCB et de composants. | 3 | 3 | 9 | Établir une nomenclature chiffrée avant les commandes et simplifier le prototype si nécessaire. |

### Matrice probabilité × impact

Les identifiants R1 à R8 permettent de situer les risques dans la matrice. Le nombre indiqué dans chaque case correspond à la criticité.

| Probabilité \ Impact | 1 — Insignifiant | 2 — Mineur | 3 — Significatif | 4 — Majeur | 5 — Sévère |
|---|---:|---:|---:|---:|---:|
| **5 — Presque certain** | 🟨 5 | 🟧 10 | 🟥 15 | 🟥 20 | 🟥 25 |
| **4 — Probable** | 🟨 4 | 🟨 8 | 🟧 12 | 🟥 **16 — R7** | 🟥 20 |
| **3 — Modéré** | 🟩 3 | 🟨 6 | 🟨 **9 — R4, R8** | 🟧 **12 — R2, R3, R5** | 🟥 15 |
| **2 — Improbable** | 🟩 2 | 🟩 4 | 🟨 **6 — R6** | 🟨 8 | 🟧 **10 — R1** |
| **1 — Rare** | 🟩 1 | 🟩 2 | 🟩 3 | 🟨 4 | 🟨 5 |

**Lecture des couleurs :**

- 🟩 Vert : surveillance simple
- 🟨 Jaune : attention
- 🟧 Orange : réduction du risque à prévoir
- 🟥 Rouge : action prioritaire

### Priorités

Le risque de retard (**R7**) est le plus critique : une démonstration en février 2027 réduirait le temps disponible par rapport à la livraison prévue en avril.

Les risques de faisabilité (**R2**), de communication (**R3**) et de conception des PCB (**R5**) doivent être traités tôt pour éviter de bloquer l'intégration.

Le risque de sécurité (**R1**) constitue un point bloquant pour l'utilisation des condensateurs, indépendamment de son score. Si la sécurité ou la faisabilité de cet effet n'est pas validée, le prototype utilisera un retour par LED.
