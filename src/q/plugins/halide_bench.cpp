#include <chrono>
#include "k.h"

// Prefer high_resolution_clock, but only if it's steady...
template<bool HighResIsSteady = std::chrono::high_resolution_clock::is_steady>
struct SteadyClock {
    using type = std::chrono::high_resolution_clock;
};

// ...otherwise use steady_clock.
template<>
struct SteadyClock<false> {
    using type = std::chrono::steady_clock;
};

inline SteadyClock<>::type::time_point benchmark_now() {
    return SteadyClock<>::type::now();
}

inline double benchmark_duration_seconds(
    SteadyClock<>::type::time_point start,
    SteadyClock<>::type::time_point end) {
    return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
}

SteadyClock<>::type::time_point start = benchmark_now();

extern "C"{
K start_time(K x)
{
    start = benchmark_now();
    return kj(0);
}

K get_duration(K x)
{
    return kf(benchmark_duration_seconds(start, benchmark_now()));
}
}
