#include "../headers/AoS_vs_SoA.h"
#include <cmath>
#include <immintrin.h>

// A 0.5 GiB buffer used to flush the cache before running our benchmark
#define trash_size ((uint)1 << 29)
volatile uint8_t trash[trash_size];
// can be used to flush the cache in between runs.
void flush_cache() {
  for (unsigned u = 0; u < trash_size; u++) {
    trash[u] += 1;
  }
}

// Creates a SoA track container and fills the "muonScores" array with random
// numbers. count is the number of tracks created.
TrackAll createTrackContainer(unsigned count) {
  TrackAll trackCollection;
  // We use valloc here because valloc allocates on page-boundaries, it's an
  // easy way to guarentee alignment
  trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>() =
      (X *)valloc(sizeof(X) * count);
  trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>() =
      (Y *)valloc(sizeof(Y) * count);
  trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>() =
      (Z *)valloc(sizeof(Z) * count);
  // trackCollection.GetRef<IPosition>().GetRef<Temporal>() =
  //     (Temporal *)valloc(sizeof(Temporal) * count);
  trackCollection.GetRef<IKinematics>().GetRef<Pt>() =
      (Pt *)valloc(sizeof(Pt) * count);
  // trackCollection.GetRef<IKinematics>().GetRef<Orientation>() =
  //     (Orientation *)valloc(sizeof(Orientation) * count);
  // trackCollection.GetRef<Covariance>() =
  //     (Covariance *)valloc(sizeof(Covariance) * count);
  trackCollection.GetRef<MuonScore>() =
      (MuonScore *)valloc(sizeof(MuonScore) * count);
  auto muonScores = trackCollection.GetRef<MuonScore>();
  auto *Pt = trackCollection.GetRef<IKinematics>().GetRef<track::Pt>();
  auto *x = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>();
  auto *y = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>();
  auto *z = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>();
  // reset the rng for reliability
  srand(0);
  for (unsigned u = 0; u < count; u++) {
    muonScores[u].set(rand());
    Pt[u].set(5);
    x[u].set(1.3);
    y[u].set(7.43);
    z[u].set(0.1);
  }

  return trackCollection;
}

/// Clears up the space allocated by `createTrackContainer`.
void freeTrackContainer(TrackAll &trackCollection) {
  free(trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>());
  free(trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>());
  free(trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>());
  // free(trackCollection.GetRef<IPosition>().GetRef<Temporal>());
  free(trackCollection.GetRef<IKinematics>().GetRef<Pt>());
  // free(trackCollection.GetRef<IKinematics>().GetRef<Orientation>());
  // free(trackCollection.GetRef<Covariance>());
}

// Creates a flat (AoS) style buffer and fills the muon array.
FlatTrack::flat_type *make_flat(unsigned count) {
  // allocate our array
  FlatTrack::flat_type *flattend = new FlatTrack::flat_type[count];
  srand(0);
  // and fill it
  for (unsigned u = 0; u < count; u++) {
    flattend[u].GetRef<MuonScore>().set(rand());
    flattend[u].GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>().set(1.3);
    flattend[u].GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>().set(7.43);
    flattend[u].GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>().set(0.1);
    flattend[u].GetRef<IKinematics>().GetRef<Pt>().set(5);
  }
  return flattend;
}

// AoS analysis
double analyseMuons_flat(FlatTrack::flat_type *trackCollection, unsigned count,
                         int oneIn) {
  unsigned cutoff = RAND_MAX / oneIn;
  float arbitrary = 0;
// Run the following for loop in parallel, while doing a reduction over
// arbitrary. Schedule splits the work into chunks of the given amount of
// iterations and dynamically allocates parts of it.
#pragma omp parallel for reduction(+ : arbitrary) schedule(                    \
    dynamic, 4096 / sizeof(FlatTrack::flat_type))
  for (unsigned u = 0; u < count; u++) {
    auto &track = trackCollection[u];
    // if the u'd track passes the cutoff:
    if (track.GetRef<MuonScore>() < cutoff) {
      float x = track.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>();
      float y = track.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>();
      float z = track.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>();
      float r = sqrtf(x * x + y * y + z * z);
      // sum pt-r.
      arbitrary += (track.GetRef<IKinematics>().GetRef<Pt>() - r);
    }
  }
  return arbitrary;
}

