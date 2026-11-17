#include "stm32l4xx_hal.h"
#include "encoder.h"
#include <stdbool.h>
#include <stddef.h>

/* Reglages du plateau : une case = une LED, coordonnees de 0 a 7.
   (0, 0) en haut a gauche ; X vers la droite, Y vers le bas.
   1 = serpentin (ligne 0 gauche -> droite, ligne 1 droite -> gauche).
   0 = lignes successives, toutes de gauche a droite. */
#define BOARD_WIDTH           8U
#define BOARD_HEIGHT          8U
#define BOARD_SERPENTINE      1U

/* Position initiale conservee ; ensuite actualisee par les Pmod ENC.
   Ces variables sont publiees dans la boucle principale, pas dans l'ISR. */
volatile int32_t cursor_x = 0;
volatile int32_t cursor_y = 2;

/* Reglages des encodeurs : 4 transitions = un cycle A/B complet.
   Si un deplacement demande deux crans physiques, essayer 2U sur cet axe.
   Direction +1 : A monte avant B -> X droite / Y bas ; -1 inverse l'axe.
   Filtrage : nouvel etat A/B stable pendant au moins 2 ms. */
#define ENCODER_X_TRANSITIONS_PER_DETENT  4U
#define ENCODER_Y_TRANSITIONS_PER_DETENT  4U
#define ENCODER_X_DIRECTION               1
#define ENCODER_Y_DIRECTION               1
#define ENCODER_DEBOUNCE_MS               2U

_Static_assert(ENCODER_X_TRANSITIONS_PER_DETENT == 1U ||
               ENCODER_X_TRANSITIONS_PER_DETENT == 2U ||
               ENCODER_X_TRANSITIONS_PER_DETENT == 4U, "Transitions X : 1, 2 ou 4");
_Static_assert(ENCODER_Y_TRANSITIONS_PER_DETENT == 1U ||
               ENCODER_Y_TRANSITIONS_PER_DETENT == 2U ||
               ENCODER_Y_TRANSITIONS_PER_DETENT == 4U, "Transitions Y : 1, 2 ou 4");
_Static_assert(ENCODER_X_DIRECTION == 1 || ENCODER_X_DIRECTION == -1,
               "Direction X : +1 ou -1");
_Static_assert(ENCODER_Y_DIRECTION == 1 || ENCODER_Y_DIRECTION == -1,
               "Direction Y : +1 ou -1");
_Static_assert(ENCODER_DEBOUNCE_MS >= 1U, "Filtrage d'au moins 1 ms");

static EncoderDecoder encoder_x;
static EncoderDecoder encoder_y;
/* Les positions internes sont possedees par SysTick apres l'initialisation. */
static int32_t encoder_x_position;
static int32_t encoder_y_position;
static volatile bool encoders_ready;
/* Une lecture 32 bits alignee fournit un instantane coherent des deux axes.
   X dans les bits 0..7, Y dans les bits 8..15 : aucun verrou ni file de crans. */
static volatile uint32_t encoder_cursor;

static uint8_t Encoder_ReadX(uint32_t port_b)
{
  /* A = PB4 / D5 ; B = PB5 / D4. */
  return (uint8_t)(((port_b & GPIO_PIN_4) ? 2U : 0U) |
                   ((port_b & GPIO_PIN_5) ? 1U : 0U));
}

static uint8_t Encoder_ReadY(uint32_t port_b, uint32_t port_a)
{
  /* A = PB3 / D3 ; B = PA10 / D2. */
  return (uint8_t)(((port_b & GPIO_PIN_3) ? 2U : 0U) |
                   ((port_a & GPIO_PIN_10) ? 1U : 0U));
}

