#include "field.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {
void require(bool cond, const char* message) {
  if (!cond) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

void test_determinism() {
  groove::Field a(24, 48000.0f);
  groove::Field b(24, 48000.0f);
  a.excite(4, 0.75f);
  b.excite(4, 0.75f);
  for (int i = 0; i < 4096; ++i) {
    const float ya = a.process_sample();
    const float yb = b.process_sample();
    require(ya == yb, "identical initial state and excitation must render identically");
  }
}

void test_decay() {
  groove::Field f(24, 48000.0f);
  f.excite(3, 1.0f);
  float early = 0.0f;
  float late = 0.0f;
  for (int i = 0; i < 24000; ++i) {
    const float y = std::fabs(f.process_sample());
    if (i < 2000) early += y;
    if (i >= 22000) late += y;
  }
  require(early > 0.001f, "excitation must produce output");
  require(late < early * 0.35f, "field must decay without continued excitation");
}

void test_extreme_controls_bounded() {
  groove::Field f(24, 48000.0f);
  groove::FieldParams p{};
  p.damping = 0.0001f;
  p.stiffness = 0.999f;
  p.coupling = 1.0f;
  p.nonlinearity = 1.0f;
  f.set_params(p);
  for (int n = 0; n < 24; ++n) f.excite(n, 4.0f);
  for (int i = 0; i < 20000; ++i) {
    const float y = f.process_sample();
    require(std::isfinite(y), "extreme controls must not produce NaN/Inf");
    require(std::fabs(y) <= 1.0001f, "output must remain bounded");
    require(f.energy() <= groove::Field::kMaxEnergy + 1e-3f, "energy guard must remain active");
  }
}

void test_invalid_state_recovery() {
  groove::Field f(24, 48000.0f);
  f.excite(2, 0.5f);
  f.debug_corrupt_state();
  (void)f.process_sample();
  require(f.reset_count() == 1, "invalid state must force deterministic reset");
  require(std::isfinite(f.energy()) && f.energy() == 0.0f, "reset must clear field energy");
}
}

int main() {
  test_determinism();
  test_decay();
  test_extreme_controls_bounded();
  test_invalid_state_recovery();
  std::cout << "field tests passed\n";
  return 0;
}