// SoA analysis
double analyseMuons(TrackAll &trackCollection, unsigned count, int oneIn) {
  unsigned cutoff = RAND_MAX / oneIn;
  // get the arrays from the struct
  auto *muonScores = trackCollection.GetRef<MuonScore>();
  auto *Pt = trackCollection.GetRef<IKinematics>().GetRef<track::Pt>();
  auto *x = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>();
  auto *y = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>();
  auto *z = trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>();
  float arbitrary = 0;
// perform the following loop in parallel, reducing over arbitrary, splitting
// the work in chunks of 1024 iterations each
#pragma omp parallel for reduction(+ : arbitrary) schedule(dynamic, 1024)
  for (unsigned u = 0; u < count; u++) {
    if (cutoff > muonScores[u]) {
      float r = sqrtf(x[u] * x[u] + y[u] * y[u] + z[u] * z[u]);
      arbitrary += (Pt[u] - r);
    }
  }
  return arbitrary;
}

// SoA vectorized
double analyseMuons_v(TrackAll &trackCollection, unsigned count, int oneIn) {
  v8i cutoff = {RAND_MAX / oneIn, RAND_MAX / oneIn, RAND_MAX / oneIn,
                RAND_MAX / oneIn, RAND_MAX / oneIn, RAND_MAX / oneIn,
                RAND_MAX / oneIn, RAND_MAX / oneIn};
  // get the arrays, reinterpret_cast to vectorized arrays. PoC for now, needs
  // to be cleaner.
  v8i *muonScores =
      reinterpret_cast<v8i *>(trackCollection.GetRef<MuonScore>());
  v8f *Pt = reinterpret_cast<v8f *>(
      trackCollection.GetRef<IKinematics>().GetRef<track::Pt>());
  v8f *x = reinterpret_cast<v8f *>(
      trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<X>());
  v8f *y = reinterpret_cast<v8f *>(
      trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Y>());
  v8f *z = reinterpret_cast<v8f *>(
      trackCollection.GetRef<IPosition>().GetRef<ISpatial>().GetRef<Z>());
  double arbitrary = 0;

  // vector of all zeros
  v8f zero = _mm256_set1_ps(0);
// v8f one = _mm256_set1_ps(1);
// reduce over arbitrary at the end of this block
#pragma omp parallel reduction(+ : arbitrary)
  {
    // vector frontend for arbitrary
    v8f arbitrary_v = _mm256_set1_ps(0);
// parallel loop, count/8 because we are dealing with a vector of 8 elements
// each iteration
#pragma omp for schedule(dynamic, 128)
    for (unsigned u = 0; u < count / 8; u++) {
      v8i muon = muonScores[u];
      // what follows is a bit of magic to if 'muon < cutoff' for any of the 8
      // elements
      // get max element-wise off cutoff and muon
      // TODO: switched from unsigned to signed. can use normal cmplt
      __m256i max = _mm256_max_epu32((__m256i)cutoff, (__m256i)muon);
      // cutoff >= muonscores
      __m256i cmp = _mm256_cmpeq_epi32(max, (__m256i)cutoff);
      if (!_mm256_testz_si256(cmp, cmp)) { // checks if cmp is not all zero.
                                           // i.e. at least one element of the 8
                                           // passes the filter

        v8f r = _mm256_sqrt_ps(x[u] * x[u] + y[u] * y[u] +
                               z[u] * z[u]); // compute r for all 8 at a time
        v8f blend = _mm256_blendv_ps(
            zero, Pt[u] - r, (__m256)cmp); // pt-r if the corresponding element
                                           // passes the filter, zero otherwise
        arbitrary_v += blend;              // add the blend to the sum.
      }
    }
    // sum arbitrary_v into arbitrary
    for (unsigned u = 0; u < 8; u++) {
      arbitrary += arbitrary_v[u];
    }
  }
  return arbitrary;
}
