# GrooveMatter UI Concepts

High-fidelity GrooveMatter/Tab5 UI renders were created during the initial sandbox design pass. They are **design targets, not screenshots of working firmware**.

The current preferred direction is the 720×1280 portrait performance layout, with:

- a central material-field / macro-orb visualization
- a large Matter/Tension XY touch surface
- touch-safe Matter, Tension, Coupling, Memory, Mutation, Excitation, and Chaos controls
- simplified live modulation feedback
- CPU / AI rate / sample-rate / latency / voice / output diagnostics
- a compact playable key/pad strip

The production UI is intended to be implemented with M5GFX-compatible primitives, bounded incremental drawing, large touch targets, and strict separation from the audio/DSP callback.

The original generated raster concepts remain preserved in the project sandbox; source development in this repository should treat this document as the UI contract until those binary assets are published here.
