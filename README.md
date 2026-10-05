# GrooveMatter

GrooveMatter is an experimental synthesis architecture centered on **Coupled Perceptual Field Synthesis**: notes excite a shared, stateful resonant field, harmony changes the field's behavior, and compact learned controllers steer deterministic DSP rather than generating every audio sample.

The long-term target is one portable synthesis system spanning desktop, Android, and edge hardware, with **ESP32-P4 / M5Stack Tab5** as the first embedded reference platform.

## Repository status

**Design / implementation-planning stage.**

This repository currently preserves the approved architecture and first-playable implementation direction from the initial sandbox work. The synthesis engine source, training pipeline, and ESP32-P4 firmware are **not yet present** and should not be inferred from the design documents.

## Core system names

- **GrooveMatter** — realtime synthesis engine / instrument
- **GrooveDNA** — learning and capture pipeline
- **GrooveGenome** — portable, versioned patch format
- **GrooveNet-µ** — compact edge controller model family

## Design goals

- 48 kHz causal, low-latency synthesis
- Shared-field harmonic interaction instead of summed complete independent voices
- Deterministic DSP renderer with bounded CPU and memory
- Compact control-rate ML suitable for quantized edge deployment
- ML-free fallback path for every edge-safe patch
- Portable patch semantics across Windows, macOS, Linux, Android, and ESP32-P4
- A touch-first M5Stack Tab5 instrument UI using an M5GFX-style rendering architecture

## Documents

- [Architecture](docs/architecture.md)
- [First Playable Implementation Plan](docs/implementation-plan.md)

## UI direction

High-fidelity UI concepts were produced in the sandbox for the M5Stack Tab5. The current preferred direction is a 720×1280 portrait performance surface with a central material-field visualization, a large Matter/Tension XY control, touch-safe macro controls, live modulation feedback, and compact embedded-system diagnostics.

The production interface is intended to use direct M5GFX-style rendering and the same realtime separation principles proven in OrcSDR: display work never enters the audio/DSP callback.

## First hardware target

The intended first embedded deployment is an **ESP32-P4**, with the M5Stack Tab5 serving as the initial self-contained instrument target and headless P4 boards following as GrooveMatter nodes.

## Current next milestone

Implement the first-playable plan with test-first development:

1. bounded coupled-field DSP core
2. ring/lattice topologies and exciters
3. harmony-aware shared-field engine
4. deterministic render/probe tools
5. GrooveGenome v0
6. GrooveDNA dataset generation
7. GrooveNet-µ32 / µ128 training
8. deterministic INT8 edgepack export
9. portable C++ INT8 reference inference
10. ESP32-P4 benchmark firmware

---

GrooveMatter is research software. No claim of patent novelty is made by this repository.