static void Encoders_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef inputs = {
    .Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
    .Mode = GPIO_MODE_INPUT,
    /* Le Pmod a deja des pull-up ; celles-ci evitent aussi une entree
       flottante si un module n'est pas branche. Alimentation Pmod : 3,3 V. */
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_FREQ_LOW
  };
  /* PB3/PB4 quittent AF0 JTAG/SWO ; SWD garde PA13/PA14 pour ST-LINK.
     Ne pas activer de trace SWO sur PB3 dans le debogueur. */
  HAL_GPIO_Init(GPIOB, &inputs);
  inputs.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &inputs);
  HAL_Delay(ENCODER_DEBOUNCE_MS);

  const uint32_t port_b = GPIOB->IDR;
  const uint32_t port_a = GPIOA->IDR;
  const uint32_t now = HAL_GetTick();
  Encoder_Init(&encoder_x, Encoder_ReadX(port_b), now);
  Encoder_Init(&encoder_y, Encoder_ReadY(port_b, port_a), now);
  encoder_x_position = Encoder_MoveClamped(cursor_x, 0, (int32_t)BOARD_WIDTH - 1);
  encoder_y_position = Encoder_MoveClamped(cursor_y, 0, (int32_t)BOARD_HEIGHT - 1);
  encoder_cursor = (uint32_t)encoder_x_position | ((uint32_t)encoder_y_position << 8U);
  /* Priorite 2 : le callback DMA du ruban (priorite 1) peut preempter SysTick.
     Le tick HAL reste a 1 ms et la configuration de TIM1 n'est pas modifiee. */
  HAL_NVIC_SetPriority(SysTick_IRQn, 2U, 0U);
  __DMB();
  encoders_ready = true;
}

static void Encoders_Sample1ms(void)
{
  if (!encoders_ready)
  {
    return;
  }

  /* Deux lectures de ports seulement, aucun delai ni affichage dans l'ISR. */
  const uint32_t port_b = GPIOB->IDR;
  const uint32_t port_a = GPIOA->IDR;
  const uint32_t now = HAL_GetTick();
  const int8_t step_x = Encoder_Sample(&encoder_x, Encoder_ReadX(port_b), now,
    ENCODER_DEBOUNCE_MS, ENCODER_X_TRANSITIONS_PER_DETENT);
  const int8_t step_y = Encoder_Sample(&encoder_y, Encoder_ReadY(port_b, port_a), now,
    ENCODER_DEBOUNCE_MS, ENCODER_Y_TRANSITIONS_PER_DETENT);
  encoder_x_position = Encoder_MoveClamped(encoder_x_position,
    (int8_t)(step_x * ENCODER_X_DIRECTION), (int32_t)BOARD_WIDTH - 1);
  encoder_y_position = Encoder_MoveClamped(encoder_y_position,
    (int8_t)(step_y * ENCODER_Y_DIRECTION), (int32_t)BOARD_HEIGHT - 1);
  encoder_cursor = (uint32_t)encoder_x_position | ((uint32_t)encoder_y_position << 8U);
}

typedef struct
{
  int32_t x;
  int32_t y;
} BoardPosition;

/* Bateaux caches : chaque entree represente une case occupee. */
static const BoardPosition test_ships[] = {
  {4, 3}, {5, 3}, {6, 3},
  {2, 5}, {2, 6}
};

/* Tirs appliques une seule fois au demarrage : eau (1,1), touche (4,3).
   Si les bateaux changent, le resultat est recalcule par Board_Fire(). */
static const BoardPosition test_shots[] = {
  {1, 1},
  {4, 3}
};

typedef enum
{
  CELL_SEA = 0,
  CELL_MISS,
  CELL_HIT
} CellState;

/* Etat persistant des tirs, independant du curseur et du cablage des LED. */
static CellState board[BOARD_HEIGHT][BOARD_WIDTH];

static bool Board_Contains(int32_t x, int32_t y)
{
  return x >= 0 && x < (int32_t)BOARD_WIDTH &&
         y >= 0 && y < (int32_t)BOARD_HEIGHT;
}

/* Index de 0 a 63 : 0 = premiere LED sur DIN (LED physique numero 1).
   UINT32_MAX signale une coordonnee invalide. */
static uint32_t Board_XYToLed(int32_t x, int32_t y)
{
  if (!Board_Contains(x, y))
  {
    return UINT32_MAX;
  }

  const uint32_t column = (BOARD_SERPENTINE && ((uint32_t)y & 1U))
                        ? BOARD_WIDTH - 1U - (uint32_t)x : (uint32_t)x;
  return (uint32_t)y * BOARD_WIDTH + column;
}

/* Futur bouton : appeler Board_Fire(cursor_x, cursor_y) dans la boucle
   principale, apres detection/anti-rebond. Aucun acces au materiel ici.
   Retourne false hors plateau, true pour un tir valide. Un nouveau tir sur
   la meme case conserve le resultat ; les bateaux non vises restent bleus. */
