# Bataille navale : plateau 8 × 8 sur NUCLEO-L476RG

Test avec deux encodeurs **Digilent Pmod ENC** et le ruban
**ILPX-K517-RGB1-2M0-SK15V10-01**, à LED
**SK6812 RGB, 24 bits GRB**. Une case correspond à une LED : le programme
commande les **64 premières LED** depuis **PA8 / D7**, avec **TIM1 canal 1,
PWM et DMA**. Les LED suivantes ne sont pas pilotées par ce test.
Les encodeurs déplacent le curseur. Leurs sorties BTN et SWT restent libres ;
aucun bouton n'est nécessaire pour ce test.

## Réglages dans `src/main.c`

| Réglage | Valeur initiale | Utilisation |
|---|---|---|
| `cursor_x`, `cursor_y` | `0`, `2` | Position initiale conservée ; ensuite mise à jour depuis les encodeurs |
| `ENCODER_X_DIRECTION`, `ENCODER_Y_DIRECTION` | `1`, `1` | Mettre `-1` pour inverser un axe |
| `ENCODER_X_TRANSITIONS_PER_DETENT`, `ENCODER_Y_TRANSITIONS_PER_DETENT` | `4U`, `4U` | Nombre de transitions A/B par case, réglable séparément : 1, 2 ou 4 |
| `ENCODER_DEBOUNCE_MS` | `2U` | Durée de stabilité exigée pour accepter un nouvel état A/B |
| `test_shots[]` | `{1, 1}`, `{4, 3}` | Cases visées une fois au démarrage |
| `test_ships[]` | `{4, 3}`, `{5, 3}`, `{6, 3}`, `{2, 5}`, `{2, 6}` | Cases occupées par des bateaux cachés |
| `BOARD_SERPENTINE` | `1U` | `1U` = serpentin ; `0U` = lignes successives |
| `SK6812_BRIGHTNESS` | `24U` | Luminosité de 0 à 255, environ 9,4 % par composante |
| `SK6812_REFRESH_MS` | `1000U` | Rappel périodique ; une nouvelle position déclenche un envoi immédiat |

Après une modification des réglages, recompiler et téléverser. Les encodeurs
sont relatifs : leur position mécanique au démarrage ne détermine pas une
coordonnée absolue. Le programme part de `(0, 2)`, puis suit les crans.
Les coordonnées restent entre **0 et 7**, sans bouclage. Les crans tournés
contre un bord sont consommés : un cran inverse repart immédiatement.
Il n'y a ni déplacement automatique ni tir automatique répété.

## Branchement des Pmod ENC et disponibilité des broches

| Fonction | Pmod ENC | NUCLEO-L476RG |
|---|---|---|
| Axe X, A | Broche 1 | D5 / PB4, CN9 broche 6 |
| Axe X, B | Broche 2 | D4 / PB5, CN9 broche 5 |
| Axe Y, A | Broche 1 | D3 / PB3, CN9 broche 4 |
| Axe Y, B | Broche 2 | D2 / PA10, CN9 broche 3 |
| Les deux modules, GND | Broche 5 | GND commun |
| Les deux modules, VCC | Broche 6 | **3,3 V** |
| BTN et SWT | Broches 3 et 4 | Non connectées |

Ces quatre entrées ne sont pas utilisées par le pilote SK6812, qui conserve
**PA8 / D7, TIM1_CH1 et DMA1 canal 2, requête 7**. PA5 reste la LED LD2.
PB3 et PB4 ont des fonctions alternatives JTAG/SWO : `Encoders_Init()` les
configure explicitement en entrées GPIO. La configuration ST-LINK du projet
utilise **SWD sur PA13/PA14**, qui reste disponible. Ne pas activer de trace
SWO sur PB3 ni de débogage JTAG complet avec ce câblage.

Le manuel Pmod indique des résistances de rappel au niveau haut sur A/B.
Les entrées STM32 utilisent également des pull-up pour rester définies
si les modules sont absents. Aucun signal de commande n'est envoyé aux Pmod.

## Lecture, sens et réglage des crans

La lecture s'effectue toutes les **1 ms dans SysTick**, y compris pendant
les attentes HAL et les transferts du ruban. Ce choix conserve les broches
proposées : l'axe Y ne fournit pas une paire CH1/CH2 d'un même timer pour un
décodage matériel en mode encodeur. Aucun nouveau timer ni canal DMA n'est utilisé.

