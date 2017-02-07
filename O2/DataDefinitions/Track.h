#ifndef TF_DD_TRACKS_H
#define TF_DD_TRACKS_H
#include "base.h"

IMPLEMENT_MULTIPLE(Track, void);
//
// #include "Generic/Position.h"
#include "Track/Covariance.h"
#include "Track/Kinematics.h"
#include "Track/MuonScore.h"
#include "Track/Position.h"

using TrackAll = Track<track::PositionAll, track::KinematicsAll,
                       track::Covariance, track::MuonScore>;
#endif
