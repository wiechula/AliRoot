#ifndef ALIEMCALGETTER_H
#define ALIEMCALGETTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  A singleton that returns various objects 
//  Should be used on the analysis stage to avoid confusing between different
//  branches of reconstruction tree: e.g. reading RecPoints and TS made from 
//  another set of RecPoints.
// 
//  The objects are retrived from folders.  
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)
//    


// --- ROOT system ---
#include "TClonesArray.h"
#include "TFolder.h"  
#include "TTree.h"
class TString ;
class TParticle ;
class TTask ;

// --- Standard library ---
#include <stdlib.h>
#include <iostream.h>

// --- AliRoot header files ---

#include "AliRun.h"
#include "AliLoader.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliEMCALv1.h"
#include "AliEMCALHit.h"
#include "AliEMCALDigit.h"
#include "AliEMCALDigitizer.h"
#include "AliEMCALSDigitizer.h"
#include "AliEMCALTowerRecPoint.h"
class AliEMCALGeometry ;
class AliEMCALClusterizerv1 ;


//

class AliEMCALLoader : public AliLoader {
  
 public:

  AliEMCALLoader();
  AliEMCALLoader(const AliEMCALLoader & obj){}
  AliEMCALLoader(const Char_t *detname,const Char_t *eventfoldername); 
  
  virtual ~AliEMCALLoader() ; 

  // assignement operator requested by coding convention, but not needed
  AliEMCALLoader & operator = (const AliEMCALLoader & ) {return *this;}
  Int_t  PostHits(); 
  Int_t  PostSDigits();
  Int_t  PostDigits();
  Int_t  PostRecPoints();
  Int_t  PostTracks();  //previous PostTrackSegments
 
  void   CleanFolders();//cleans all the stuff loaded by this detector + calls AliLoader::Clean

//up to now it is only here -> no definition about global/incremental tracking/PID
  Int_t   LoadRecParticles(Option_t* opt="");//loads reconstructed particles
  Int_t   WriteRecParticles(Option_t* opt="");//writes the reconstructed particles
  Int_t   WritePID(Option_t* opt="");//writes the task for PID to file
  Bool_t  PostQA   (void) const ;
  
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

  TObject** HitsRef(){return GetDetectorDataRef(Hits());}
  TObject** SDigitsRef(){return GetDetectorDataRef(SDigits());}
  TObject** DigitsRef(){return GetDetectorDataRef(Digits());}
  TObject** EmcRecPointsRef(){return GetDetectorDataRef(EmcRecPoints());}
  TObject** CpvRecPointsRef(){return GetDetectorDataRef(CpvRecPoints());}
  TObject** TracksRef(){return GetDetectorDataRef(TrackSegments());}
  TObject** AlarmsRef(){return GetDetectorDataRef(Alarms());}
  void   Track(Int_t itrack) ;

  static AliEMCALGeometry* GetEMCALGeometry();
  static AliEMCALLoader* GetEMCALLoader(const  char* eventfoldername);

  //Method to be used when digitizing under AliRunDigitizer, who opens all files etc.
  Int_t  EventNumber()       { return (Int_t) GetRunLoader()->GetEventNumber();}
  Int_t  MaxEvent()          { return (Int_t) GetRunLoader()->TreeE()->GetEntries();}

  const AliEMCAL *         EMCAL();
  const AliEMCALGeometry  *EMCALGeometry() ; 
  // Alarms
  // TFolder * Alarms() const { return (TFolder*)(ReturnO("Alarms", 0)); }
  TObjArray *  Alarms();  

  /*********************************************/
  /************    TClonesArrays     ***********/
  /*********************************************/
  /****   H i t s  ****/
  TClonesArray*  Hits(void);
  const AliEMCALHit*    Hit(Int_t index);
  /****   S D i g i t s  ****/ 
  TClonesArray*  SDigits();
  const AliEMCALDigit*  SDigit(Int_t index);
  /****  D i g i t s  ****/
  TClonesArray*   Digits();
  const AliEMCALDigit *  Digit(Int_t index);
  /****  R e c P o i n t s  ****/
  TObjArray * EmcRecPoints();
  TObjArray * CpvRecPoints();
  const AliEMCALEmcRecPoint * EmcRecPoint(Int_t index) ;
  const AliEMCALCpvRecPoint * CpvRecPoint(Int_t index) ;
  /****   T r a c k S e g m e n t s ****/
  TClonesArray * TrackSegments();
  const AliEMCALTrackSegment * TrackSegment(Int_t index);
  /****  R e c P a r t ic l e s   ****/
  TClonesArray * RecParticles() ;
  const AliEMCALRecParticle * RecParticle(Int_t index);

