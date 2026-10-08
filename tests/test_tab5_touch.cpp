#include "ui/tab5_touch.hpp"

#include <cmath>
#include <iostream>

using namespace orcgroove::ui;
#define CHECK(expr) do { if (!(expr)) { std::cerr << "CHECK failed: " #expr << " at " << __LINE__ << "\n"; return 1; } } while (0)

static TouchPoint center(uint8_t id, const Rect& r, bool down = true) {
  return {id, r.x + r.w / 2, r.y + r.h / 2, down};
}

int main() {
  const auto l = performance_layout();
  TouchRouter router(l);

  auto e0 = router.update(center(1, l.note_keys[0], true));
  auto e1 = router.update(center(2, l.note_keys[2], true));
  CHECK(e0.kind == TouchEventKind::NoteOn && e0.index == 0);
  CHECK(e1.kind == TouchEventKind::NoteOn && e1.index == 2);
  CHECK(router.active_note_count() == 2);
  CHECK(router.update(center(1, l.note_keys[0], true)).kind == TouchEventKind::None);

  auto up0 = center(1, l.note_keys[0], false);
  auto off0 = router.update(up0);
  CHECK(off0.kind == TouchEventKind::NoteOff && off0.index == 0);
  CHECK(router.active_note_count() == 1);

  TouchPoint xy{7, l.xy_pad.x, l.xy_pad.y, true};
  auto xy0 = router.update(xy);
  CHECK(xy0.kind == TouchEventKind::SetXY);
  CHECK(std::abs(xy0.x - 0.0f) < 1e-6f && std::abs(xy0.y - 1.0f) < 1e-6f);
  xy.x = l.xy_pad.x + l.xy_pad.w;
  xy.y = l.xy_pad.y + l.xy_pad.h;
  auto xy1 = router.update(xy);
  CHECK(xy1.kind == TouchEventKind::SetXY);
  CHECK(std::abs(xy1.x - 1.0f) < 1e-6f && std::abs(xy1.y - 0.0f) < 1e-6f);
  CHECK(router.active_note_count() == 1);

  auto slide_from = center(9, l.note_keys[4], true);
  CHECK(router.update(slide_from).kind == TouchEventKind::NoteOn);
  auto slide_to = center(9, l.note_keys[5], true);
  auto slide = router.update(slide_to);
  CHECK(slide.kind == TouchEventKind::NoteChange);
  CHECK(slide.index == 5 && slide.previous_index == 4);

  CHECK(router.update(center(20, l.octave_up, true)).kind == TouchEventKind::OctaveUp);
  CHECK(router.update(center(21, l.hold_button, true)).kind == TouchEventKind::HoldToggle);

  std::cout << "tab5 multitouch tests passed\n";
  return 0;
}
