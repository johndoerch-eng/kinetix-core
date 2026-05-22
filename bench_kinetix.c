#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

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
    printf("[SYS] Initializing KinetiX Multi-Cassette Global Benchmark...\n");


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

    printf("[SYS] Running %d multiplexing iterations...\n", ITERATIONS);

    volatile int dummy_sum = 0;

    // Mesure globale (en dehors de la boucle)
#ifdef _MSC_VER
    start_cycles = __rdtscp(&ui); 
#else
    start_cycles = __rdtscp(&ui); 
#endif

    for (int i = 0; i < ITERATIONS; ++i) {
        dummy_sum += Kinetix_Veto_Check(sram_buffer, 0, signal_state);
    }

    end_cycles = __rdtscp(&ui); 
    total_cycles = (end_cycles - start_cycles);

    double avg_cycles = (double)total_cycles / ITERATIONS;
    double nanoseconds = (avg_cycles / 4.2);

    printf("======================================\n");
    printf("[RESULT] Average CPU cycles per step : %.2f cycles\n", avg_cycles);
    printf("[RESULT] Estimated physical latency  : %.2f nanoseconds\n", nanoseconds);
    printf("======================================\n");

    return 0;
}


