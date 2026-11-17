#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

/* Decodeur sans dependance HAL. AB = (A << 1) | B.
   Appeler Encoder_Sample() toutes les 1 ms, meme pendant l'affichage. */
typedef struct
{
  uint32_t candidate_since_ms;
  uint8_t candidate_ab;
  uint8_t accepted_ab;
  uint8_t reference_ab;
  int8_t transitions;
  bool synchronized;
} EncoderDecoder;

void Encoder_Init(EncoderDecoder *encoder, uint8_t ab, uint32_t now_ms);

/* Retourne -1, 0 ou +1 cran. transitions_per_detent : 1, 2 ou 4.
   Positif : 00 -> 10 -> 11 -> 01 -> 00 (A monte avant B).
   Une transition n'est acceptee qu'apres debounce_ms de stabilite.
   Un saut de deux bits annule le mouvement partiel et impose un retour
   a l'etat de reference avant de recommencer a compter. */
int8_t Encoder_Sample(EncoderDecoder *encoder, uint8_t ab, uint32_t now_ms,
                      uint32_t debounce_ms, uint8_t transitions_per_detent);

/* Appliquer chaque cran separement : pas d'accumulation au bord du plateau. */
int32_t Encoder_MoveClamped(int32_t position, int8_t step, int32_t maximum);

#endif