Le décodeur de `src/encoder.c` accepte un nouvel état A/B après **2 ms de
stabilité**. Il additionne les transitions de quadrature valides dans les
deux sens ; les allers-retours dus aux rebonds s'annulent. Un changement
simultané des deux bits est rejeté, le mouvement partiel est abandonné et
le décodeur attend un retour à l'état A/B relevé à l'initialisation pour se
resynchroniser. Aucun délai bloquant ni affichage n'est exécuté dans SysTick.

Par défaut, **A montant avant B** est le sens positif, suivant le diagramme
« Rotate Right » du manuel : **X augmente vers la droite**, **Y augmente vers
le bas**. L'autre sens diminue la coordonnée. Électriquement, le cycle positif
est `AB : 00 → 10 → 11 → 01 → 00`. Si le sens mécanique observé est inversé,
mettre `ENCODER_X_DIRECTION` ou `ENCODER_Y_DIRECTION` à **`-1`**.

Le réglage initial de **4 transitions par case** correspond à un cycle A/B
complet. La correspondance exacte avec un cran physique reste à confirmer
sur les modules : si deux crans physiques sont nécessaires pour une case,
mettre le réglage de cet axe à **`2U`**. Si chaque cran produit plusieurs
cases, augmenter le nombre de transitions par case. `1U` est aussi accepté,
mais filtre moins les allers-retours qu'un cycle complet.

Le filtrage privilégie la stabilité pour une commande manuelle ; une rotation
très rapide peut perdre des crans si les états ne durent pas assez longtemps.
`ENCODER_DEBOUNCE_MS` peut être réduit à `1U` après vérification des rebonds.

Les positions internes sont bornées à chaque cran dans SysTick. Une seule
écriture 32 bits publie les deux coordonnées ; la boucle principale les lit
ensemble, met à jour `cursor_x/y` puis reconstruit l'image si la position a
changé. Elle n'écrit jamais le tampon PWM pendant sa transmission.
TIM1/DMA produit les impulsions indépendamment du CPU ; l'interruption DMA
a la priorité 1, supérieure à SysTick (priorité 2). Aucun masquage global des
interruptions n'est ajouté. Les images intermédiaires peuvent être regroupées
si l'on tourne pendant un transfert : la position suivante est la plus récente.

## Coordonnées et câblage

L'origine **(0, 0) est en haut à gauche**, X augmente vers la droite et Y vers
le bas. **`Board_XYToLed(x, y)`** retourne un **index de 0 à 63**. L'index 0
désigne la **LED physique n° 1**, la première depuis `DIN`, dans le sens des
flèches du ruban. Une coordonnée invalide renvoie `UINT32_MAX`.

La convention initiale est le **serpentin** :

```text
                       X →
        0   1   2   3   4   5   6   7
Y = 0   1   2   3   4   5   6   7   8    →
Y = 1  16  15  14  13  12  11  10   9    ←
Y = 2  17  18  19  20  21  22  23  24    →
Y = 3  32  31  30  29  28  27  26  25    ←
Y = 4  33  34  35  36  37  38  39  40    →
Y = 5  48  47  46  45  44  43  42  41    ←
Y = 6  49  50  51  52  53  54  55  56    →
Y = 7  64  63  62  61  60  59  58  57    ←
```

Les nombres ci-dessus sont les numéros physiques **à partir de 1**.
La formule de l'index est `y * 8 + x` sur les lignes paires, et
`y * 8 + (7 - x)` sur les lignes impaires.
Pour des lignes toutes câblées de gauche à droite, mettre `BOARD_SERPENTINE`
à **`0U`** : l'index devient toujours `y * 8 + x`.

Le logiciel suppose un assemblage physique de huit lignes de huit LED.
Un ruban laissé droit affiche les mêmes cases dans l'ordre de la chaîne.

## Affichage au démarrage et tirs

`Board_Init()` initialise toutes les cases comme mer, puis applique les tirs
de `test_shots[]` avec `Board_Fire()`. Le premier affichage est donc :

| Case `(x, y)` | LED physique en serpentin | Couleur | Octets G, R, B transmis |
|---|---:|---|---|
| `(0, 2)` | 17 | Blanc : curseur au-dessus de la mer | `18 18 18` |
| `(1, 1)` | 15 | Vert : tir dans l'eau | `18 00 00` |
| `(4, 3)` | 28 | Rouge : bateau touché | `00 18 00` |
| Toutes les autres cases | Les 61 autres LED | Bleu : case non visée | `00 00 18` |

Ces octets sont en hexadécimal : `18` vaut **24 sur 255**. Le blanc allume les
trois composantes RGB, sans octet supplémentaire. Les bateaux non visés restent
bleus. L'état des tirs est mémorisé dans **`board[y][x]`** ; le blanc est
appliqué uniquement dans `SK6812_FillBuffer()`, sans modifier cet état.
Chaque image est reconstruite : l'ancienne case retrouve donc sa couleur
lorsque le curseur se déplace.

