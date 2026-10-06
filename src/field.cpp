#include "field.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace groove {
namespace {
float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
}

Field::Field(std::size_t cells, float sample_rate)
    : cell_count_(std::clamp(cells, kMinCells, kMaxCells)),
      sample_rate_(sample_rate > 1000.0f ? sample_rate : 48000.0f) {
  reset();
}

void Field::reset() {
  for (auto& c : cells_) c = {};
  next_x_.fill(0.0f);
  next_v_.fill(0.0f);
  energy_ = 0.0f;
}

void Field::set_params(const FieldParams& params) {
  params_.damping = std::clamp(params.damping, 0.0001f, 0.25f);
  params_.stiffness = clamp01(params.stiffness);
  params_.coupling = clamp01(params.coupling);
  params_.nonlinearity = clamp01(params.nonlinearity);
}

void Field::excite(std::size_t cell, float amount) {
  if (cell >= cell_count_ || !std::isfinite(amount)) return;
  cells_[cell].v = std::clamp(cells_[cell].v + amount * 0.08f, -2.0f, 2.0f);
  recompute_energy_and_guard();
}

bool Field::state_valid() const {
  if (!std::isfinite(energy_)) return false;
  for (std::size_t i = 0; i < cell_count_; ++i) {
    const auto& c = cells_[i];
    if (!std::isfinite(c.x) || !std::isfinite(c.v) || !std::isfinite(c.memory)) return false;
  }
  return true;
}

void Field::recompute_energy_and_guard() {
  float e = 0.0f;
  for (std::size_t i = 0; i < cell_count_; ++i) {
    const auto& c = cells_[i];
    e += c.x * c.x + c.v * c.v + 0.25f * c.memory * c.memory;
  }
  if (!std::isfinite(e)) {
    energy_ = e;
    return;
  }
  if (e > kMaxEnergy && e > 0.0f) {
    const float scale = std::sqrt(kMaxEnergy / e);
    for (std::size_t i = 0; i < cell_count_; ++i) {
      cells_[i].x *= scale;
      cells_[i].v *= scale;
      cells_[i].memory *= scale;
    }
    e = kMaxEnergy;
  }
  energy_ = e;
}

float Field::process_sample() {
  if (!state_valid()) {
    reset();
    ++reset_count_;
    return 0.0f;
  }

  const float spring = 0.0015f + params_.stiffness * 0.0185f;
  const float coupling = params_.coupling * 0.012f;
  const float damping = params_.damping;
  const float nonlinear = params_.nonlinearity * 0.015f;

  for (std::size_t i = 0; i < cell_count_; ++i) {
    const std::size_t left = (i + cell_count_ - 1) % cell_count_;
    const std::size_t right = (i + 1) % cell_count_;
    const float neighbor = 0.5f * (cells_[left].x + cells_[right].x);
    const float coupling_force = coupling * (neighbor - cells_[i].x);
    const float restoring = -spring * cells_[i].x;
    const float shaped = -nonlinear * std::tanh(cells_[i].x + 0.25f * cells_[i].memory);
    float v = cells_[i].v + restoring + coupling_force + shaped;
    v *= (1.0f - damping);
    float x = cells_[i].x + v;
    if (!std::isfinite(v) || !std::isfinite(x)) {
      reset();
      ++reset_count_;
      return 0.0f;
    }
    next_v_[i] = std::clamp(v, -4.0f, 4.0f);
    next_x_[i] = std::clamp(x, -4.0f, 4.0f);
  }

  float mix = 0.0f;
  for (std::size_t i = 0; i < cell_count_; ++i) {
    cells_[i].v = next_v_[i];
    cells_[i].x = next_x_[i];
    cells_[i].memory = 0.995f * cells_[i].memory + 0.005f * cells_[i].x;
    mix += cells_[i].x;
  }
  recompute_energy_and_guard();
  if (!state_valid()) {
    reset();
    ++reset_count_;
    return 0.0f;
  }
  const float normalized = mix / static_cast<float>(cell_count_);
  return std::clamp(normalized, -1.0f, 1.0f);
}

void Field::debug_corrupt_state() {
  cells_[0].x = std::numeric_limits<float>::quiet_NaN();
}

}  // namespace groove
