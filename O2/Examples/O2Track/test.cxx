/// \file AliO2Track.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Track class.
///
/// \copyright
///  This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as
/// published by the Free Software Foundation; either version 3 of
/// the License, or (at your option) any later version.
/// This program is distributed in the hope that it will be useful, but
/// WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
/// General Public License for more details at
/// https://www.gnu.org/copyleft/gpl.html

#ifndef ALIO2TRACK_H
#define ALIO2TRACK_H
#include "../../DataDefinitions/Track.h"
#include "../../O2NotImplemented.h"

Track<MuonScore> onlyScore;
Track<KinematicsAll> kinematics;
TrackAll all;
Track<Position<Temporal, Spatial<Y, X, Z>>> track3d;
Track<Position<Temporal, Spatial<Y, X>>> track2d;

int main() {
  std::cout << "kin   : " << kinematics.pt(0) << std::endl;
  std::cout << "distance = "
            << all.Get<IPosition>().Get<ISpatial>().distance_from(
                   track3d.Get<IPosition>().Get<ISpatial>())
            << std::endl;
  // std::cout << "len = " << all.Get<IPosition>().Get<ISpatial>().length(0)
  //           << std::endl;
  // std::cout << "scores: " << onlyScore.pt(0) << std::endl;
  return 0;
}

#endif
