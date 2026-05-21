#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>
#define ALIGN64 __declspec(align(64))
#else
#include <x86intrin.h>
#define ALIGN64 __attribute__((aligned(64)))
#endif

#include "kinetix_core.h" 

#define ITERATIONS 1000000

int main() {
    printf("[SYS] Initialisation du Benchmark KinetiX Multi-Cassettes (Mondial)...\n");

    // Allocation SRAM de 1 Mo pour stocker les cassettes mondiales chargées
    static ALIGN64 uint8_t sram_buffer[1024 * 1024]; 
    
    // Chargement du fichier binaire mondial concaténé (dummy payload public)
    Kinetix_Init(sram_buffer, "data/dummy_payload.bin");

    // Simulation de l'état du signal pour la station cible (station index 0)
    ALIGN64 uint64_t signal_state[625];
    for(int i = 0; i < 625; ++i) {
        signal_state[i] = ((uint64_t*)((uint8_t*)sram_buffer + 16))[i];
    }

    unsigned long long start_cycles, end_cycles, total_cycles = 0;
    unsigned int ui;

    printf("[SYS] Lancement de %d itérations de multiplexage...\n", ITERATIONS);

    // Boucle de mesure
    for (int i = 0; i < ITERATIONS; ++i) {
#ifdef _MSC_VER
        start_cycles = __rdtscp(&ui); 
#else
        start_cycles = __rdtscp(&ui); 
#endif
        
        // Interrogation de la station ID 0
        Kinetix_Veto_Check(sram_buffer, 0, signal_state);

        end_cycles = __rdtscp(&ui); 
        total_cycles += (end_cycles - start_cycles);
    }

    double avg_cycles = (double)total_cycles / ITERATIONS;
    double nanoseconds = (avg_cycles / 4.2);

    printf("======================================\n");
    printf("[RESULTAT] Cycles CPU moyens par pas : %.2f cycles\n", avg_cycles);
    printf("[RESULTAT] Latence physique estimée  : %.2f nanosecondes\n", nanoseconds);
    printf("======================================\n");

    return 0;
}

