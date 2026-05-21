#ifndef KINETIX_CORE_H
#define KINETIX_CORE_H
#include <stdint.h>

// Initialisation globale avec le paquet concaténé de cassettes
void Kinetix_Init(void* sram_buffer, const char* multi_cassette_path);

// Évaluation O(1) d'une station critique spécifique
// Retourne 1 si DÉRIVE DÉTECTÉE, 0 si OK
int Kinetix_Veto_Check(const void* sram_buffer, uint32_t station_id, const uint64_t* current_signal_state);

#endif
