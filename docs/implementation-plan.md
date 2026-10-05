# OrcGroove First Playable Plan

## Goal

Build a reproducible first playable GrooveMatter prototype that proves:

1. stable shared-field synthesis at 48 kHz
2. harmony-dependent timbral interaction
3. deterministic fallback control without ML
4. compact GrooveNet-µ training
5. INT8 edge export
6. portable reference inference
7. ESP32-P4 build/benchmark firmware
8. a Tab5 touch-first instrument UI

## Milestone tasks

### 1. Stable coupled-field DSP core
Create fixed-capacity 12–24-cell field state, bounded nonlinear coupling, energy guards, deterministic reset, and Debug/Release tests for determinism, decay, extreme controls, and invalid-state recovery.

### 2. Topologies, exciters, and macro mapping
Implement ring and lattice/grid topologies, pluck/impulse and continuous excitation, and the five first-generation macros: Matter, Tension, Coupling, Memory, Chaos.

### 3. Harmony features
Create a deterministic compact harmony representation that captures active-note relationships such as pitch classes, interval structure, register spread, consonance/roughness proxies, and bounded note summaries.

### 4. Realtime engine
Integrate field, exciters, macros, and harmony state into a no-allocation render path. Support 32/64-sample blocks and 128-sample fallback control cadence with interpolation. Missing/skipped ML inference must not interrupt audio.

### 5. Deterministic render/probe tools
Add host CLIs that produce repeatable WAV/control traces and measure stability, interaction residuals, reset count, and host timing.

### 6. GrooveGenome v0
Define versioned, checksummed portable patches with strict rejection of unsupported major versions, traversal, corrupted resources, executable payloads, and missing edge fallbacks.

### 7. GrooveDNA dataset generation
Generate deterministic training sequences spanning ring/lattice, pluck/continuous, isolated notes, consonant intervals, close clusters, wide registers, and macro extremes.

### 8. GrooveNet-µ reference models
Implement small edge-friendly recurrent controllers using projection/GEMM, explicit state, bounded outputs, and LUT-friendly activation paths. Reference µ32 and µ128 models must remain inside their parameter budgets.

### 9. Training
Train seeded µ32/µ128 models and compare them against the deterministic no-ML fallback. A learned model only advances if it measurably improves at least one held-out trajectory metric without violating stability.

### 10. INT8 edge artifacts
Quantize the useful model deterministically, export a versioned edgepack containing shapes/scales/state metadata/checksum, compare FP32 vs INT8 error, and gate `edge_safe` on size and stability.

### 11. ESP32-P4 benchmark firmware
Build fixed-memory inference and DSP benchmark firmware for ESP32-P4. Measure control rates, model footprint, arena usage, p50/p99 inference time, and deadline misses when hardware is available. Do not invent physical-device measurements.

### 12. First playable acceptance
Provide one command that runs native tests, Python tests, deterministic renders, learned/fallback comparison, INT8 validation, GrooveGenome validation, and P4 build status.

## First hardware/UI slice

After the portable core exists, the first instrument integration targets M5Stack Tab5:

- M5GFX rendering
- 720×1280 portrait orientation
- performance/material-field page
- large Matter/Tension XY control
- touch-safe macro controls
- simple playable keyboard/pad strip
- CPU/AI/sample-rate/latency diagnostics
- onboard speaker/headphone output

The UI must remain subordinate to audio deadlines; rendering, allocation, SD I/O, networking, and diagnostics never run in the audio callback.

## Completion gate

The first playable is complete when all host acceptance checks pass and the ESP32-P4 firmware target builds.

A physical ESP32-P4 measurement is required before claiming a realtime embedded performance deadline. If the learned controller fails to beat the fallback, or the shared field fails to demonstrate meaningful harmony-dependent interaction, the result is recorded as a negative research result and the architecture is revised rather than hidden.