Tourner les encodeurs pour placer le curseur en `(1, 1)` puis ailleurs : la
LED 15 doit passer du vert au blanc puis redevenir verte. Faire de même en
`(4, 3)` pour vérifier le retour au rouge. Les variables `cursor_x/y` permettent
d'observer la position dans le débogueur ; la boucle les actualise depuis les
encodeurs, donc une modification directe de ces variables n'est pas conservée.

Pour le futur bouton, appeler **`Board_Fire(cursor_x, cursor_y)`** dans la
boucle principale, à l'emplacement commenté, après lecture et anti-rebond,
puis mettre `displayed_cursor = UINT32_MAX` pour demander un nouvel affichage.
La fonction compare la case à `test_ships[]` et mémorise `CELL_HIT` (rouge)
ou `CELL_MISS` (vert). Elle retourne `false` hors plateau et `true` pour un
tir valide. Tirer à nouveau sur une case conserve son résultat. Une case visée
sous le curseur reste blanche jusqu'à son déplacement.

## Signal et horloge

La commande matérielle existante est réutilisée :

- **PA8 / D7**, connecteur **CN9 broche 8**, en **AF1 / TIM1_CH1** pendant la trame.
- **DMA1 canal 2, requête 7**, mode normal, mémoire et périphérique en demi-mots.
  Un demi-mot par bit ; le pointeur `uint32_t *` de l'API HAL ne change pas
  cet alignement DMA.
- `BoardClock_Init()` configure **HSI16 → PLL /1 ×10 /2 → SYSCLK 80 MHz**.
  AHB et APB2 divisés par 1 : **TIM1 reçoit 80 MHz**. La valeur `F_CPU` seule
  ne configure pas l'horloge matérielle.
- `PSC = 0`, `ARR = 99` : **100 coups de 12,5 ns = 1,25 µs par bit**,
  soit **800 kbit/s**.

| Bit | CCR1 | Durée haute | Durée basse |
|---|---:|---:|---:|
| 0 | 24 | 300 ns | 950 ns |
| 1 | 48 | 600 ns | 650 ns |

Ces valeurs calculées sont dans les tolérances de la fiche SK6812 RGB :
T0H = 0,30 ± 0,15 µs, T0L = 0,90 ± 0,15 µs,
T1H = 0,60 ± 0,15 µs et T1L = 0,60 ± 0,15 µs.

La trame transmet **64 × 24 = 1 536 bits utiles**, soit **1,920 ms**.
Pour chaque LED : **G7…G0, R7…R0, B7…B0**, poids fort en premier.
Les **242 valeurs nulles** de fin assurent au moins **300 µs bas** avant
le callback DMA, marge de préchargement incluse : plus que les 80 µs de
reset de la fiche SK6812.

Le préchargement de `CCR1` évite de déformer les impulsions ; `CCDS = 1`
déclenche les requêtes CC1 sur les updates, même avec `CCR1 = 0`.
`RCR = 0` assure un update par période et la HAL gère `BDTR.MOE` pour TIM1.
Le compare actif est chargé à zéro avant le démarrage : les premières périodes
restent basses, puis le premier bit est transmis sans être sauté.
PA8 reste basse au moins 2 ms avant la trame, revient en GPIO basse avant
l'arrêt du PWM, puis reste basse au moins 1 ms supplémentaire.
Le délai maximal de 10 ms couvre le transfert d'environ 2,223 ms.

`SK6812_Show()` vérifie à chaque transfert la fréquence TIM1 issue de la
configuration RCC, `PSC`, `ARR`, `RCR` et les principaux paramètres DMA.
Ces contrôles logiciels ne constituent pas une mesure électrique.

## Compiler et téléverser

`platformio.ini` a été analysé et conservé : **NUCLEO-L476RG**, **STM32Cube**,
plateforme **ST STM32 20.0.0**, téléversement par **ST-LINK**.

Dans VS Code : **PlatformIO → Project Tasks → nucleo_l476rg → General → Build**,
puis **Upload**. Brancher la Nucleo au port USB **ST-LINK** avec un câble USB
de données. Aucun port COM n'est nécessaire pour ce téléversement.

```text
pio run -e nucleo_l476rg
pio run -e nucleo_l476rg -t upload
```

