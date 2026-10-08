#pragma once

#include "ui/tab5_ui.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace orcgroove::ui {

struct TouchPoint {
  uint8_t id = 0;
  int x = 0;
  int y = 0;
  bool down = false;
};

enum class TouchEventKind : uint8_t {
  None,
  SetXY,
  NoteOn,
  NoteOff,
  NoteChange,
  Navigate,
  MacroSelect,
  HoldToggle,
  OctaveUp,
  OctaveDown,
};

struct TouchEvent {
  TouchEventKind kind = TouchEventKind::None;
  int index = -1;
  int previous_index = -1;
  float x = 0.0f;
  float y = 0.0f;
};

class TouchRouter {
 public:
  explicit TouchRouter(const PerformanceLayout& layout) : layout_(layout) {}

  TouchEvent update(const TouchPoint& point);
  size_t active_note_count() const;
  void reset();

 private:
  enum class Role : uint8_t { None, Note, XY, Utility };
  struct Contact {
    bool active = false;
    uint8_t id = 0;
    Role role = Role::None;
    int index = -1;
  };

  Contact* find(uint8_t id);
  Contact* acquire(uint8_t id);
  TouchEvent begin(Contact& c, const TouchPoint& point);

  PerformanceLayout layout_;
  std::array<Contact, 10> contacts_{};
};

}  // namespace orcgroove::ui
