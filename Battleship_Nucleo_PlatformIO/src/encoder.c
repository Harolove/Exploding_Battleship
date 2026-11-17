#include "encoder.h"

void Encoder_Init(EncoderDecoder *encoder, uint8_t ab, uint32_t now_ms)
{
  ab &= 3U;
  *encoder = (EncoderDecoder) {
    .candidate_since_ms = now_ms,
    .candidate_ab = ab,
    .accepted_ab = ab,
    .reference_ab = ab,
    .transitions = 0,
    .synchronized = true
  };
}

int8_t Encoder_Sample(EncoderDecoder *encoder, uint8_t ab, uint32_t now_ms,
                      uint32_t debounce_ms, uint8_t transitions_per_detent)
{
  /* Lignes = ancien AB, colonnes = nouvel AB. Les rebonds aller-retour
     sur une seule voie s'annulent dans l'accumulateur signe. */
  static const int8_t direction[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  ab &= 3U;
  if (ab != encoder->candidate_ab)
  {
    encoder->candidate_ab = ab;
    encoder->candidate_since_ms = now_ms;
    return 0;
  }
  if (ab == encoder->accepted_ab ||
      (uint32_t)(now_ms - encoder->candidate_since_ms) < debounce_ms)
  {
    return 0;
  }

  const uint8_t previous = encoder->accepted_ab;
  encoder->accepted_ab = ab;
  if ((previous ^ ab) == 3U)
  {
    /* Deux voies ont change : sens indeterminable, aucun cran invente. */
    encoder->transitions = 0;
    encoder->synchronized = false;
  }
  if (!encoder->synchronized)
  {
    if (ab == encoder->reference_ab)
    {
      encoder->synchronized = true;
    }
    return 0;
  }

  encoder->transitions += direction[(previous << 2U) | ab];
  if (encoder->transitions >= (int8_t)transitions_per_detent)
  {
    encoder->transitions = 0;
    return 1;
  }
  if (encoder->transitions <= -(int8_t)transitions_per_detent)
  {
    encoder->transitions = 0;
    return -1;
  }
  return 0;
}

int32_t Encoder_MoveClamped(int32_t position, int8_t step, int32_t maximum)
{
  if (position < 0)
  {
    position = 0;
  }
  else if (position > maximum)
  {
    position = maximum;
  }
  if (step > 0 && position < maximum)
  {
    ++position;
  }
  else if (step < 0 && position > 0)
  {
    --position;
  }
  return position;
}
