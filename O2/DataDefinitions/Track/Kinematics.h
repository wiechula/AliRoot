#ifndef TF_DD_TRACK_KINEMATICS_H
#define TF_DD_TRACK_KINEMATICS_H

#include "../Track.h"
namespace track {
IMPLEMENT_MULTIPLE(Kinematics, ITrack);
}

#include "Kinematics/Orientation.h"
#include "Kinematics/Pt.h"
namespace track {
using KinematicsAll = Kinematics<Pt, Orientation>;
}
#endif
