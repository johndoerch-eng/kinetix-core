#!/usr/bin/env python3
import os

# =============================================================================
# KINETIX PUBLIC SANDBOX : DUMMY CASSETTE GENERATOR
# =============================================================================
# This script generates a strictly L1-aligned dummy cassette for benchmark purposes.
# It contains NO proprietary matrix weights or semantic HDC projections.

STATIONS_COUNT = 10
DIMENSIONS_HDC = 40000
PAYLOAD_BYTES = DIMENSIONS_HDC // 8  # 5000 bytes
L1_ALIGNED_SIZE = 5056               # Pad to 64-byte boundaries

def generate_dummy():
    os.makedirs("data", exist_ok=True)
    filepath = "data/dummy_payload.bin"
    
    with open(filepath, "wb") as f:
        for station_id in range(STATIONS_COUNT):
            # Header : Magic (8B) + Version (4B) + Dims (4B) = 16 bytes
            header = b'XENISOS1' + (1).to_bytes(4, 'little') + DIMENSIONS_HDC.to_bytes(4, 'little')
            
            # Dummy Payload : Dead geometric pattern (0xAA = 10101010)
            payload = bytes([0xAA] * PAYLOAD_BYTES)
            
            # Padding to align perfectly with CPU Cache Lines (L1)
            raw_cassette = header + payload
            padding_needed = L1_ALIGNED_SIZE - len(raw_cassette)
            final_cassette = raw_cassette + (b'\x00' * padding_needed)
            
            f.write(final_cassette)
            
    print(f"[SUCCESS] Dummy cassette generated at {filepath} ({os.path.getsize(filepath)} bytes).")
    print("[INFO] L1 Cache alignment enforced. Ready for O(1) benchmarking.")

if __name__ == "__main__":
    generate_dummy()
