#include "headers/AoS_vs_SoA.h"
#include <algorithm>
#include <array>
#include <chrono>

int main() {
  constexpr unsigned number_of_tracks = 1 << 26;
  constexpr int max_one_in = 1 << 24;      // 1<<24 ~= one in 17 million
  constexpr int repetitions_per_test = 11; // run each the analysis this many
                                           // times in a row to get some
                                           // statistically sound results.
  std::array<double, repetitions_per_test>
      timings; // Array for storing our timing results.

  auto tracks = createTrackContainer(number_of_tracks);
  puts("\"SoA (Vectorized)\"");
  // loop over different odds of passing a filter to simulate cutting on
  // relatively few variables.
  for (int one_in = 1; one_in <= max_one_in; one_in += (one_in + 1) / 2) {
    double result = 0; // we need to get and use the result from each run to
                       // prevent the compiler from optimizing it out.

    // repeat several times to get better timing information
    for (int run = 0; run < repetitions_per_test; run++) {
      auto start_time = std::chrono::steady_clock::now();
      result += analyseMuons_v(tracks, number_of_tracks, one_in);
      auto end_time = std::chrono::steady_clock::now();
      timings[run] =
          std::chrono::duration<double, std::nano>(end_time - start_time)
              .count(); // add the timing information, in ns, to the timings
                        // aray
    }
    // get the fastest, median, and slowest run time.
    std::sort(timings.begin(), timings.end());
    double fastest = timings[0];
    double slowest = timings[repetitions_per_test - 1];
    double median = timings[repetitions_per_test / 2];
    // Compute the efective bandwidth achieved
    size_t size_filter_array = number_of_tracks * 4;
    size_t size_passing_tracks =
        (sizeof(MinimalTrack::flat_type) - 4) * number_of_tracks /
        one_in; // subtract 4 to prevent double counting the filter variable.
    double highest_bandwith =
        (size_filter_array + size_passing_tracks) / fastest;
    double pass_rate = 1.0 / one_in;
    // print the results.
    printf("%.4e\t%.4e %.4e %.4e\t%6.2f\t%2.1e\n", pass_rate, slowest, median,
           fastest, highest_bandwith, result);
  }
  puts("\n");

  puts("\"SoA (Scalar)\"");
  for (int one_in = 1; one_in <= max_one_in; one_in += (one_in + 1) / 2) {
    double result = 0; // we need to get and use the result from each run to
                       // prevent the compiler from optimizing it out.
    for (int run = 0; run < repetitions_per_test; run++) {
      auto start_time = std::chrono::steady_clock::now();
      result += analyseMuons(tracks, number_of_tracks, one_in);
      auto end_time = std::chrono::steady_clock::now();
      timings[run] =
          std::chrono::duration<double, std::nano>(end_time - start_time)
              .count();
    }
    std::sort(timings.begin(), timings.end());
    double fastest = timings[0];
    double slowest = timings[repetitions_per_test - 1];
    double median = timings[repetitions_per_test / 2];
    size_t size_filter_array = number_of_tracks * 4;
    size_t size_passing_tracks =
        (sizeof(MinimalTrack::flat_type) - 4) * number_of_tracks /
        one_in; // subtract 4 to prevent double counting the filter variable.
    double highest_bandwith =
        (size_filter_array + size_passing_tracks) / fastest;
    double pass_rate = 1.0 / one_in;
    printf("%.4e\t%.4e %.4e %.4e\t%6.2f\t%2.1e\n", pass_rate, slowest, median,
           fastest, highest_bandwith, result);
  }
  puts("\n");

  freeTrackContainer(tracks);
  auto tracks_flat = make_flat(number_of_tracks);
  puts("\"AoS (Scalar)\"");
  for (int one_in = 1; one_in <= max_one_in; one_in += (one_in + 1) / 2) {
    double result = 0; // we need to get and use the result from each run to
                       // prevent the compiler from optimizing it out.
    for (int run = 0; run < repetitions_per_test; run++) {
      auto start_time = std::chrono::steady_clock::now();
      result += analyseMuons_flat(tracks_flat, number_of_tracks, one_in);
      auto end_time = std::chrono::steady_clock::now();
      timings[run] =
          std::chrono::duration<double, std::nano>(end_time - start_time)
              .count();
    }
    std::sort(timings.begin(), timings.end());
    double fastest = timings[0];
    double slowest = timings[repetitions_per_test - 1];
    double median = timings[repetitions_per_test / 2];
    size_t size_filter_array = number_of_tracks * 4;
    size_t size_passing_tracks =
        (sizeof(MinimalTrack::flat_type) - 4) * number_of_tracks /
        one_in; // subtract 4 to prevent double counting the filter variable.
    double highest_bandwith =
        (size_filter_array + size_passing_tracks) / fastest;
    double pass_rate = 1.0 / one_in;
    printf("%.4e\t%.4e %.4e %.4e\t%6.2f\t%2.1e\n", pass_rate, slowest, median,
           fastest, highest_bandwith, result);
  }
  puts("\n");

  return 0;
}
