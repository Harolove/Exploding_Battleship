#include "encoder.h"
#include <stddef.h>

/* Tests du vrai decodeur C, executes sur un Cortex-M4 emule, sans HAL. */
#define CHECK(condition) do { if (!(condition)) { return __LINE__; } } while (0)

static uint32_t now_ms;

static int8_t Hold(EncoderDecoder *encoder, uint8_t ab, uint8_t per_detent)
{
  int8_t steps = 0;
  for (uint32_t i = 0U; i < 3U; ++i)
  {
    steps += Encoder_Sample(encoder, ab, ++now_ms, 2U, per_detent);
  }
  return steps;
}

int main(void)
{
  /* Toutes les phases initiales, les deux sens, et les trois resolutions. */
  const uint8_t cycle[] = {0U, 2U, 3U, 1U};
  const uint8_t resolutions[] = {1U, 2U, 4U};
  for (size_t r = 0U; r < sizeof(resolutions); ++r)
  {
    for (uint32_t start = 0U; start < 4U; ++start)
    {
      for (int32_t sign = -1; sign <= 1; sign += 2)
      {
        EncoderDecoder encoder;
        Encoder_Init(&encoder, cycle[start], now_ms);
        int32_t total = 0;
        for (int32_t edge = 1; edge <= 24; ++edge)
        {
          const uint32_t index = (uint32_t)((int32_t)start + sign * edge + 28) % 4U;
          const int8_t step = Hold(&encoder, cycle[index], resolutions[r]);
          CHECK(step == ((edge % resolutions[r] == 0) ? sign : 0));
          total += step;
        }
        CHECK(total == sign * 24 / resolutions[r]);
      }
    }
  }

  EncoderDecoder encoder;
  Encoder_Init(&encoder, 3U, now_ms);
  /* Entree immobile et impulsions plus courtes que le filtre. */
  for (uint32_t i = 0U; i < 100U; ++i)
  {
    CHECK(Hold(&encoder, 3U, 4U) == 0);
    CHECK(Encoder_Sample(&encoder, 1U, ++now_ms, 2U, 4U) == 0);
    CHECK(Encoder_Sample(&encoder, 3U, ++now_ms, 2U, 4U) == 0);
  }
  CHECK(encoder.accepted_ab == 3U && encoder.transitions == 0);

  /* Meme un rebond assez long pour passer le filtre s'annule au retour. */
  for (uint32_t i = 0U; i < 100U; ++i)
  {
    CHECK(Hold(&encoder, 1U, 4U) == 0);
    CHECK(Hold(&encoder, 3U, 4U) == 0);
  }
  CHECK(encoder.transitions == 0);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 2U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 1);

  /* Inversion avant la fin d'un cran : pas de deplacement net. */
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 0);
  CHECK(encoder.transitions == 0);

  /* Saut diagonal : abandonner le partiel, attendre la reference, repartir. */
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 2U, 4U) == 0);
  CHECK(!encoder.synchronized && encoder.transitions == 0);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 0);
  CHECK(encoder.synchronized && encoder.transitions == 0);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 2U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 1);

  /* Bouclage de HAL_GetTick : la soustraction non signee reste correcte. */
  now_ms = UINT32_MAX - 1U;
  Encoder_Init(&encoder, 3U, now_ms);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(now_ms == 1U && encoder.accepted_ab == 1U);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 2U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 1);

  /* Deux decodeurs independants, y compris si les axes tournent ensemble. */
  EncoderDecoder other;
  Encoder_Init(&encoder, 3U, now_ms);
  Encoder_Init(&other, 3U, now_ms);
  CHECK(Hold(&encoder, 1U, 4U) == 0);
  CHECK(Hold(&other, 2U, 4U) == 0);
  CHECK(Hold(&encoder, 0U, 4U) == 0);
  CHECK(Hold(&other, 0U, 4U) == 0);
  CHECK(Hold(&encoder, 2U, 4U) == 0);
  CHECK(Hold(&other, 1U, 4U) == 0);
  CHECK(Hold(&encoder, 3U, 4U) == 1);
  CHECK(Hold(&other, 3U, 4U) == -1);

  /* Saturation a chaque cran, retour immediat, inversion d'un axe. */
  int32_t x = 0;
  int32_t y = 7;
  for (uint32_t i = 0U; i < 100U; ++i)
  {
    x = Encoder_MoveClamped(x, -1, 7);
    y = Encoder_MoveClamped(y, 1, 7);
    CHECK(x == 0 && y == 7);
  }
  CHECK(Encoder_MoveClamped(x, 1, 7) == 1);
  CHECK(Encoder_MoveClamped(y, -1, 7) == 6);
  CHECK(Encoder_MoveClamped(3, (int8_t)(1 * -1), 7) == 2);
  CHECK(Encoder_MoveClamped(-100, 0, 7) == 0);
  CHECK(Encoder_MoveClamped(100, 0, 7) == 7);
  return 0;
}
