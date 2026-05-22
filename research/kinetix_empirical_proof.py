import numpy as np
import matplotlib.pyplot as plt

# =============================================================================
# [KINETIX] EMPIRICAL TOPOLOGICAL DRIFT SIMULATOR (HDC)
# =============================================================================

# Architecture parameters
DIMENSIONS = 40000
VETO_THRESHOLD = 0.45       # KinetiX Veto threshold (45% Hamming distance)
HARDWARE_ERROR = 0.015625   # The "Superscalar Hack": 1.5625% constant hardware error
TICKS_TOTAL = 10000         # Number of orders simulated before crash injection

def generate_noise_mask(dimensions, error_rate):
    """Generates a bit mask to simulate data corruption (market or hardware noise)"""
    return np.random.rand(dimensions) < error_rate

print("[SYS] Initializing 40,000-dimensional HDC space...")
# 1. Generate Nominal Cassette (The baseline healthy market state)
nominal_cassette = np.random.randint(2, size=DIMENSIONS, dtype=np.int8)

distance_history = []

print(f"[SYS] Simulating {TICKS_TOTAL} market ticks with hardware noise (1.56%)...")
# 2. Nominal Phase (Healthy market + KinetiX hardware noise)
for i in range(TICKS_TOTAL):
    # Natural market volatility (stochastic, e.g., between 0% and 3%)
    market_volatility = np.random.uniform(0.00, 0.03) 
    
    # Combined vector noise rate (market volatility + hardware carry loss)
    total_noise_rate = market_volatility + HARDWARE_ERROR
    noise_mask = generate_noise_mask(DIMENSIONS, total_noise_rate)
    
    # Received hypervector at the evaluation core
    incoming_vector = np.bitwise_xor(nominal_cassette, noise_mask)
    
    # Calculate Hamming distance (Veto Check simulation)
    distance = np.mean(incoming_vector != nominal_cassette)
    distance_history.append(distance)

# 3. Inject Flash Crash (Topological Dislocation)
print("[!] INJECTING FLASH CRASH (Massive topological decorrelation)")
# A flash crash generates an orthogonal vector (independent, resulting in ~50% distance)
flash_crash_vector = np.random.randint(2, size=DIMENSIONS, dtype=np.int8)
crash_distance = np.mean(flash_crash_vector != nominal_cassette)
distance_history.append(crash_distance)

# =============================================================================
# VISUAL WHITE PAPER GENERATION
# =============================================================================
plt.figure(figsize=(12, 6))
plt.plot(distance_history, color='dodgerblue', alpha=0.8, label='Topological Drift (Incoming Vector)')

# Reference Lines
plt.axhline(y=VETO_THRESHOLD, color='red', linestyle='-', linewidth=2, label='KinetiX Veto Threshold (45%)')
plt.axhline(y=HARDWARE_ERROR, color='orange', linestyle='--', linewidth=1.5, label='Fixed Hardware Noise (1.56%)')

# Chart Styling & Labels
plt.title('Empirical Proof: HDC Robustness (40,000 Dimensions) vs Hardware Noise', fontsize=14)
plt.xlabel('Ticks (Network Orders)', fontsize=12)
plt.ylabel('Hamming Distance (%)', fontsize=12)
plt.ylim(0, 0.55)
plt.yticks(np.arange(0, 0.60, 0.05), [f"{int(x*100)}%" for x in np.arange(0, 0.60, 0.05)])
plt.legend(loc='upper left')
plt.grid(True, alpha=0.3)

# Highlight the Flash Crash trigger point
plt.scatter(TICKS_TOTAL, crash_distance, color='darkred', s=100, zorder=5)
plt.annotate('FLASH CRASH\n(Veto Latched)', 
             xy=(TICKS_TOTAL, crash_distance), 
             xytext=(TICKS_TOTAL - 1500, crash_distance + 0.02),
             arrowprops=dict(facecolor='red', shrink=0.05, width=2, headwidth=8),
             fontsize=10, color='darkred', fontweight='bold')

plt.tight_layout()
print("[SYS] Graph rendering complete. Saving to 'kinetix_proof.png'...")
plt.savefig('kinetix_proof.png', dpi=300)
plt.close()
print("[SYS] Script execution completed successfully.")
