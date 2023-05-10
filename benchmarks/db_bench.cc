//
// Created by essethon on 5/5/23. Adapted from leveldb/benchmarks/db_bench.cc.
//

#include <sys/time.h>
#include <sys/types.h>

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>

#include "port/port.h"
#include "port/port_stdcxx.h"
#include "util/histogram.h"
#include "util/random.h"

using Slice = std::string_view;

static bool FLAGS_histogram = false;

namespace spatialkv {

// Return the number of micro-seconds since some fixed point in time (e.g.,
// since the start of the day.)
uint64_t NowMicros() {
  static constexpr uint64_t kUsecondsPerSecond = 1000000;
  struct ::timeval tv {};
  ::gettimeofday(&tv, nullptr);
  return static_cast<uint64_t>(tv.tv_sec) * kUsecondsPerSecond + tv.tv_usec;
}

Slice RandomString(RandomInt& rnd, int len, std::string* dst) {
  dst->resize(len);
  for (int i = 0; i < len; i++) {
    (*dst)[i] = static_cast<char>(rnd(32, 127));  // ' ' .. '~'
  }
  return *dst;
}

// Helper for quickly generating random data.
class RandomGenerator {
 private:
  std::string data_;
  int pos_;

 public:
  RandomGenerator() {
    RandomInt rnd;
    std::string piece;
    while (data_.size() < 1048576) {
      // Add a short fragment that is as compressible as specified
      // by FLAGS_compression_ratio.
      RandomString(rnd, 100, &piece);
      data_.append(piece);
    }
    pos_ = 0;
  }

  Slice Generate(size_t len) {
    if (pos_ + len > data_.size()) {
      pos_ = 0;
      assert(len < data_.size());
    }
    pos_ += len;
    return {data_.data() + pos_ - len, len};
  }
};

class Stats {
 private:
  double start_{};
  double finish_{};
  double seconds_{};
  int done_{};
  int next_report_{};
  int64_t bytes_{};
  double last_op_finish_{};
  Histogram hist_;
  std::string message_;

 public:
  Stats() { Start(); }

  void Start() {
    next_report_ = 100;
    hist_.Clear();
    done_ = 0;
    bytes_ = 0;
    seconds_ = 0;
    message_.clear();
    start_ = finish_ = last_op_finish_ = NowMicros();
  }

  void Stop() {
    finish_ = NowMicros();
    seconds_ = (finish_ - start_) * 1e-6;
  }

  void FinishedSingleOp() {
    if (FLAGS_histogram) {
      double now = NowMicros();
      double micros = now - last_op_finish_;
      hist_.Add(micros);
      if (micros > 20000) {
        std::fprintf(stderr, "long op: %.1f micros%30s\r", micros, "");
        std::fflush(stderr);
      }
      last_op_finish_ = now;
    }

    done_++;
    if (done_ >= next_report_) {
      if (next_report_ < 1000)
        next_report_ += 100;
      else if (next_report_ < 5000)
        next_report_ += 500;
      else if (next_report_ < 10000)
        next_report_ += 1000;
      else if (next_report_ < 50000)
        next_report_ += 5000;
      else if (next_report_ < 100000)
        next_report_ += 10000;
      else if (next_report_ < 500000)
        next_report_ += 50000;
      else
        next_report_ += 100000;
      std::fprintf(stderr, "... finished %d ops%30s\r", done_, "");
      std::fflush(stderr);
    }
  }
};

struct SharedState {
  port::Mutex mu;
  port::CondVar cv GUARDED_BY(mu);
  int total GUARDED_BY(mu);

  // Each thread goes through the following states:
  //    (1) initializing
  //    (2) waiting for others to be initialized
  //    (3) running
  //    (4) done

  int num_initialized GUARDED_BY(mu);
  int num_done GUARDED_BY(mu);
  bool start GUARDED_BY(mu);

  SharedState(int total)
      : cv(&mu), total(total), num_initialized(0), num_done(0), start(false) {}
};

struct ThreadState {
  int tid;         // 0..n-1 when running in n threads
  RandomInt rand;  // Has different seeds for different threads
  Stats stats;
  SharedState* shared;
};

class Benchmark {
 private:
  void PrintHeader() {
    std::cout << std::format("SpatialKV version {}", random_double(0, 0.1))
              << std::endl;
  }

  RandomDouble random_double;

 public:
  void Run() { PrintHeader(); }
};
}  // namespace spatialkv

int main(int argc, char** argv) {
  spatialkv::Benchmark benchmark;
  benchmark.Run();
  spatialkv::RandomGenerator rg;
  for (int i = 0; i < 10; i++) {
    std::cout << rg.Generate(50) << std::endl;
  }
  return 0;
}