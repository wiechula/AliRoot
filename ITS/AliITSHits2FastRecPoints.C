#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TParticle.h>
#include <TStopwatch.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>

#include "AliRun.h"
#include "AliHeader.h"
#include "AliITS.h"
#include "AliITSgeom.h"
#include "AliITSpList.h"
#include "AliITSDetType.h"
#include "AliITSsimulation.h"
#include "AliRunLoader.h"
#include "AliITSLoader.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSsegmentationSSD.h"
#include "AliITSsimulationFastPoints.h"
#include "AliITSClusterFinderSPD.h"
#include "AliITSClusterFinderSDD.h"
#include "AliITSClusterFinderSSD.h"
#include "AliITSresponseSDD.h"

#endif

AliRunLoader* AccessFile(TString inFile="galice.root");

void AliITSHits2FastRecPoints (Int_t evNumber1=0,Int_t evNumber2=0, TString inFile = "galice.root", Int_t nsignal=25, Int_t size=-1) 
{
  /////////////////////////////////////////////////////////////////////////
  //   
  //   This macro creates fast recpoints
  //   
  /////////////////////////////////////////////////////////////////////////
  /*
  // Dynamically link some shared libs

  if (gClassTable->GetID("AliRun") < 0) {
    gROOT->LoadMacro("loadlibs.C");
    loadlibs();
  } else {
  */
  if(gAlice){
    delete gAlice;
    gAlice=0;
  }

  // Connect the Root Galice file containing Geometry, Kine and Hits


  AliRunLoader *rl = AccessFile(inFile);

  AliITS *ITS  = (AliITS*) gAlice->GetModule("ITS");
  if (!ITS) return;

  AliITSLoader * itsl = (AliITSLoader*)rl->GetLoader("ITSLoader");
  itsl->LoadHits();
  itsl->LoadRecPoints("update");

  ITS->SetTreeAddress();

  // Set the simulation model

  for (Int_t i=0;i<3;i++) {
    ITS->SetSimulationModel(i,new AliITSsimulationFastPoints());
  }
   

  //
  // Event Loop
  //


  TStopwatch timer;

  cout << "Creating fast reconstructed points from hits for the ITS..." << endl;

  for (int ev=evNumber1; ev<= evNumber2; ev++) {
    cout << "...working on event "<< ev << " ..." << endl;
    rl->GetEvent(ev);
    cout << "event         " <<ev<<endl;
    if(!itsl->TreeR()) itsl->MakeTree("R");
    ITS->MakeBranch("RF");
    if (ev < evNumber1) continue;

    Int_t bgr_ev=Int_t(ev/nsignal);

    timer.Start();
    ITS->HitsToFastRecPoints(ev,bgr_ev,size," ","All"," ");
    timer.Stop(); timer.Print();
  } // event loop 

}


//-------------------------------------------------------------------
AliRunLoader * AccessFile(TString FileName){

  // Function used to open the input file and fetch the AliRun object

  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(FileName);
  if (file) {file->Close(); delete file; file = 0;}

  AliRunLoader *rl = AliRunLoader::Open(FileName.Data(),"Event","read");
  if (!rl) {
    cerr<<"Can't load RunLoader from "<<FileName<<" !\n";
    return rl;
  }


  // Get AliRun object from file or return if not on file
  if (gAlice) {delete gAlice; gAlice = 0;}
  rl->LoadgAlice();
  gAlice = rl->GetAliRun();

  if (!gAlice) {
	cerr << "AliRun object not found on file"<< endl;
	return 0x0;
  } 

  // load header

  rl->LoadHeader();
  return rl;
}
