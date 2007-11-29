#ifndef ALITRDCLUSTERCORRECTION_H
#define ALITRDCLUSTERCORRECTION_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "TObject.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Used for correcting the cluster positions in some way or the other    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class AliTRDclusterCorrection : public TObject {

 public:  

  AliTRDclusterCorrection();
  virtual ~AliTRDclusterCorrection();

          Float_t  GetCorrection(Int_t plane, Int_t timebin, Float_t angle) const;
          Float_t  GetOffsetAngle() const            { return fOffsetAngle;  }
          Float_t  GetAngle(Int_t i) const           { return (i - 10.0) / 10.0 + fOffsetAngle; }
          Float_t  GetSigma(Int_t plane, Int_t timebin, Float_t angle) const;

          void     SetCorrection(Int_t plane,Int_t timebin, Float_t angle, Float_t value,Float_t sigma);
          void     SetOffsetAngle(Float_t angle)     { fOffsetAngle = angle; }

  static  AliTRDclusterCorrection *GetCorrection();

 protected:

          Float_t  fCorrections[6][30][20][2];   //  Correction values
          Float_t  fOffsetAngle;                 //  Offset angle
				 
  ClassDef(AliTRDclusterCorrection,1)            //  ClusterCorrection for the TRD
 
};

#endif
