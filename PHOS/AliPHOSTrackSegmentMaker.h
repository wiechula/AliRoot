#ifndef ALIPHOSTRACKSEGMENTMAKER_H
#define ALIPHOSTRACKSEGMENTMAKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
// Algorithm Base class to construct PHOS track segments
// Associates EMC and PPSD clusters
// Unfolds the EMC cluster   
//                  
//*-- Author: Dmitri Peressounko (RRC Kurchatov Institute  & SUBATECH)

// --- ROOT system ---
#include "TTask.h"
class TFile ; 

// --- Standard library ---
#include <iostream>

// --- AliRoot header files ---


class AliPHOSClusterizer ;
class AliPHOSGeometry ;

class  AliPHOSTrackSegmentMaker : public TTask {

public:

  AliPHOSTrackSegmentMaker() ;                     
  AliPHOSTrackSegmentMaker(const char* evFoldName, const char* name);
  
  virtual ~ AliPHOSTrackSegmentMaker() ;

  virtual void    Exec(Option_t * option){cout << "Not Defined" << endl ; } 
  virtual const Int_t GetTrackSegmentsInRun()  const {cout << "Not Defined" << endl ; return 0 ; } 

  virtual void    Print(Option_t * option)const {cout << "Not Defined" << endl ; }  

  virtual const char * Version() const {cout << "Not Defined" << endl ; return 0 ; }   
  virtual void WriteTrackSegments(Int_t event){cout << "Not Defined" << endl ; } 
  
protected:
  
  ClassDef( AliPHOSTrackSegmentMaker,2)    // Algorithm class to make PHOS track segments (Base Class)

};

#endif // ALIPHOSTRACKSEGMENTMAKER_H
