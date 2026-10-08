#pragma once

#include <array>
#include <cstdint>

namespace orcgroove::ui {

struct PerformanceSnapshot {
  std::array<float, 7> macros{{0.68f, 0.42f, 0.73f, 0.31f, 0.58f, 0.87f, 0.24f}};
  std::array<bool, 8> held_notes{};
  float matter_xy = 0.68f;
  float tension_xy = 0.42f;
  float cpu_percent = 0.0f;
  float ai_hz = 0.0f;
  float sample_rate_hz = 48000.0f;
  float latency_ms = 0.0f;
  int voices = 0;
  int max_voices = 8;
  float output_db = -60.0f;
  int octave = 0;
  bool hold = false;
  bool wifi = false;
  bool audio = false;
  bool midi = false;
  int battery_percent = -1;
  const char* time = "--:--";
  const char* patch = "INIT MATERIAL";
};

}  // namespace orcgroove::ui
