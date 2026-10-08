#include "ui/tab5_ui.hpp"

#include <algorithm>

namespace orcgroove::ui {

bool inside(const Rect& r, int x, int y) {
  return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

PerformanceLayout performance_layout() {
  PerformanceLayout l{};
  l.header = {0, 0, 720, 68};
  l.nav = {12, 74, 696, 56};
  constexpr int tab_gap = 6;
  constexpr int tab_w = (696 - tab_gap * 4) / 5;
  for (int i = 0; i < 5; ++i)
    l.nav_tabs[static_cast<size_t>(i)] = {12 + i * (tab_w + tab_gap), 74, tab_w, 56};

  l.orb = {12, 136, 696, 350};
  l.xy_pad = {12, 492, 330, 252};
  l.macro_panel = {348, 492, 360, 252};

  constexpr int macro_gap = 6;
  constexpr int macro_margin = 8;
  constexpr int macro_w = (360 - macro_margin * 2 - macro_gap * 3) / 4;
  constexpr int macro_h = (252 - macro_margin * 2 - macro_gap) / 2;
  for (int i = 0; i < 7; ++i) {
    const int col = i % 4;
    const int row = i / 4;
    l.macro_buttons[static_cast<size_t>(i)] = {
        348 + macro_margin + col * (macro_w + macro_gap),
        492 + macro_margin + row * (macro_h + macro_gap), macro_w, macro_h};
  }

  l.modulation = {12, 750, 696, 190};
  l.metrics = {12, 946, 696, 90};
  l.note_strip = {12, 1042, 696, 232};

  l.octave_up = {20, 1050, 60, 100};
  l.octave_down = {20, 1158, 60, 100};
  l.hold_button = {628, 1050, 72, 208};

  constexpr int key_x = 86;
  constexpr int key_y = 1050;
  constexpr int key_gap = 4;
  constexpr int key_w = 63;
  constexpr int key_h = 208;
  for (int i = 0; i < 8; ++i)
    l.note_keys[static_cast<size_t>(i)] = {key_x + i * (key_w + key_gap), key_y, key_w, key_h};
  return l;
}

Action hit_test(const PerformanceLayout& l, int x, int y) {
  for (size_t i = 0; i < l.nav_tabs.size(); ++i)
    if (inside(l.nav_tabs[i], x, y)) return {ActionKind::Navigate, static_cast<int>(i)};
  if (inside(l.xy_pad, x, y)) return {ActionKind::XY, 0};
  for (size_t i = 0; i < l.macro_buttons.size(); ++i)
    if (inside(l.macro_buttons[i], x, y)) return {ActionKind::MacroSelect, static_cast<int>(i)};
  if (inside(l.octave_up, x, y)) return {ActionKind::OctaveUp, 0};
  if (inside(l.octave_down, x, y)) return {ActionKind::OctaveDown, 0};
  for (size_t i = 0; i < l.note_keys.size(); ++i)
    if (inside(l.note_keys[i], x, y)) return {ActionKind::Note, static_cast<int>(i)};
  if (inside(l.hold_button, x, y)) return {ActionKind::HoldToggle, 0};
  return {};
}

std::pair<float, float> xy_value(const PerformanceLayout& l, int x, int y) {
  const float fx = std::clamp((x - l.xy_pad.x) / static_cast<float>(l.xy_pad.w), 0.0f, 1.0f);
  const float fy = 1.0f - std::clamp((y - l.xy_pad.y) / static_cast<float>(l.xy_pad.h), 0.0f, 1.0f);
  return {fx, fy};
}

}  // namespace orcgroove::ui
