#ifndef AliRICHClusterFinder_h
#define AliRICHClusterFinder_h

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "TTask.h"

#include "AliRICH.h"
class AliHitMap;

class AliRICHClusterFinder : public TTask
{
public:    
           AliRICHClusterFinder(AliRICH *pRICH);
  virtual ~AliRICHClusterFinder()                                          {;}
  
  AliRICH *Rich() {return fRICH;}                                             //Pointer to RICH  
  void     Exec();                                                            //Loop on events and chambers  
  void     FindRawClusters(Int_t iChamber);                                   //Find raw clusters  
  void     FormRawCluster(Int_t i, Int_t j);                                  //form a raw cluster
  void     FindLocalMaxima();                                                 //Find local maxima in a cluster
  void     ResolveCluster();                                                  //Try to resolve a cluster with maxima > 2
  void     FitCoG();                                                          //Evaluate the CoG as the best 
  void     WriteRawCluster();                                                 //write in the list of the raw clusters  
  AliRICHcluster *GetCurrentCluster() {return &fCurrentCluster;}              //Return pointer to the current cluster
protected:
  AliRICH                *fRICH;                         //Pointer to RICH
  AliHitMap              *fHitMap;                       //Hit Map with digit positions
  AliRICHcluster         fCurrentCluster;                //Current cluster to examine
  ClassDef(AliRICHClusterFinder,0) //Finds raw clusters, trasfers them to resolved clusters through declustering.
};
#endif
