#include "kinetix_core.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>
#define popcount64 __popcnt64
#else
#define popcount64 __builtin_popcountll
#endif

#define KINETIX_TOPOLOGICAL_THRESHOLD_MAX 0.45f

void Kinetix_Init(void* sram_buffer, const char* multi_cassette_path) {
    if (!sram_buffer || !multi_cassette_path) return;
    
    // Chargement global du set de cassettes alignées L1 (5056 octets par cassette)
    FILE* file = fopen(multi_cassette_path, "rb");
    if (file) {
        // Chargement d'un set de sécurité (ex: 100 cassettes, ~500 Ko)
        fread(sram_buffer, 1, 5056 * 100, file);
        fclose(file);
    }
}

int Kinetix_Veto_Check(const void* sram_buffer, uint32_t station_id, const uint64_t* current_signal_state) {
    if (!sram_buffer || !current_signal_state) return 1;

    /* ========================================================================= */
    /* KINETIX VETO - FUSED BITWISE ROLL & POPCOUNT MULTIPLEXED (O(1))           */
    /* ========================================================================= */

    // Calcul d'adresse direct (station_id * 5056) traduisible en LEA par le compilateur
    // 5056 octets = 79 lignes de cache L1 de 64 octets (alignement optimal)
    const uint64_t* nominal_hv_64 = (const uint64_t*)((const uint8_t*)sram_buffer + (station_id * 5056) + 16); 
    const uint64_t* source_hv_64  = current_signal_state;

    uint32_t h0 = 0, h1 = 0, h2 = 0, h3 = 0;

    // PISTE 2 + ACCUMULATION SUPERSCALAIRE
    // On déroule la boucle par 4 et on casse la dépendance d'accumulation.
    uint32_t i = 0;
    for (; i < 624; i += 4) {
        h0 += popcount64((source_hv_64[i]   << 1) ^ nominal_hv_64[i]);
        h1 += popcount64((source_hv_64[i+1] << 1) ^ nominal_hv_64[i+1]);
        h2 += popcount64((source_hv_64[i+2] << 1) ^ nominal_hv_64[i+2]);
        h3 += popcount64((source_hv_64[i+3] << 1) ^ nominal_hv_64[i+3]);
    }

    uint32_t hamming_distance = h0 + h1 + h2 + h3;
    hamming_distance += popcount64((source_hv_64[624] << 1) ^ nominal_hv_64[624]);


    float current_drift = (float)hamming_distance / 40000.0f;
    
    if (current_drift > KINETIX_TOPOLOGICAL_THRESHOLD_MAX) {
        return 1; // VETO LATCHED (Sécurité déclenchée)
    }
    
    return 0; // NOMINAL
}
