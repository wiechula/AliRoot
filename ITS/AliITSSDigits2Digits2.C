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
void AliITSSDigits2Digits2(const char *inFile = "galice.root"){

  TClonesArray * sdig = new TClonesArray( "AliITSpListItem",1000 );

    // Connect the Root Galice file
  
  TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject(inFile);
  if (file) {file->Close(); delete file;}
  AliRunLoader *rl = AliRunLoader::Open(inFile,"Event","read");
  if (!rl) 
    {
      cerr<<"Can't load RunLoader from "<<inFile<<" !\n";
      return;
    }

  cout << "AliITSSDigits2Digits2" << endl;

    // Get AliRun object from file or return if not on file
  if (gAlice) { delete gAlice; gAlice = 0;}
  rl->LoadgAlice();
  gAlice = rl->GetAliRun();
  if (!gAlice) {
	cerr << "AliITSSDigits2Digits2.C : AliRun object not found on file"
         << endl;
	return;
  } // end if !gAlice

  rl->GetEvent(0);
  AliITS *ITS = (AliITS*)gAlice->GetDetector("ITS");      
  if (!ITS) {
	cerr<<"AliITSSDigits2Digits2.C : AliITS object not found on file"
	    << endl;
	return;
  } 
 
  ITS->SetDefaultSimulation();

  AliLoader * itsl = rl->GetLoader("ITSLoader");
  rl->LoadHeader();                           // load header
  cout<<"MACRO: header loaded.......................\n";
  itsl->LoadHits("read");                     // load hits (read mode)
  cout<<"MACRO: hits loaded.........................\n";
  itsl->LoadSDigits("read");                  // load SDigtis (read m.)
  cout<<"MACRO: summable digits loaded..............\n";
  itsl->LoadDigits("update");                 // load digits
  cout<<"MACRO: digits loaded.......................\n";

  if(!(ITS->GetITSgeom())){
	cerr << " AliITSgeom not found. Can't digitize with out it." << endl;
	return;
  }

  if(!itsl->TreeD()){ 
	cout << "MACRO: Having to create the Digits Tree." << endl;
	itsl->MakeTree("D");
  } 
  ITS->MakeBranchInTreeD(itsl->TreeD());
  TTree *treeS = itsl->TreeS();
  ITS->SetTreeAddress();
           
  if( !(treeS && ITS->GetSDigits()) ) {
    cerr<<"Fatal problem either with TreeS or with SDigits:\n";
    cerr<<"TreeS address: "<<treeS<<endl;
    cerr<<"SDigits address" <<ITS->GetSDigits()<<endl;
  }
  TBranch *brchSDigits = treeS->GetBranch( ITS->GetName() );
  if( brchSDigits ) 
    {
      brchSDigits->SetAddress( &sdig ); 
    } else {
      cerr<<"AliITSSDigits2Digits2: branch ITS not found in TreeS"<<endl;
      return;
    } // end if brchSDigits
  for(Int_t module=0; module<ITS->GetITSgeom()->GetIndexMax(); module++ )
    {
      Int_t id = ITS->GetITSgeom()->GetModuleType(module);
      AliITSDetType *iDetType = ITS->DetType( id );


      AliITSsimulation *sim = (AliITSsimulation*)iDetType->GetSimulationModel();
      if(!sim) {
        cerr<<"AliITSSdigits2Digits2: The simulation class was not instantiated!"<<endl;
        return;
      } // end if !sim

      // Fill the module with the sum of SDigits
      sim->InitSimulationModule(module, rl->GetHeader()->GetEvent());
      
      sdig->Clear();
      // add summable digits to module
      brchSDigits->GetEvent( module );
      sim->AddSDigitsToModule(sdig,0);
      // Digitize current module sum(SDigits)->Digits
      sim->FinishSDigitiseModule();

      // fills all branches - wasted disk space
      itsl->TreeD()->Fill();
      ITS->ResetDigits();
    } // end for module

  itsl->TreeD()->AutoSave();
  itsl->WriteDigits("OVERWRITE");
  sdig->Clear();
  delete sdig;
}