  /*********************************************/
  /************    T A S K S      **************/
  /*********************************************/
  AliEMCALPID * PID(const char * name =0) const { MayNotUse("PID"); return 0x0;  }
  AliEMCALSDigitizer*  EMCALSDigitizer() { return (AliEMCALSDigitizer*)SDigitizer();}
  AliEMCALDigitizer*   EMCALDigitizer() {  return (AliEMCALDigitizer*)Digitizer();}
  AliEMCALClusterizer* Clusterizer ()  {return (AliEMCALClusterizer*)Reconstructioner();}
  AliEMCALTrackSegmentMaker * TrackSegmentMaker () { return (AliEMCALTrackSegmentMaker*)Tracker() ;}
  
  Int_t PostClusterizer(TTask* clust){return PostReconstructioner(clust);}
  Int_t PostTrackSegmentMaker(TTask* segmaker){return PostTracker(segmaker);}
  
  void   SetDebug(Int_t level) {fDebug = level;} // Set debug level
private:

  Int_t ReadHits();
  Int_t ReadDigits();
  Int_t ReadSDigits();
  Int_t ReadRecPoints();
  Int_t ReadTracks();
  Int_t ReadRecParticles();
  
  void  ReadTreeQA() ;

 private:

  Int_t          fDebug ;             // Debug level
 
 public:

  static const TString fgkHitsName;//Name for TClonesArray with hits from one event
  static const TString fgkSDigitsName;//Name for TClonesArray 
  static const TString fgkDigitsName;//Name for TClonesArray 
  static const TString fgkEmcRecPointsName;//Name for TClonesArray 
  static const TString fgkCpvRecPointsName;//Name for TClonesArray 
  static const TString fgkTracksName;//Name for TClonesArray 
  static const TString fgkReconstrParticles;//Name for TClonesArray

  static const TString fgkEmcRecPointsBranchName;//Name for TClonesArray 
  static const TString fgkCpvRecPointsBranchName;//Name for TClonesArray 
  
  
  ClassDef(AliEMCALLoader,2)  // Algorithm class that provides methods to retrieve objects from a list knowing the index 

};

/******************************************************************************/
/****************    I N L I N E S     ****************************************/
/******************************************************************************/

inline TClonesArray* AliEMCALLoader::Hits()  
{
 return (TClonesArray*)GetDetectorData(fgkHitsName);
}
/******************************************************************************/

inline const AliEMCALHit* AliEMCALLoader::Hit(Int_t index)  
{
  const TClonesArray* tcarr = Hits();
  if (tcarr)
    return (const AliEMCALHit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray* AliEMCALLoader::SDigits()
{
   return dynamic_cast<TClonesArray*>(GetDetectorData(fgkSDigitsName));
}
/******************************************************************************/

inline const AliEMCALDigit*  AliEMCALLoader::SDigit(Int_t index)
{
  const TClonesArray* tcarr = SDigits();
  if (tcarr)
    return (const AliEMCALDigit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray* AliEMCALLoader::Digits()
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkDigitsName));
}
/******************************************************************************/

inline const AliEMCALDigit*  AliEMCALLoader::Digit(Int_t index)
{
  const TClonesArray* tcarr = Digits();
  if (tcarr)
    return (const AliEMCALDigit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TObjArray * AliEMCALLoader::EmcRecPoints()
{
 return dynamic_cast<TObjArray*>(GetDetectorData(fgkEmcRecPointsName));
}
/******************************************************************************/

inline const AliEMCALEmcRecPoint * AliEMCALLoader::EmcRecPoint(Int_t index)
{
  TObjArray* tcarr = EmcRecPoints();
  if (tcarr)
    return (const AliEMCALEmcRecPoint*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TObjArray * AliEMCALLoader::CpvRecPoints()
{
 return dynamic_cast<TObjArray*>(GetDetectorData(fgkCpvRecPointsName));
}
/******************************************************************************/

inline const AliEMCALCpvRecPoint * AliEMCALLoader::CpvRecPoint(Int_t index)
{
  TObjArray* tcarr = CpvRecPoints();
  if (tcarr)
    return (const AliEMCALCpvRecPoint*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray * AliEMCALLoader::TrackSegments()
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkTracksName));
}
/******************************************************************************/

inline const AliEMCALTrackSegment * AliEMCALLoader::TrackSegment(Int_t index)
{
  const TClonesArray* tcarr = TrackSegments();
  if (tcarr)
    return (const AliEMCALTrackSegment*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray * AliEMCALLoader::RecParticles() 
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkReconstrParticles)); 
}
/******************************************************************************/

inline const AliEMCALRecParticle* AliEMCALLoader::RecParticle(Int_t index)
{
  TClonesArray* tcarr = RecParticles();
  if (tcarr)
    return (const AliEMCALRecParticle*) tcarr->At(index);
  return 0x0;  
}
/******************************************************************************/
inline TObjArray *  AliEMCALLoader::Alarms()
{ return (TObjArray*)(GetQAFolder()->FindObject(*fDetectorName));}

#endif // AliEMCALGETTER_H
