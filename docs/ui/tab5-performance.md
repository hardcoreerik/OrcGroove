# Tab5 Performance UI Contract

## Target

The first GrooveMatter instrument front end is the M5Stack Tab5 in portrait orientation. The logical UI canvas is **720 × 1280**. Rendering uses M5GFX/M5Unified primitives; the DSP/audio callback never performs display work, allocation, logging, storage, or networking.

The design target is high fidelity, not desktop density. A 5-inch display must remain playable by touch.

## Primary screen geometry

| Region | x | y | w | h | Purpose |
|---|---:|---:|---:|---:|---|
| Header | 0 | 0 | 720 | 68 | Brand, patch, Wi-Fi/audio/MIDI/battery/time, edge model |
| Navigation | 12 | 74 | 696 | 56 | PERF / SYNTH / MOD / PATCH / SYS |
| Material Field | 12 | 136 | 696 | 350 | Main animated shared-field / macro-orb visual |
| Matter/Tension XY | 12 | 492 | 330 | 252 | Primary continuous touch expression |
| Macro panel | 348 | 492 | 360 | 252 | Seven macro cards |
| Live modulation | 12 | 750 | 696 | 190 | Bounded visual motion feedback |
| Metrics | 12 | 946 | 696 | 90 | CPU, AI rate, sample rate, latency, voices, output |
| Note strip | 12 | 1042 | 696 | 232 | Octave controls, 8 playable pads, hold |

All primary interactive targets are at least 48 px in both dimensions.

## Performance interactions

- Dragging inside Matter/Tension maps X to Matter 0..1 and inverted Y to Tension 0..1.
- Macro cards open/focus one of Matter, Tension, Coupling, Memory, Mutation, Excitation, Chaos.
- Eight note pads are momentary unless Hold is active.
- Octave +/- are dedicated targets and never share a hit region with notes.
- Top navigation changes screen; PERF is the startup screen.
- No hidden gesture is required for primary playability.

## Visual system

- Background: black / near-black.
- Panels: blue-black RGB565 fills.
- Cyan: framing, primary data, active navigation.
- Green: live/healthy/held state.
- Blue: coupling and secondary structural state.
- Orange: memory / caution.
- Purple: mutation.
- Red: chaos / fault emphasis.
- Text stays white or light gray; colored text is reserved for state/value emphasis.

The material-field hero visual uses only M5GFX-friendly primitives: circles, lines, dots, arcs/round-rects, and flat fills. No runtime PNG compositing is required for the core screen.

## Render budget strategy

`performance_enter()` may draw the full screen during navigation. Runtime updates are constrained to the Performance-owned regions. The first prototype redraws each dynamic region as a bounded block; follow-up profiling may split the orb, XY handle, meters, and held-note pads into smaller dirty rectangles. Full-screen high-rate copies are explicitly out of scope until measured on Tab5 hardware.

## UI state boundary

The renderer consumes a fixed `PerformanceSnapshot`. It does not own synth state. Audio/DSP publishes bounded snapshot values; UI may miss intermediate states without affecting sound. Touch routing produces semantic actions and normalized XY values for a controller layer to apply to GrooveMatter.

## First-device acceptance

On a real Tab5:

1. logical dimensions report 720×1280 in the selected portrait rotation;
2. all five nav targets and all seven macro targets register reliably;
3. continuous XY dragging reaches 0/1 corners and is stable under finger movement;
4. eight note pads can be played without adjacent accidental triggers;
5. display updates do not cause audible speaker/headphone underruns;
6. field animation and meters remain legible at normal handheld viewing distance;
7. CPU/latency readouts come from measured runtime data, never placeholders in production builds.
