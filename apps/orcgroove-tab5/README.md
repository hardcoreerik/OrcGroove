# OrcGroove Tab5 Frontend

This is the first device frontend for GrooveMatter.

Current slice:

- ESP32-P4 / M5Stack Tab5
- 720×1280 portrait UI
- M5Unified/M5GFX rendering
- Performance page
- 10-contact fixed-capacity touch router
- chord-capable note pad strip
- Matter/Tension XY control
- seven macro cards
- 30 Hz UI redraw cap

The UI does **not** synthesize audio yet. Note and macro events are intentionally semantic and ready to bind to the GrooveMatter realtime engine in the next integration step.

## Build environment

The component names follow the same ESP-IDF/M5Unified convention used by OrcSDR:

- `m5unified`
- `m5gfx`

From an ESP-IDF environment where those components are available:

```bash
cd apps/orcgroove-tab5
idf.py set-target esp32p4
idf.py build
idf.py -p <PORT> flash monitor
```

Do not treat a successful host UI test as a Tab5 firmware build. Hardware display/touch/audio timing must be verified on a real Tab5.
