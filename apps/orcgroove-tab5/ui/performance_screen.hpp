#pragma once

#include "ui/tab5_state.hpp"
#include "ui/tab5_ui.hpp"

namespace orcgroove::tab5 {

// Selects a 720x1280 portrait rotation and returns true when the panel reports
// the expected logical dimensions.
bool ensure_portrait_display();

// Full entry draw. Use when opening Performance or after a display reset.
void performance_enter(const ui::PerformanceSnapshot& snapshot);

// Bounded dynamic redraw. Static navigation chrome is not rebuilt.
void performance_update(const ui::PerformanceSnapshot& snapshot);

// Shared geometry hit test for touch routing.
ui::Action performance_hit_test(int32_t x, int32_t y);

}  // namespace orcgroove::tab5
