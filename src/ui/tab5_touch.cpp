#include "ui/tab5_touch.hpp"

namespace orcgroove::ui {

TouchRouter::Contact* TouchRouter::find(uint8_t id) {
  for (auto& c : contacts_)
    if (c.active && c.id == id) return &c;
  return nullptr;
}

TouchRouter::Contact* TouchRouter::acquire(uint8_t id) {
  for (auto& c : contacts_) {
    if (!c.active) {
      c = {};
      c.active = true;
      c.id = id;
      return &c;
    }
  }
  return nullptr;
}

TouchEvent TouchRouter::begin(Contact& c, const TouchPoint& point) {
  const Action a = hit_test(layout_, point.x, point.y);
  switch (a.kind) {
    case ActionKind::XY: {
      c.role = Role::XY;
      const auto [x, y] = xy_value(layout_, point.x, point.y);
      return {TouchEventKind::SetXY, 0, -1, x, y};
    }
    case ActionKind::Note:
      c.role = Role::Note;
      c.index = a.index;
      return {TouchEventKind::NoteOn, a.index};
    case ActionKind::Navigate:
      c.role = Role::Utility;
      return {TouchEventKind::Navigate, a.index};
    case ActionKind::MacroSelect:
      c.role = Role::Utility;
      return {TouchEventKind::MacroSelect, a.index};
    case ActionKind::HoldToggle:
      c.role = Role::Utility;
      return {TouchEventKind::HoldToggle};
    case ActionKind::OctaveUp:
      c.role = Role::Utility;
      return {TouchEventKind::OctaveUp};
    case ActionKind::OctaveDown:
      c.role = Role::Utility;
      return {TouchEventKind::OctaveDown};
    case ActionKind::None:
      c.role = Role::Utility;
      return {};
  }
  return {};
}

TouchEvent TouchRouter::update(const TouchPoint& point) {
  Contact* c = find(point.id);
  if (!point.down) {
    if (!c) return {};
    TouchEvent out{};
    if (c->role == Role::Note) out = {TouchEventKind::NoteOff, c->index};
    *c = {};
    return out;
  }

  if (!c) {
    c = acquire(point.id);
    if (!c) return {};
    return begin(*c, point);
  }

  if (c->role == Role::XY) {
    const auto [x, y] = xy_value(layout_, point.x, point.y);
    return {TouchEventKind::SetXY, 0, -1, x, y};
  }

  if (c->role == Role::Note) {
    const Action a = hit_test(layout_, point.x, point.y);
    if (a.kind == ActionKind::Note && a.index != c->index) {
      const int previous = c->index;
      c->index = a.index;
      return {TouchEventKind::NoteChange, a.index, previous};
    }
    return {};
  }

  return {};
}

size_t TouchRouter::active_note_count() const {
  size_t count = 0;
  for (const auto& c : contacts_)
    if (c.active && c.role == Role::Note) ++count;
  return count;
}

void TouchRouter::reset() { contacts_ = {}; }

}  // namespace orcgroove::ui
