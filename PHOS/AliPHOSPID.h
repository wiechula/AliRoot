#ifndef ALIPHOSPID_H
#define ALIPHOSPID_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
                            
/* $Id$ */

//_________________________________________________________________________
//  Algorithm class for the identification of particles detected in PHOS        
//  base  class                             
//  of identified particles                
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---

#include "TTask.h" 
class TFormula ;
class TClonesArray ;

// --- Standard library ---

// --- AliRoot header files ---

class AliPHOSGeometry ;
class AliPHOSClusterizer ;
class AliPHOSTrackSegmentMaker ;

class AliPHOSPID : public TTask {

 public:

  AliPHOSPID() ;          // ctor            
  AliPHOSPID(const char* evFolderName,const char * name) ;
  virtual ~AliPHOSPID() ; // dtor

  virtual void Exec(Option_t * option) { cout << "AliPHOSPID::Exec not define " << endl ; }
  virtual const Int_t GetRecParticlesInRun()  const { cout << "AliPHOSPID:::GetRecParticlesInRun not defined " << endl ; return 0 ;} 
  virtual void Print(Option_t * option) const { cout << "AliPHOSPID::Print not defined " << endl ;}
  virtual void SetCpvtoEmcDistanceCut(Float_t Cluster_En, TString Eff_Pur,Float_t cut ) { cout << "AliPHOSPID::SetCpvtoEmcDistanceCut not defined " << endl ;}
  virtual void SetTimeGate(Float_t Cluster_En, TString Eff_Pur, Float_t gate) { cout << "AliPHOSPID::SetTimeGate not defined " << endl ; }
  virtual const char * Version() const { cout << "AliPHOSPID::Version not defined " << endl ; return 0 ; }  
  virtual void WriteRecParticles(Int_t event) { cout << "AliPHOSPID::WriteRecParticles not defined " << endl ; }

private: 
  virtual void Init() { cout << "AliPHOSPID::Init not define " << endl ; } 

protected:

  ClassDef(AliPHOSPID,2)  // Particle Identifier algorithm (base class)

} ;

#endif // ALIPHOSPID_H
