#include "iostream.h"

void MUONdigit (Int_t evNumber1=0,Int_t evNumber2=0,Int_t nsignal  =25) 
{
/////////////////////////////////////////////////////////////////////////
//   This macro is a small example of a ROOT macro
//   illustrating how to read the output of GALICE
//   and do some analysis.
//   
/////////////////////////////////////////////////////////////////////////

// Dynamically link some shared libs

   if (gClassTable->GetID("AliRun") < 0) {
      gROOT->LoadMacro("loadlibs.C");
      loadlibs();
   }


// Connect the Root Galice file containing Geometry, Kine and Hits

   TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject("galice.root");
   if (file) file->Close(); 
   file = new TFile("galice.root","UPDATE");
   file->ls();

   printf ("I'm after Map \n");

// Get AliRun object from file or create it if not on file

   if (!gAlice) {
       gAlice = (AliRun*)file->Get("gAlice");
       if (gAlice) printf("AliRun object found on file\n");
       if (!gAlice) gAlice = new AliRun("gAlice","Alice test program");
   }
   printf ("I'm after gAlice \n");
   
   AliMUON *MUON  = (AliMUON*) gAlice->GetModule("MUON");
//
// Event Loop
//
   Int_t nbgr_ev=0;
   
   for (int nev=0; nev<= evNumber2; nev++) {
       Int_t nparticles = gAlice->GetEvent(nev);
       cout << "nev         " <<nev<<endl;
       cout << "nparticles  " <<nparticles<<endl;
       if (nev < evNumber1) continue;
       if (nparticles <= 0) return;

       Int_t nbgr_ev=Int_t(nev/nsignal);
//       printf("nbgr_ev %d\n",nbgr_ev);
       //if (MUON) MUON->Digitise(nev,nbgr_ev,"Add"," ","galice_bgr.root");
       if (MUON) MUON->Digitise(nev,nbgr_ev,"rien"," ","galice_bgr.root");  
//       char hname[30];
//       sprintf(hname,"TreeD%d",nev);
//       file->ls();
   } // event loop 
   file->Close();
}