Si `pio` n'est pas reconnu dans PowerShell, sur ce PC :

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e nucleo_l476rg
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e nucleo_l476rg -t upload
```

Compilation de la version à encodeurs réussie le **17 novembre 2026**, sans
erreur ni avertissement affiché : **8 616 octets de Flash** et **3 868 octets de RAM**.
Fichiers produits : `.pio/build/nucleo_l476rg/firmware.elf` et `firmware.bin`.

**Aucun téléversement ni contrôle de l'affichage sur le matériel n'a été
effectué pour cette version.** Les durées sont calculées depuis la
configuration ; les impulsions sur DIN restent à mesurer à l'oscilloscope
ou à l'analyseur logique.

**LD2 fixe** : les transferts DMA se terminent sans erreur détectée.
**LD2 clignotante** : erreur d'initialisation ou de transfert.
LD2 ne confirme pas la réception du signal ni les couleurs du ruban.

## Tests logiciels du décodeur

`tests/test_encoder.c` est compilé avec la toolchain ARM et exécuté sur un
Cortex-M4 émulé par Unicorn, sans HAL ni carte. Les tests ont réussi : rotations
dans les deux sens depuis les quatre phases initiales, résolutions 1/2/4,
rebonds courts et longs, inversion avant la fin d'un cran, transitions
invalides et resynchronisation, débordement du tick, indépendance des axes,
saturation et retour immédiat depuis les bords.

Pour les reproduire avec Python et la toolchain PlatformIO installés :

```text
python -m pip install --target .pio/encoder-test-tools unicorn==2.1.4
python tests/run_encoder_tests.py
```

Unicorn sert uniquement aux tests sur PC et n'est pas une dépendance du
firmware. Ces tests ne valident pas les rebonds réels, le nombre de transitions
par cran physique, le sens mécanique des modules ni le signal sur DIN.

## Branchement DATA et alimentation

Le signal est déjà configuré sur **PA8 / D7**. Conserver le montage décrit
dans le projet avec un **74AHCT125 alimenté en 5 V** pour convertir le signal
3,3 V de la Nucleo vers le niveau logique du ruban alimenté en 5 V.

| Point de départ | Connexion |
|---|---|
| Nucleo D7 / PA8 | Entrée `1A` du 74AHCT125 (broche 2 du boîtier 14 broches) |
| Sortie `1Y` (broche 3) | Résistance 330 Ω en série, puis **DIN** du ruban |
| `/1OE` (broche 1) | GND : activation du canal |
| `VCC` (broche 14) et `+5V` du ruban | Alimentation +5 V externe adaptée au ruban |
| `GND` (broche 7) | Masse commune avec ruban, alimentation et Nucleo |

Placer un condensateur de 100 nF entre VCC et GND près du 74AHCT125.
Relier les entrées A des canaux inutilisés à GND et leurs entrées /OE au +5 V.
Alimenter le ruban depuis l'alimentation externe, pas depuis une sortie GPIO.
Modifier le câblage alimentations coupées et suivre les flèches vers les LED.

Si seule la première LED répond, vérifier `DIN`/`DOUT`, le 5 V et les masses,
puis mesurer DATA. Sur DIN de la LED 1 : **1 536 impulsions pendant 1,920 ms**,
puis le reset bas. Au démarrage, les premiers octets sont `00 00 18` (mer bleue) ;
la LED 17 reçoit `18 18 18` (curseur blanc). Sur DOUT de
la LED 1 : les **1 512 bits** restants pour les LED 2 à 64.
Un multimètre ne permet pas de valider cette trame.

## Documents de référence

- [Manuel Digilent Pmod ENC : broches, quadrature, pull-up et rebonds](https://digilent.com/reference/_media/reference/pmod/pmodenc/pmodenc_rm.pdf).
- [Ruban ILS : SK6812 RGB, 24 bits, 5 V](https://i-led.co.uk/PDFs/Pixels/ILPX-K517-RGB1-2M0-SK15V10-xx%20V1.0.pdf).
- [Fiche SK6812 RGB : temporisations p. 5, structure GRB p. 6](https://www.mouser.com/datasheet/2/737/SK6812_LED_datasheet_-953546.pdf).
- [STM32L476, DS10198 : fonctions alternatives de PA8](https://www.st.com/resource/en/datasheet/stm32l476vc.pdf).
- [STM32L476, RM0351 : horloges, TIM1, préchargement, CCDS et DMA](https://www.st.com/resource/en/reference_manual/dm00083560.pdf).
- [NUCLEO-L476RG, UM1724, tableau 23 : PA8 = D7 / CN9 broche 8](https://www.st.com/resource/en/user_manual/dm00105823-stm32-nucleo-32-boards-stmicroelectronics.pdf).
- [74AHC125 / 74AHCT125, Nexperia : brochage et activation /OE](https://assets.nexperia.com/documents/data-sheet/74AHC_AHCT125.pdf).
