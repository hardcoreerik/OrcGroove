#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace groove {

struct FieldParams {
  float damping = 0.0125f;
  float stiffness = 0.35f;
  float coupling = 0.10f;
  float nonlinearity = 0.15f;
};

class Field {
 public:
  static constexpr std::size_t kMinCells = 12;
  static constexpr std::size_t kMaxCells = 24;
  static constexpr float kMaxEnergy = 64.0f;

  explicit Field(std::size_t cells = 24, float sample_rate = 48000.0f);

  void reset();
  void set_params(const FieldParams& params);
  void excite(std::size_t cell, float amount);
  float process_sample();

  std::size_t cell_count() const { return cell_count_; }
  float energy() const { return energy_; }
  std::uint32_t reset_count() const { return reset_count_; }

  void debug_corrupt_state();

 private:
  struct Cell {
    float x = 0.0f;
    float v = 0.0f;
    float memory = 0.0f;
  };

  bool state_valid() const;
  void recompute_energy_and_guard();

  std::array<Cell, kMaxCells> cells_{};
  std::array<float, kMaxCells> next_x_{};
  std::array<float, kMaxCells> next_v_{};
  std::size_t cell_count_ = kMaxCells;
  float sample_rate_ = 48000.0f;
  FieldParams params_{};
  float energy_ = 0.0f;
  std::uint32_t reset_count_ = 0;
};

}  // namespace groove
