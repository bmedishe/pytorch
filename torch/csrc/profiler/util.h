#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <c10/macros/Macros.h>
#include <c10/util/hash.h>
#include <ATen/record_function.h>
#include <torch/csrc/Export.h>
#include <torch/csrc/jit/frontend/source_range.h>

#ifndef _WIN32
#include <ctime>
#endif
#if defined(C10_IOS) && defined(C10_MOBILE)
#include <sys/time.h> // for gettimeofday()
#endif

namespace torch {
namespace profiler {
namespace impl {

using time_t = int64_t;
using steady_clock_t = std::conditional<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock>::type;

inline time_t getTimeSinceEpoch() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

inline time_t getTime(bool allow_monotonic = false) {
#if defined(C10_IOS) && defined(C10_MOBILE)
  // clock_gettime is only available on iOS 10.0 or newer. Unlike OS X, iOS
  // can't rely on CLOCK_REALTIME, as it is defined no matter if clock_gettime
  // is implemented or not
  struct timeval now;
  gettimeofday(&now, NULL);
  return static_cast<time_t>(now.tv_sec) * 1000000000 +
      static_cast<time_t>(now.tv_usec) * 1000;
#elif defined(_WIN32) || defined(__MACH__)
  return duration_cast<std::chrono::nanoseconds>(
             steady_clock_t::now().time_since_epoch())
      .count();
#else
  // clock_gettime is *much* faster than std::chrono implementation on Linux
  struct timespec t {};
  auto mode = CLOCK_REALTIME;
  if (allow_monotonic) {
    mode = CLOCK_MONOTONIC;
  }
  clock_gettime(mode, &t);
  return static_cast<time_t>(t.tv_sec) * 1000000000 +
      static_cast<time_t>(t.tv_nsec);
#endif
}

//   We often do not need to capture true wall times. If a fast mechanism such
// as TSC is available we can use that instead and convert back to epoch time
// during post processing. This greatly reduce the clock's contribution to
// profiling.
//   http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/
//   https://quick-bench.com/q/r8opkkGZSJMu9wM_XTbDouq-0Io
// TODO: We should use
// `https://github.com/google/benchmark/blob/main/src/cycleclock.h`
inline auto getApproximateTime() {
#if defined(BENCHMARK_OS_MACOSX) || defined(BENCHMARK_OS_EMSCRIPTEN)
  return getTime();
#elif defined(__i386__)
  // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
  int64_t ret;
  __asm__ volatile("rdtsc" : "=A"(ret));
  return ret;
#elif defined(__x86_64__) || defined(__amd64__)
  // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
  uint64_t low, high;
  __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
  return (high << 32) | low;
#else
  return getTime();
#endif
}

using approx_time_t = decltype(getApproximateTime());
static_assert(
    std::is_same<approx_time_t, int64_t>::value ||
        std::is_same<approx_time_t, uint64_t>::value,
    "Expected either int64_t (`getTime`) or uint64_t (some TSC reads).");

// Convert `getCount` results to Nanoseconds since unix epoch.
class ApproximateClockToUnixTimeConverter {
 public:
  ApproximateClockToUnixTimeConverter();
  std::function<time_t(approx_time_t)> makeConverter();

  struct UnixAndApproximateTimePair {
    time_t t_;
    approx_time_t approx_t_;
  };
  static UnixAndApproximateTimePair measurePair();

 private:
  static constexpr size_t replicates = 101;
  using time_pairs = std::array<UnixAndApproximateTimePair, replicates>;
  time_pairs measurePairs();

  time_pairs start_times_;
};

std::string getNvtxStr(
    const char* name,
    int64_t sequence_nr,
    const std::vector<std::vector<int64_t>>& shapes);

struct TORCH_API FileLineFunc {
  std::string filename;
  size_t line;
  std::string funcname;
};

TORCH_API std::vector<FileLineFunc> prepareCallstack(
    const std::vector<jit::StackEntry>& cs);
TORCH_API std::vector<std::string> callstackStr(
    const std::vector<FileLineFunc>& cs);
TORCH_API std::string stacksToStr(
    const std::vector<std::string>& stacks,
    const char* delim);
TORCH_API std::vector<std::vector<int64_t>> inputSizes(
    const at::RecordFunction& fn);
TORCH_API std::string shapesToStr(
    const std::vector<std::vector<int64_t>>& shapes);
TORCH_API std::string dtypesToStr(const std::vector<std::string>& types);
TORCH_API std::vector<std::string> inputTypes(const at::RecordFunction& fn);

std::unordered_map<std::string, c10::IValue> TORCH_API
saveExtraArgs(const at::RecordFunction& fn);

uint64_t TORCH_API computeFlops(
    const std::string& op_name,
    const std::unordered_map<std::string, c10::IValue>& extra_args);

} // namespace impl
} // namespace profiler
} // namespace torch

namespace torch {
namespace autograd {
namespace profiler {
using torch::profiler::impl::getTime;
using torch::profiler::impl::computeFlops;
} // namespace profiler
} // namespace autograd
} // namespace torch