static bool Board_Fire(int32_t x, int32_t y)
{
  if (!Board_Contains(x, y))
  {
    return false;
  }

  CellState result = CELL_MISS;
  for (size_t i = 0U; i < sizeof(test_ships) / sizeof(test_ships[0]); ++i)
  {
    if (test_ships[i].x == x && test_ships[i].y == y)
    {
      result = CELL_HIT;
      break;
    }
  }

  board[y][x] = result;
  return true;
}

static void Board_Init(void)
{
  for (uint32_t y = 0U; y < BOARD_HEIGHT; ++y)
  {
    for (uint32_t x = 0U; x < BOARD_WIDTH; ++x)
    {
      board[y][x] = CELL_SEA;
    }
  }

  for (size_t i = 0U; i < sizeof(test_shots) / sizeof(test_shots[0]); ++i)
  {
    (void)Board_Fire(test_shots[i].x, test_shots[i].y);
  }
}

/* NUCLEO-L476RG: PA8/D7, TIM1_CH1, DMA1 canal 2, requete 7. */
static TIM_HandleTypeDef htim1;
static DMA_HandleTypeDef hdma_tim1_ch1;

#define SK6812_LED_COUNT      (BOARD_WIDTH * BOARD_HEIGHT)
#define SK6812_BITS_PER_LED   24U
#define SK6812_BRIGHTNESS     24U  /* Echelle de 0 a 255. */
/* Reenvoi de securite si le ruban est alimente apres la Nucleo.
   Les changements de position declenchent un envoi immediat, sans ce delai. */
#define SK6812_REFRESH_MS     1000U

#define SK6812_TIM_HZ         80000000U
#define SK6812_BITRATE        800000U
#define SK6812_PERIOD_TICKS   (SK6812_TIM_HZ / SK6812_BITRATE)
#define SK6812_T0H_TICKS      24U  /* Bit 0 : haut 300 ns, bas 950 ns. */
#define SK6812_T1H_TICKS      48U  /* Bit 1 : haut 600 ns, bas 650 ns. */

#define SK6812_DATA_SLOTS     (SK6812_LED_COUNT * SK6812_BITS_PER_LED)
#define SK6812_RESET_US      300U
/* Deux periodes en plus couvrent le preload et la derniere periode incomplete. */
#define SK6812_RESET_SLOTS    (((SK6812_RESET_US * SK6812_BITRATE + 999999U) \
                              / 1000000U) + 2U)
#define SK6812_DMA_SLOTS      (SK6812_DATA_SLOTS + SK6812_RESET_SLOTS)
#define SK6812_TIMEOUT_MS     10U

_Static_assert(SK6812_BRIGHTNESS <= 255U, "Luminosite entre 0 et 255");
_Static_assert(BOARD_SERPENTINE <= 1U, "Cablage : 0 = lignes, 1 = serpentin");
_Static_assert(SK6812_TIM_HZ % SK6812_BITRATE == 0U, "Periode PWM entiere");
_Static_assert(SK6812_T0H_TICKS < SK6812_T1H_TICKS &&
               SK6812_T1H_TICKS < SK6812_PERIOD_TICKS, "Rapports cycliques valides");
_Static_assert(SK6812_DMA_SLOTS <= UINT16_MAX, "Longueur DMA sur 16 bits");

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} SK6812_Color;

static const SK6812_Color sk6812_miss = {0U, 255U, 0U};
static const SK6812_Color sk6812_hit  = {255U, 0U, 0U};
static const SK6812_Color sk6812_sea  = {0U, 0U, 255U};
static const SK6812_Color sk6812_cursor = {255U, 255U, 255U};

typedef enum
{
  SK6812_DMA_PENDING,
  SK6812_DMA_COMPLETE,
  SK6812_DMA_ERROR
} SK6812_DmaState;

static volatile SK6812_DmaState sk6812_dma_state;
/* Pour le debogueur : un transfert termine ne prouve pas la reception du ruban. */
static volatile uint32_t sk6812_frames_sent;

