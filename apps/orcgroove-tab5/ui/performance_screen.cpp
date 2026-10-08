#include "performance_screen.hpp"

#if defined(ESP_PLATFORM)

#include <M5Unified.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

namespace orcgroove::tab5 {
namespace {

using orcgroove::ui::Rect;

constexpr uint16_t kBg = 0x0000;
constexpr uint16_t kPanel = 0x0862;
constexpr uint16_t kPanel2 = 0x10C4;
constexpr uint16_t kCyan = 0x05FF;
constexpr uint16_t kBlue = 0x24BF;
constexpr uint16_t kGreen = 0x07E8;
constexpr uint16_t kOrange = 0xFD20;
constexpr uint16_t kPurple = 0xA1BF;
constexpr uint16_t kRed = 0xF800;
constexpr uint16_t kWhite = 0xFFFF;
constexpr uint16_t kMuted = 0x8410;
constexpr uint16_t kGrid = 0x2124;

constexpr std::array<uint16_t, 7> kMacroColors = {
    kCyan, kGreen, kBlue, kOrange, kPurple, 0x07FF, kRed};
constexpr const char* kMacroNames[] = {
    "MATTER", "TENSION", "COUPLING", "MEMORY", "MUTATION", "EXCITE", "CHAOS"};
constexpr const char* kTabs[] = {"PERF", "SYNTH", "MOD", "PATCH", "SYS"};
constexpr const char* kNotes[] = {"C", "D", "E", "F", "G", "A", "B", "C"};

ui::PerformanceLayout g_layout = ui::performance_layout();
bool g_shown = false;

float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }

void text(const char* s, int x, int y, uint16_t color, uint8_t size,
          textdatum_t datum = middle_left) {
  M5.Display.setTextDatum(datum);
  M5.Display.setTextColor(color, kBg);
  M5.Display.setTextSize(size);
  M5.Display.drawString(s, x, y);
}

void panel(const Rect& r, uint16_t edge = kCyan, int radius = 10) {
  M5.Display.fillRoundRect(r.x, r.y, r.w, r.h, radius, kPanel);
  M5.Display.drawRoundRect(r.x, r.y, r.w, r.h, radius, edge);
}

void status_chip(int x, int y, int w, const char* label, bool on) {
  M5.Display.fillRoundRect(x, y, w, 32, 8, kPanel2);
  M5.Display.drawRoundRect(x, y, w, 32, 8, on ? kGreen : kGrid);
  M5.Display.fillCircle(x + 14, y + 16, 4, on ? kGreen : kMuted);
  text(label, x + 25, y + 16, on ? kWhite : kMuted, 1);
}

void draw_header(const ui::PerformanceSnapshot& s) {
  M5.Display.fillRect(0, 0, 720, 68, kBg);
  text("GROOVE", 18, 24, kCyan, 2);
  text("MATTER", 105, 24, kWhite, 2);
  text(s.patch ? s.patch : "INIT MATERIAL", 18, 49, kMuted, 1);
  status_chip(254, 12, 74, "WIFI", s.wifi);
  status_chip(334, 12, 78, "AUDIO", s.audio);
  status_chip(418, 12, 72, "MIDI", s.midi);
  char batt[16];
  if (s.battery_percent >= 0) std::snprintf(batt, sizeof(batt), "%d%%", s.battery_percent);
  else std::snprintf(batt, sizeof(batt), "--%%");
  status_chip(496, 12, 66, batt, s.battery_percent >= 0);
  text(s.time ? s.time : "--:--", 575, 28, kWhite, 2);
  M5.Display.fillRoundRect(632, 12, 76, 34, 10, 0x0193);
  M5.Display.drawRoundRect(632, 12, 76, 34, 10, kBlue);
  text("u32 EDGE", 670, 29, kCyan, 1, middle_center);
  M5.Display.drawFastHLine(12, 67, 696, kGrid);
}

void draw_nav() {
  for (size_t i = 0; i < g_layout.nav_tabs.size(); ++i) {
    const auto& r = g_layout.nav_tabs[i];
    const bool active = i == 0;
    M5.Display.fillRoundRect(r.x, r.y, r.w, r.h, 10, active ? 0x0193 : kPanel);
    M5.Display.drawRoundRect(r.x, r.y, r.w, r.h, 10, active ? kCyan : kGrid);
    text(kTabs[i], r.x + r.w / 2, r.y + r.h / 2, active ? kWhite : kMuted, 2,
         middle_center);
  }
}

void draw_orb(const ui::PerformanceSnapshot& s) {
  const Rect& r = g_layout.orb;
  panel(r, kCyan, 14);
  text("MATERIAL FIELD", r.x + 18, r.y + 24, kCyan, 2);
  M5.Display.fillRoundRect(r.x + r.w - 80, r.y + 12, 62, 28, 8, 0x0200);
  text("LIVE", r.x + r.w - 49, r.y + 26, kGreen, 1, middle_center);

  const int cx = r.x + r.w / 2;
  const int cy = r.y + 196;
  for (int rad : {58, 92, 130}) M5.Display.drawCircle(cx, cy, rad, kGrid);

  for (int y = r.y + 55; y < r.y + r.h - 18; y += 30)
    for (int x = r.x + 24; x < r.x + r.w - 20; x += 30)
      M5.Display.fillCircle(x, y, 1, kGrid);

  const float matter = clamp01(s.macros[0]);
  const float tension = clamp01(s.macros[1]);
  const float coupling = clamp01(s.macros[2]);
  const int core_r = 25 + static_cast<int>(matter * 16.0f);
  M5.Display.fillCircle(cx, cy, core_r + 8, 0x0182);
  M5.Display.fillCircle(cx, cy, core_r, kGreen);
  M5.Display.drawCircle(cx, cy, core_r + 15, kCyan);

  constexpr std::array<float, 7> base_angles = {
      0.15f, 0.95f, 1.85f, 2.75f, 3.65f, 4.55f, 5.45f};
  for (size_t i = 0; i < base_angles.size(); ++i) {
    const float phase =
        base_angles[i] + tension * 0.7f + static_cast<float>(i) * coupling * 0.08f;
    const float radius = 70.0f + static_cast<float>((i % 3) * 28) + coupling * 20.0f;
    const int nx = cx + static_cast<int>(std::cos(phase) * radius);
    const int ny = cy + static_cast<int>(std::sin(phase) * radius * 0.72f);
    const int nr = 6 + static_cast<int>(clamp01(s.macros[i]) * 9.0f);
    M5.Display.drawLine(cx, cy, nx, ny, kGrid);
    M5.Display.fillCircle(nx, ny, nr + 3, kPanel2);
    M5.Display.fillCircle(nx, ny, nr, kMacroColors[i]);
  }
}

void draw_xy(const ui::PerformanceSnapshot& s) {
  const Rect& r = g_layout.xy_pad;
  panel(r, kCyan, 12);
  text("MATTER / TENSION", r.x + 14, r.y + 20, kWhite, 1);
  const int gx = r.x + 16;
  const int gy = r.y + 42;
  const int gw = r.w - 32;
  const int gh = r.h - 58;
  M5.Display.fillRect(gx, gy, gw, gh, kBg);
  M5.Display.drawRect(gx, gy, gw, gh, kGrid);
  for (int i = 1; i < 5; ++i) {
    M5.Display.drawFastVLine(gx + i * gw / 5, gy, gh, kGrid);
    M5.Display.drawFastHLine(gx, gy + i * gh / 5, gw, kGrid);
  }
  const int px = gx + static_cast<int>(clamp01(s.matter_xy) * gw);
  const int py = gy + static_cast<int>((1.0f - clamp01(s.tension_xy)) * gh);
  M5.Display.drawFastVLine(px, gy, gh, 0x03EF);
  M5.Display.drawFastHLine(gx, py, gw, 0x03EF);
  M5.Display.fillCircle(px, py, 18, 0x0182);
  M5.Display.drawCircle(px, py, 16, kCyan);
  M5.Display.fillCircle(px, py, 7, kGreen);
}

void draw_macros(const ui::PerformanceSnapshot& s) {
  panel(g_layout.macro_panel, kBlue, 12);
  for (size_t i = 0; i < g_layout.macro_buttons.size(); ++i) {
    const Rect& r = g_layout.macro_buttons[i];
    const float v = clamp01(s.macros[i]);
    M5.Display.fillRoundRect(r.x, r.y, r.w, r.h, 8, kPanel2);
    M5.Display.drawRoundRect(r.x, r.y, r.w, r.h, 8, kMacroColors[i]);
    text(kMacroNames[i], r.x + 7, r.y + 14, kMuted, 1);
    char value[12];
    std::snprintf(value, sizeof(value), "%.2f", static_cast<double>(v));
    text(value, r.x + 7, r.y + 38, kMacroColors[i], 2);
    const int bx = r.x + r.w - 13;
    const int by = r.y + 14;
    const int bh = r.h - 28;
    M5.Display.fillRoundRect(bx, by, 6, bh, 3, kGrid);
    const int fill = static_cast<int>(v * bh);
    if (fill > 0)
      M5.Display.fillRoundRect(bx, by + bh - fill, 6, fill, 3, kMacroColors[i]);
  }
}

void draw_modulation(const ui::PerformanceSnapshot& s) {
  const Rect& r = g_layout.modulation;
  panel(r, kBlue, 12);
  text("LIVE MODULATION", r.x + 14, r.y + 20, kWhite, 1);
  const int x0 = r.x + 15;
  const int y0 = r.y + 42;
  const int w = r.w - 30;
  const int h = r.h - 56;
  M5.Display.fillRect(x0, y0, w, h, kBg);
  for (int i = 1; i < 5; ++i) M5.Display.drawFastHLine(x0, y0 + i * h / 5, w, kGrid);
  for (int i = 1; i < 8; ++i) M5.Display.drawFastVLine(x0 + i * w / 8, y0, h, kGrid);

  for (size_t curve = 0; curve < 4; ++curve) {
    int px = x0;
    int py = y0 + h / 2;
    for (int x = 0; x < w; x += 5) {
      const float t = x / static_cast<float>(w);
      const float amp = 10.0f + clamp01(s.macros[curve]) * 34.0f;
      const float phase = static_cast<float>(curve) * 0.9f + clamp01(s.macros[6]) * 1.4f;
      const float yy =
          std::sin(t * 6.28318f * (1.0f + curve * 0.35f) + phase) * amp;
      const int nx = x0 + x;
      const int ny = y0 + h / 2 - static_cast<int>(yy);
      if (x) M5.Display.drawLine(px, py, nx, ny, kMacroColors[curve]);
      px = nx;
      py = ny;
    }
  }
}

void metric(int x, int y, int w, const char* label, const char* value, uint16_t color) {
  M5.Display.fillRoundRect(x, y, w, 70, 8, kPanel2);
  text(label, x + 8, y + 18, kMuted, 1);
  text(value, x + 8, y + 46, color, 2);
}

void draw_metrics(const ui::PerformanceSnapshot& s) {
  const Rect& r = g_layout.metrics;
  panel(r, kGrid, 10);
  char cpu[16], ai[16], sr[16], lat[16], voices[16], out[16];
  std::snprintf(cpu, sizeof(cpu), "%.0f%%", static_cast<double>(s.cpu_percent));
  std::snprintf(ai, sizeof(ai), "%.0f", static_cast<double>(s.ai_hz));
  std::snprintf(sr, sizeof(sr), "%.1fk", static_cast<double>(s.sample_rate_hz / 1000.0f));
  std::snprintf(lat, sizeof(lat), "%.1fms", static_cast<double>(s.latency_ms));
  std::snprintf(voices, sizeof(voices), "%d/%d", s.voices, s.max_voices);
  std::snprintf(out, sizeof(out), "%.0fdB", static_cast<double>(s.output_db));

  const int widths[] = {101, 101, 112, 112, 112, 112};
  const char* labels[] = {"CPU", "AI Hz", "SR", "LATENCY", "VOICES", "OUTPUT"};
  const char* values[] = {cpu, ai, sr, lat, voices, out};

  int x = r.x + 8;
  for (int i = 0; i < 6; ++i) {
    metric(x, r.y + 10, widths[i] - 6, labels[i], values[i], i == 5 ? kGreen : kCyan);
    x += widths[i];
  }
}

void draw_keyboard(const ui::PerformanceSnapshot& s) {
  panel(g_layout.note_strip, kCyan, 12);

  const Rect& up = g_layout.octave_up;
  M5.Display.fillRoundRect(up.x, up.y, up.w, up.h, 8, kPanel2);
  M5.Display.drawRoundRect(up.x, up.y, up.w, up.h, 8, kGrid);
  M5.Display.fillTriangle(up.x + 30, up.y + 20, up.x + 16, up.y + 42,
                          up.x + 44, up.y + 42, kCyan);
  char oct[12];
  std::snprintf(oct, sizeof(oct), "OCT %+d", s.octave);
  text(oct, up.x + 30, up.y + 74, kWhite, 1, middle_center);

  const Rect& down = g_layout.octave_down;
  M5.Display.fillRoundRect(down.x, down.y, down.w, down.h, 8, kPanel2);
  M5.Display.drawRoundRect(down.x, down.y, down.w, down.h, 8, kGrid);
  M5.Display.fillTriangle(down.x + 16, down.y + 56, down.x + 44, down.y + 56,
                          down.x + 30, down.y + 78, kCyan);

  for (size_t i = 0; i < g_layout.note_keys.size(); ++i) {
    const Rect& k = g_layout.note_keys[i];
    const uint16_t fill = s.held_notes[i] ? 0x04B4 : kPanel2;
    const uint16_t edge = s.held_notes[i] ? kGreen : kBlue;
    M5.Display.fillRoundRect(k.x, k.y, k.w, k.h, 8, fill);
    M5.Display.drawRoundRect(k.x, k.y, k.w, k.h, 8, edge);
    text(kNotes[i], k.x + k.w / 2, k.y + k.h - 28,
         s.held_notes[i] ? kWhite : kCyan, 2, middle_center);
  }

  const Rect& hold = g_layout.hold_button;
  M5.Display.fillRoundRect(hold.x, hold.y, hold.w, hold.h, 8,
                           s.hold ? 0x0300 : kPanel2);
  M5.Display.drawRoundRect(hold.x, hold.y, hold.w, hold.h, 8,
                           s.hold ? kGreen : kGrid);
  text("HOLD", hold.x + hold.w / 2, hold.y + hold.h / 2,
       s.hold ? kGreen : kMuted, 1, middle_center);
}

void draw_full(const ui::PerformanceSnapshot& s) {
  M5.Display.startWrite();
  M5.Display.fillScreen(kBg);
  draw_header(s);
  draw_nav();
  draw_orb(s);
  draw_xy(s);
  draw_macros(s);
  draw_modulation(s);
  draw_metrics(s);
  draw_keyboard(s);
  M5.Display.drawRoundRect(4, 4, 712, 1272, 14, kCyan);
  M5.Display.endWrite();
}

}  // namespace

bool ensure_portrait_display() {
  for (uint8_t rotation : {uint8_t{0}, uint8_t{2}}) {
    M5.Display.setRotation(rotation);
    if (M5.Display.width() == ui::kScreenWidth &&
        M5.Display.height() == ui::kScreenHeight)
      return true;
  }
  return false;
}

void performance_enter(const ui::PerformanceSnapshot& snapshot) {
  g_shown = true;
  draw_full(snapshot);
}

void performance_update(const ui::PerformanceSnapshot& snapshot) {
  if (!g_shown) return;
  M5.Display.startWrite();
  draw_header(snapshot);
  draw_orb(snapshot);
  draw_xy(snapshot);
  draw_macros(snapshot);
  draw_modulation(snapshot);
  draw_metrics(snapshot);
  draw_keyboard(snapshot);
  M5.Display.endWrite();
}

ui::Action performance_hit_test(int32_t x, int32_t y) {
  return ui::hit_test(g_layout, x, y);
}

}  // namespace orcgroove::tab5

#endif
