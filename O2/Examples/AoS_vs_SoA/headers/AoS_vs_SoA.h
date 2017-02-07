#include "../../../DataDefinitions/Track.h"
#include "../../../DataDefinitions/Vertex.h"

void flush_cache();

typedef int32_t v8i __attribute__((vector_size(32)));
typedef float v8f __attribute__((vector_size(32)));
// We define a FlatTrack which only contains a subset of data to make things
// more fair. (i.e. favourable to AoS).

using namespace track;
using FlatTrack = Track<MuonScore, Position<SpatialAll>, Kinematics<Pt>>;

// Creates a SoA track container and fills the "muonScores" array with random
// numbers. count is the number of tracks created.
TrackAll createTrackContainer(unsigned count);
// frees up the memory allocated by `createTrackContainer`
void freeTrackContainer(TrackAll &trackCollection);

// Creates a flat (AoS) style buffer and fills the arrays
// the returned array can be `delete[]`ed. count is the number of tracks created.
FlatTrack::flat_type *make_flat(unsigned count);

// the following functions perform our fake "analysis", the select approximately one in "oneIn" tracks depeding on the value of the MuonScore track variable. If it passes the filter it will compute the length of the position vector and subtract this from the Pt. The resulting value is summed for all tracks that pass the filter.
// There are three version, the regular SoA version, the "_flat" version which uses SoA, and the "_vectorized" version which is SoA but uisng avx2 vector instructions.
double analyseMuons_flat(FlatTrack::flat_type *trackCollection, unsigned count,
                         int oneIn = 1);

double analyseMuons(TrackAll &trackCollection, unsigned count,
                    int oneIn = 1);

double analyseMuons_v(TrackAll &trackCollection, unsigned count,
                      int oneIn = 1);
