#include "ui/tab5_ui.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace orcgroove::ui;

#define CHECK(expr) do { if (!(expr)) { std::cerr << "CHECK failed: " #expr << " at " << __FILE__ << ":" << __LINE__ << "\n"; return 1; } } while (0)

static bool overlaps(const Rect& a, const Rect& b) {
  return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

int main() {
  const auto l = performance_layout();
  CHECK(kScreenWidth == 720);
  CHECK(kScreenHeight == 1280);
  CHECK(l.header.x == 0 && l.header.y == 0 && l.header.w == 720);
  CHECK(l.note_strip.y + l.note_strip.h <= kScreenHeight);

  const Rect major[] = {l.header, l.nav, l.orb, l.xy_pad, l.macro_panel,
                        l.modulation, l.metrics, l.note_strip};
  for (const auto& r : major) {
    CHECK(r.x >= 0 && r.y >= 0 && r.w > 0 && r.h > 0);
    CHECK(r.x + r.w <= kScreenWidth);
    CHECK(r.y + r.h <= kScreenHeight);
  }
  for (size_t i = 0; i < std::size(major); ++i)
    for (size_t j = i + 1; j < std::size(major); ++j)
      CHECK(!overlaps(major[i], major[j]));

  for (const auto& r : l.nav_tabs) CHECK(r.w >= kMinTouch && r.h >= kMinTouch);
  for (const auto& r : l.macro_buttons) CHECK(r.w >= kMinTouch && r.h >= kMinTouch);
  for (const auto& r : l.note_keys) CHECK(r.w >= kMinTouch && r.h >= kMinTouch);
  CHECK(l.hold_button.w >= kMinTouch && l.hold_button.h >= kMinTouch);
  CHECK(l.octave_up.w >= kMinTouch && l.octave_up.h >= kMinTouch);
  CHECK(l.octave_down.w >= kMinTouch && l.octave_down.h >= kMinTouch);

  for (size_t i = 0; i < l.nav_tabs.size(); ++i) {
    const auto& r = l.nav_tabs[i];
    const auto a = hit_test(l, r.x + r.w / 2, r.y + r.h / 2);
    CHECK(a.kind == ActionKind::Navigate);
    CHECK(static_cast<size_t>(a.index) == i);
  }

  for (size_t i = 0; i < l.macro_buttons.size(); ++i) {
    const auto& r = l.macro_buttons[i];
    const auto a = hit_test(l, r.x + r.w / 2, r.y + r.h / 2);
    CHECK(a.kind == ActionKind::MacroSelect);
    CHECK(static_cast<size_t>(a.index) == i);
  }

  auto xy = xy_value(l, l.xy_pad.x, l.xy_pad.y);
  CHECK(std::abs(xy.first - 0.0f) < 1e-6f);
  CHECK(std::abs(xy.second - 1.0f) < 1e-6f);
  xy = xy_value(l, l.xy_pad.x + l.xy_pad.w, l.xy_pad.y + l.xy_pad.h);
  CHECK(std::abs(xy.first - 1.0f) < 1e-6f);
  CHECK(std::abs(xy.second - 0.0f) < 1e-6f);
  xy = xy_value(l, -500, 5000);
  CHECK(xy.first == 0.0f && xy.second == 0.0f);

  for (size_t i = 0; i < l.note_keys.size(); ++i) {
    const auto& r = l.note_keys[i];
    const auto a = hit_test(l, r.x + r.w / 2, r.y + r.h / 2);
    CHECK(a.kind == ActionKind::Note);
    CHECK(static_cast<size_t>(a.index) == i);
  }
  CHECK(hit_test(l, l.hold_button.x + 10, l.hold_button.y + 10).kind == ActionKind::HoldToggle);
  CHECK(hit_test(l, l.octave_up.x + 10, l.octave_up.y + 10).kind == ActionKind::OctaveUp);
  CHECK(hit_test(l, l.octave_down.x + 10, l.octave_down.y + 10).kind == ActionKind::OctaveDown);

  const auto none = hit_test(l, 5, 1275);
  CHECK(none.kind == ActionKind::None);

  std::cout << "tab5 ui layout tests passed\n";
  return 0;
}