/* Le DMA transfere des demi-mots. L'API HAL utilise un pointeur uint32_t. */
_Alignas(uint32_t) static uint16_t sk6812_pwm[SK6812_DMA_SLOTS];

static uint32_t SK6812_Scale(uint8_t value)
{
  return ((uint32_t)value * SK6812_BRIGHTNESS + 127U) / 255U;
}

static void SK6812_FillBuffer(void)
{
  /* Une seule lecture de chaque variable par image. Ne jamais ecrire le
     blanc dans board[][] : le curseur ne change que le rendu de la case. */
  const int32_t frame_cursor_x = cursor_x;
  const int32_t frame_cursor_y = cursor_y;

  for (uint32_t y = 0U; y < BOARD_HEIGHT; ++y)
  {
    for (uint32_t x = 0U; x < BOARD_WIDTH; ++x)
    {
      const SK6812_Color *color = &sk6812_sea;
      if (board[y][x] == CELL_MISS)
      {
        color = &sk6812_miss;
      }
      else if (board[y][x] == CELL_HIT)
      {
        color = &sk6812_hit;
      }
      if ((int32_t)x == frame_cursor_x && (int32_t)y == frame_cursor_y)
      {
        color = &sk6812_cursor;
      }

      /* 24 bits par LED : G7..G0, R7..R0, B7..B0, poids fort en premier.
         Le DMA parcourt ensuite le tableau dans l'ordre physique du ruban. */
      const uint32_t grb = (SK6812_Scale(color->green) << 16U)
                         | (SK6812_Scale(color->red) << 8U)
                         | SK6812_Scale(color->blue);
      uint32_t slot = Board_XYToLed((int32_t)x, (int32_t)y) * SK6812_BITS_PER_LED;

      for (uint32_t mask = 1U << 23U; mask != 0U; mask >>= 1U)
      {
        sk6812_pwm[slot++] = (grb & mask) ? SK6812_T1H_TICKS : SK6812_T0H_TICKS;
      }
    }
  }

  /* CCR1 = 0 garde DATA bas pendant toute la periode PWM. */
  for (uint32_t slot = SK6812_DATA_SLOTS; slot < SK6812_DMA_SLOTS; ++slot)
  {
    sk6812_pwm[slot] = 0U;
  }
}

static void SK6812_SetDataPin(uint32_t mode)
{
  GPIO_InitTypeDef gpio = {
    .Pin = GPIO_PIN_8,
    .Mode = mode,
    .Pull = GPIO_PULLDOWN,
    .Speed = GPIO_SPEED_FREQ_HIGH,
    .Alternate = GPIO_AF1_TIM1
  };

  /* Precharger le niveau bas avant de passer en sortie GPIO. */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
  HAL_GPIO_Init(GPIOA, &gpio);
}

static uint32_t SK6812_GetTim1Clock(void)
{
  RCC_ClkInitTypeDef clocks;
  uint32_t flash_latency;
  const uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();

  HAL_RCC_GetClockConfig(&clocks, &flash_latency);
  return (clocks.APB2CLKDivider == RCC_HCLK_DIV1) ? pclk2 : (2U * pclk2);
}

