#include <M5Unified.h>

#include "performance_screen.hpp"
#include "ui/tab5_touch.hpp"
#include "ui/tab5_ui.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace {

orcgroove::ui::PerformanceSnapshot g_snapshot{};
orcgroove::ui::TouchRouter g_touch(orcgroove::ui::performance_layout());
bool g_dirty = true;
uint32_t g_last_draw_ms = 0;
constexpr uint32_t kUiFrameMs = 33;  // cap UI work near 30 Hz

void apply_event(const orcgroove::ui::TouchEvent& event) {
  using orcgroove::ui::TouchEventKind;

  switch (event.kind) {
    case TouchEventKind::SetXY:
      g_snapshot.matter_xy = event.x;
      g_snapshot.tension_xy = event.y;
      g_snapshot.macros[0] = event.x;
      g_snapshot.macros[1] = event.y;
      g_dirty = true;
      break;

    case TouchEventKind::NoteOn:
      if (event.index >= 0 && event.index < static_cast<int>(g_snapshot.held_notes.size())) {
        g_snapshot.held_notes[static_cast<size_t>(event.index)] = true;
        ++g_snapshot.voices;
        g_snapshot.voices = std::min(g_snapshot.voices, g_snapshot.max_voices);
        g_dirty = true;
      }
      break;

    case TouchEventKind::NoteOff:
      if (!g_snapshot.hold && event.index >= 0 &&
          event.index < static_cast<int>(g_snapshot.held_notes.size())) {
        g_snapshot.held_notes[static_cast<size_t>(event.index)] = false;
        g_snapshot.voices = std::max(0, g_snapshot.voices - 1);
        g_dirty = true;
      }
      break;

    case TouchEventKind::NoteChange:
      if (event.previous_index >= 0 &&
          event.previous_index < static_cast<int>(g_snapshot.held_notes.size()))
        g_snapshot.held_notes[static_cast<size_t>(event.previous_index)] = false;
      if (event.index >= 0 && event.index < static_cast<int>(g_snapshot.held_notes.size()))
        g_snapshot.held_notes[static_cast<size_t>(event.index)] = true;
      g_dirty = true;
      break;

    case TouchEventKind::HoldToggle:
      g_snapshot.hold = !g_snapshot.hold;
      if (!g_snapshot.hold) {
        for (auto& note : g_snapshot.held_notes) note = false;
        g_snapshot.voices = 0;
      }
      g_dirty = true;
      break;

    case TouchEventKind::OctaveUp:
      g_snapshot.octave = std::min(g_snapshot.octave + 1, 4);
      g_dirty = true;
      break;

    case TouchEventKind::OctaveDown:
      g_snapshot.octave = std::max(g_snapshot.octave - 1, -4);
      g_dirty = true;
      break;

    case TouchEventKind::Navigate:
    case TouchEventKind::MacroSelect:
      // Screen routing and focused macro editors land in the next UI slice.
      break;

    case TouchEventKind::None:
      break;
  }
}

void setup_snapshot() {
  g_snapshot.patch = "UI LAB";
  g_snapshot.sample_rate_hz = 0.0f;  // do not display invented audio metrics
  g_snapshot.latency_ms = 0.0f;
  g_snapshot.ai_hz = 0.0f;
  g_snapshot.cpu_percent = 0.0f;
  g_snapshot.max_voices = 8;
  g_snapshot.battery_percent = M5.Power.getBatteryLevel();
  g_snapshot.time = "--:--";
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  if (!orcgroove::tab5::ensure_portrait_display()) {
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_RED);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(20, 20);
    M5.Display.print("GrooveMatter requires 720x1280 portrait mode");
    return;
  }

  setup_snapshot();
  orcgroove::tab5::performance_enter(g_snapshot);
  g_last_draw_ms = millis();
}

void loop() {
  M5.update();

  const auto count = M5.Touch.getCount();
  for (size_t i = 0; i < count; ++i) {
    const auto t = M5.Touch.getDetail(i);
    const bool down = !t.wasReleased();
    const auto event = g_touch.update(
        {static_cast<uint8_t>(t.id), static_cast<int>(t.x), static_cast<int>(t.y), down});
    apply_event(event);
  }

  const uint32_t now = millis();
  if (g_dirty && now - g_last_draw_ms >= kUiFrameMs) {
    g_snapshot.battery_percent = M5.Power.getBatteryLevel();
    orcgroove::tab5::performance_update(g_snapshot);
    g_last_draw_ms = now;
    g_dirty = false;
  }

  M5.delay(1);
}

}  // namespace

extern "C" void app_main(void) {
  setup();
  for (;;) loop();
}
