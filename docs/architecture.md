# OrcGroove / GrooveMatter Architecture

## Purpose

OrcGroove is the project repository for GrooveMatter, an experimental synthesis architecture built around **Coupled Perceptual Field Synthesis**.

The core idea is deliberately different from conventional polyphonic synthesis: simultaneous notes do not become complete independent synth voices that are simply summed. Notes excite a **shared, stateful resonant field**, and harmonic context changes the behavior of that field.

## System layers

### GrooveMatter
The realtime synthesis engine.

Responsibilities:
- 48 kHz deterministic audio-rate renderer
- 12–24 resonant cells in the first playable milestone
- bounded nonlinear coupling
- ring and lattice/grid topologies
- pluck/impulse and continuous exciters
- five first-generation macros: Matter, Tension, Coupling, Memory, Chaos
- shared-field MIDI excitation
- deterministic stability/reset behavior

### GrooveDNA
The research and training pipeline.

Responsibilities:
- deterministic teacher rendering
- training-data generation
- harmony/perceptual feature extraction
- learned controller training
- model comparison and distillation
- edge quantization experiments

### GrooveNet-µ
A compact control-rate model family for embedded deployment.

GrooveNet-µ does **not** generate every audio sample. It predicts bounded material/control changes at a much lower rate while GrooveMatter DSP renders audio at 48 kHz.

Initial profiles:
- µ32: <= 32k learned parameters
- µ128: <= 128k learned parameters
- µ512: <= 512k learned parameters

The primary embedded quantization target is INT8.

### GrooveGenome
The portable patch/package format.

A GrooveGenome describes:
- field topology
- cell/coupling groups
- exciters
- macro mappings and ranges
- harmony feature schema
- optional GrooveNet-µ model resources
- deterministic ML-free fallback control curves
- stability limits
- checksums and format versions

Patch packages are declarative and contain no arbitrary executable code.

## Realtime safety rules

The realtime renderer must:
- allocate no heap memory in the audio callback
- keep all state bounded
- normalize coupling
- guard maximum field energy
- protect against denormals
- detect NaN/Inf
- recover through deterministic reset
- maintain audio continuity when ML inference is unavailable or skipped

## Musical model

GrooveMatter treats harmony as part of synthesis rather than metadata.

A C note sounding alone may produce a different field state than the same C sounding inside C–E–G. The amount of this interaction is governed by Coupling and learned/material behavior.

At `Coupling = 0`, the first playable implementation should approach isolated/superposable behavior. At nonzero Coupling, harmony-dependent timbral interaction is expected and measurable.

## Platform strategy

The synthesis core is intended to remain portable C++.

Target platforms:
- Windows
- macOS
- Linux
- Android
- ESP32-P4
- reduced ESP32-S3 profile where practical

### First embedded target

ESP32-P4 is the reference edge platform.

The first self-contained instrument target is the **M5Stack Tab5**, using:
- ESP32-P4 compute
- touchscreen UI
- onboard audio path
- M5GFX-style direct rendering
- MIDI/control input
- GrooveNet-µ INT8 inference

Headless Waveshare P4-class boards are intended to follow as network/embedded GrooveMatter nodes.

## UI direction

The Tab5 UI is touch-first and follows the architectural lessons learned from OrcSDR:
- one screen owner
- no display work in audio/DSP callbacks
- bounded incremental updates
- large touch targets
- deterministic screen transitions
- M5GFX primitives rather than heavyweight UI frameworks

The current preferred visual direction is a high-fidelity dark embedded-instrument interface with cyan/green active data and a central material-field visualization, designed for the Tab5's 720×1280 portrait display.

## Security/distribution principle

OrcGroove may support automatic local discovery later, but execution and control remain explicitly authorized.

The project does not use self-propagating installation behavior. Firmware, models, and GrooveGenome resources are intended to be versioned, checksummed, and eventually signed.