static HAL_StatusTypeDef SK6812_Show(void)
{
  const DMA_HandleTypeDef *dma = htim1.hdma[TIM_DMA_ID_CC1];
  SK6812_SetDataPin(GPIO_MODE_OUTPUT_PP);

  if (SK6812_GetTim1Clock() != SK6812_TIM_HZ ||
      htim1.Instance->PSC != 0U ||
      htim1.Instance->ARR != (SK6812_PERIOD_TICKS - 1U) ||
      htim1.Instance->RCR != 0U ||
      dma == NULL ||
      dma->Instance != DMA1_Channel2 ||
      dma->Init.Request != DMA_REQUEST_7 ||
      dma->Init.Mode != DMA_NORMAL ||
      dma->Init.MemDataAlignment != DMA_MDATAALIGN_HALFWORD ||
      dma->Init.PeriphDataAlignment != DMA_PDATAALIGN_HALFWORD)
  {
    return HAL_ERROR;
  }

  HAL_Delay(2U);  /* Reset bas avant le premier bit. */

  /* CCDS = 1 : requete CC1 a chaque update, meme lorsque CCR1 vaut zero.
     On conserve TIM_DMA_CC1 et DMA1 canal 2 / requete 7, pas TIM_DMA_UPDATE. */
  __HAL_TIM_DISABLE(&htim1);
  __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_CC1);
  SET_BIT(htim1.Instance->CR2, TIM_CR2_CCDS);
  __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0U);

  /* Charger zero dans le compare actif avant de connecter PA8 au timer.
     Au 1er update, le DMA ecrit le premier bit dans le preload.
     Au 2e update, ce bit devient actif : aucun bit de donnees n'est saute. */
  __HAL_TIM_SET_COUNTER(&htim1, 0U);
  htim1.Instance->EGR = TIM_EGR_UG;
  __HAL_TIM_CLEAR_FLAG(&htim1, (uint32_t)(TIM_FLAG_UPDATE | TIM_FLAG_CC1));
  sk6812_dma_state = SK6812_DMA_PENDING;
  SK6812_SetDataPin(GPIO_MODE_AF_PP);

  /* TIM1 est un timer avance : la HAL active aussi BDTR.MOE au demarrage. */
  HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(
    &htim1, TIM_CHANNEL_1, (uint32_t *)(void *)sk6812_pwm, SK6812_DMA_SLOTS);

  if (status == HAL_OK)
  {
    const uint32_t start = HAL_GetTick();

    while (sk6812_dma_state == SK6812_DMA_PENDING &&
           (HAL_GetTick() - start) < SK6812_TIMEOUT_MS)
    {
      /* TIM1 et le DMA produisent le signal pendant cette attente. */
    }

    const SK6812_DmaState result = sk6812_dma_state;
    if (result == SK6812_DMA_PENDING)
    {
      status = HAL_TIMEOUT;
    }
    else if (result == SK6812_DMA_ERROR)
    {
      status = HAL_ERROR;
    }
    else if (__HAL_DMA_GET_COUNTER(&hdma_tim1_ch1) != 0U ||
             __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != 0U)
    {
      status = HAL_ERROR;
    }
  }

  /* Les zeros de fin assurent deja au moins 300 us bas avant le callback.
     La sortie GPIO prend ensuite le relais au niveau bas avant l'arret PWM. */
  SK6812_SetDataPin(GPIO_MODE_OUTPUT_PP);
  (void)HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
  HAL_Delay(1U);  /* Garantir aussi un reset bas en cas d'erreur de transfert. */
  return status;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    sk6812_dma_state = SK6812_DMA_COMPLETE;
  }
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    sk6812_dma_state = SK6812_DMA_ERROR;
  }
}


/* HSI16 -> PLL (16 MHz / 1 x 10 / 2) -> SYSCLK et TIM1 a 80 MHz. */
static HAL_StatusTypeDef BoardClock_Init(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  RCC_OscInitTypeDef oscillator = {
    .OscillatorType = RCC_OSCILLATORTYPE_HSI,
    .HSIState = RCC_HSI_ON,
    .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
    .PLL = {
      .PLLState = RCC_PLL_ON,
      .PLLSource = RCC_PLLSOURCE_HSI,
      .PLLM = 1U,
      .PLLN = 10U,
      .PLLP = RCC_PLLP_DIV7,
      .PLLQ = RCC_PLLQ_DIV2,
      .PLLR = RCC_PLLR_DIV2
    }
  };
  if (HAL_RCC_OscConfig(&oscillator) != HAL_OK)
  {
    return HAL_ERROR;
  }

  RCC_ClkInitTypeDef clocks = {
    .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
               | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
    .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
    .AHBCLKDivider = RCC_SYSCLK_DIV1,
    .APB1CLKDivider = RCC_HCLK_DIV1,
    .APB2CLKDivider = RCC_HCLK_DIV1
  };
  return HAL_RCC_ClockConfig(&clocks, FLASH_LATENCY_4);
}

