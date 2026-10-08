#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace orcgroove::ui {

constexpr int kScreenWidth = 720;
constexpr int kScreenHeight = 1280;
constexpr int kMinTouch = 48;

enum class Screen : uint8_t { Performance, Synth, Mod, Patch, System };
enum class Macro : uint8_t { Matter, Tension, Coupling, Memory, Mutation, Excitation, Chaos };
enum class ActionKind : uint8_t {
  None,
  Navigate,
  XY,
  MacroSelect,
  Note,
  HoldToggle,
  OctaveUp,
  OctaveDown,
};

struct Rect {
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
};

struct Action {
  ActionKind kind = ActionKind::None;
  int index = -1;
};

struct PerformanceLayout {
  Rect header;
  Rect nav;
  std::array<Rect, 5> nav_tabs;
  Rect orb;
  Rect xy_pad;
  Rect macro_panel;
  std::array<Rect, 7> macro_buttons;
  Rect modulation;
  Rect metrics;
  Rect note_strip;
  Rect octave_up;
  Rect octave_down;
  std::array<Rect, 8> note_keys;
  Rect hold_button;
};

PerformanceLayout performance_layout();
Action hit_test(const PerformanceLayout& layout, int x, int y);
std::pair<float, float> xy_value(const PerformanceLayout& layout, int x, int y);
bool inside(const Rect& r, int x, int y);

}  // namespace orcgroove::ui
