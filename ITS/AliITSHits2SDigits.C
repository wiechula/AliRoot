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
#include "AliLoader.h"

#endif
Int_t AliITSHits2SDigits(const char *inFile = "galice.root"){


    // Connect the Root Galice file containing Geometry, Kine and Hits
  
    TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(inFile);
    if (file) {file->Close(); delete file;}
    AliRunLoader *rl = AliRunLoader::Open(inFile,"Event","read");
  if (!rl) 
   {
    cerr<<"Can't load RunLoader from "<<inFile<<" !\n";
    return 1;
   }

    cout << "AliITSHits2SDigitsDefault" << endl;

    // Get AliRun object from file or return if not on file
    if (gAlice) { delete gAlice; gAlice = 0;}
    rl->LoadgAlice();
    rl->LoadHeader();
    gAlice = rl->GetAliRun();
    if (!gAlice) {
	cerr << "AliITSITSHits2Digits.C : AliRun object not found on file"
	    << endl;
	return 2;
    } // end if !gAlice

    rl->GetEvent(0);
    AliITS *ITS = (AliITS*)gAlice->GetDetector("ITS");      
    if (!ITS) {
	cerr<<"AliITSHits2DigitsDefault.C : AliITS object not found on file"
	    << endl;
	return 3;
    }  // end if !ITS

    AliLoader * itsl = rl->GetLoader("ITSLoader");
    itsl->LoadHits();
    itsl->LoadSDigits("update");

    if(!(ITS->GetITSgeom())){
	cerr << " AliITSgeom not found. Can't digitize with out it." << endl;
	return 4;
    } // end if


    if(!itsl->TreeS()){ 
	cout << "Having to create the SDigits Tree." << endl;
	itsl->MakeTree("S");
    } 
    //make branch
    ITS->MakeBranch("S");
    ITS->SetTreeAddress();
    cout << "Digitizing ITS..." << endl;

    TStopwatch timer;
    timer.Start();
    ITS->Hits2SDigits();
    timer.Stop(); timer.Print();

    delete rl;

    return 0;
};