static HAL_StatusTypeDef SK6812_HardwareInit(void)
{
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  htim1.Instance = TIM1;
  htim1.Init = (TIM_Base_InitTypeDef) {
    .Prescaler = 0U,
    .CounterMode = TIM_COUNTERMODE_UP,
    .Period = SK6812_PERIOD_TICKS - 1U,
    .ClockDivision = TIM_CLOCKDIVISION_DIV1,
    .RepetitionCounter = 0U,  /* Un update et une requete DMA a chaque bit. */
    .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
  };
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  TIM_OC_InitTypeDef pwm = {
    .OCMode = TIM_OCMODE_PWM1,
    .Pulse = 0U,
    .OCPolarity = TIM_OCPOLARITY_HIGH,
    .OCFastMode = TIM_OCFAST_DISABLE,
    .OCNPolarity = TIM_OCNPOLARITY_HIGH,
    .OCIdleState = TIM_OCIDLESTATE_RESET,
    .OCNIdleState = TIM_OCNIDLESTATE_RESET
  };
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &pwm, TIM_CHANNEL_1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Aucun break, aucun temps mort : sortie CH1 uniquement.
     MOE est gere par HAL_TIM_PWM_Start_DMA / HAL_TIM_PWM_Stop_DMA. */
  TIM_BreakDeadTimeConfigTypeDef output = {
    .OffStateRunMode = TIM_OSSR_DISABLE,
    .OffStateIDLEMode = TIM_OSSI_DISABLE,
    .LockLevel = TIM_LOCKLEVEL_OFF,
    .DeadTime = 0U,
    .BreakState = TIM_BREAK_DISABLE,
    .BreakPolarity = TIM_BREAKPOLARITY_HIGH,
    .BreakFilter = 0U,
    .Break2State = TIM_BREAK2_DISABLE,
    .Break2Polarity = TIM_BREAK2POLARITY_HIGH,
    .Break2Filter = 0U,
    .AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE
  };
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &output) != HAL_OK)
  {
    return HAL_ERROR;
  }

  hdma_tim1_ch1.Instance = DMA1_Channel2;
  hdma_tim1_ch1.Init = (DMA_InitTypeDef) {
    .Request = DMA_REQUEST_7,
    .Direction = DMA_MEMORY_TO_PERIPH,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
    .MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
    .Mode = DMA_NORMAL,
    .Priority = DMA_PRIORITY_HIGH
  };
  if (HAL_DMA_Init(&hdma_tim1_ch1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  __HAL_LINKDMA(&htim1, hdma[TIM_DMA_ID_CC1], hdma_tim1_ch1);
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1U, 0U);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  return HAL_OK;
}

void SysTick_Handler(void)
{
  HAL_IncTick();
  Encoders_Sample1ms();
}

void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);
}

int main(void)
{
  HAL_Init();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* LD2 reste allumee en cas de succes et clignote en cas d'erreur. */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  GPIO_InitTypeDef status_led = {
    .Pin = GPIO_PIN_5,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW
  };
  HAL_GPIO_Init(GPIOA, &status_led);
  SK6812_SetDataPin(GPIO_MODE_OUTPUT_PP);

  if (BoardClock_Init() != HAL_OK || SK6812_HardwareInit() != HAL_OK)
  {
    while (1)
    {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      HAL_Delay(250U);
    }
  }

  Board_Init();
  Encoders_Init();
  uint32_t displayed_cursor = UINT32_MAX;
  uint32_t last_refresh = 0U;
  while (1)
  {
    /* Instantane atomique : un mouvement pendant le DMA sera pris en compte
       au tour suivant, sans toucher au tampon PWM pendant sa transmission. */
    const uint32_t current_cursor = encoder_cursor;
    cursor_x = (int32_t)(current_cursor & 0xFFU);
    cursor_y = (int32_t)((current_cursor >> 8U) & 0xFFU);
    /* Futur bouton : appeler Board_Fire(cursor_x, cursor_y) apres anti-rebond,
       puis demander un rafraichissement. BTN et SWT restent libres ici. */
    if (current_cursor != displayed_cursor ||
        (uint32_t)(HAL_GetTick() - last_refresh) >= SK6812_REFRESH_MS)
    {
      SK6812_FillBuffer();
      if (SK6812_Show() != HAL_OK)
      {
        while (1)
        {
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
          HAL_Delay(250U);
        }
      }

      displayed_cursor = current_cursor;
      last_refresh = HAL_GetTick();
      ++sk6812_frames_sent;
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
    /* SysTick reveille le coeur au plus tard au tick suivant. La lecture
       continue pendant les HAL_Delay et pendant le transfert TIM1/DMA. */
    __WFI();
  }
}
