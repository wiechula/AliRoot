//   Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved.
//  See cxx source for full Copyright notice                               
//  AliFMDReconstruction.h 
//  Task Class for making TreeR in FMD                        
//-- Authors: Evgeny Karpechev (INR) and Alla Maevskaia (INR)


#ifndef AliFMDReconstruction_h
#define AliFMDReconstruction_h

#include "TTask.h"
#include "TString.h"
#include "AliFMD.h"

class AliRunLoader;

class AliFMDReconstruction: public TTask 
{
 public:
  AliFMDReconstruction() ; 
  AliFMDReconstruction(AliRunLoader* rl) ; 
  virtual ~AliFMDReconstruction();

  virtual void  Exec(Option_t *option); 
  void SetNEvents(Int_t Nevents){fNevents = Nevents;}
  Stat_t GetNEvents(){return fNevents;}

 private:
  Int_t   fNevents ;                         // Number of events

  AliRunLoader* fRunLoader;  //!Run Loader of that event
  ClassDef(AliFMDReconstruction,2) 
};
#endif









