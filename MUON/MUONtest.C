void MUONtestabso (Int_t evNumber1=0,Int_t evNumber2=0) 
{
/////////////////////////////////////////////////////////////////////////
//   This macro is a small example of a ROOT macro
//   illustrating how to read the output of GALICE
//   and do some analysis.
//   
/////////////////////////////////////////////////////////////////////////

// Dynamically link some shared libs                    

    if (gClassTable->GetID("AliRun") < 0) {
	gSystem->Load("$ALITOP/cern.so/lib/libpdfDUMMY.so");
	gSystem->Load("$ALITOP/cern.so/lib/libPythia.so");
	gSystem->Load("$ROOTSYS/lib/libEG.so");       
	gSystem->Load("$ROOTSYS/lib/libEGPythia.so");       
	gSystem->Load("libGeant3Dummy.so");    //a dummy version of Geant3
	gSystem->Load("PHOS/libPHOSdummy.so"); //the standard Alice classes 
	gSystem->Load("libgalice.so");         // the standard Alice classes 
    }
// Connect the Root Galice file containing Geometry, Kine and Hits

    TFile *file = (TFile*)gROOT->GetListOfFiles()->FindObject("galice.root");

    
    if (!file) {
	printf("\n Creating galice.root \n");
	file = new TFile("galice.root");
    } else {
	printf("\n galice.root found in file list");
    }
    file->ls();
    
// Get AliRun object from file or create it if not on file
    if (!gAlice) {
	gAlice = (AliRun*)(file->Get("gAlice"));
	if (gAlice) printf("AliRun object found on file\n");
	if (!gAlice) {
	    printf("\n create new gAlice object");
	    gAlice = new AliRun("gAlice","Alice test program");
	}
    }
    
//  Create some histograms


   TH1F *zv = new TH1F("zv","z vertex"
			   ,100,468.,503.);
   TH1F *xv = new TH1F("xv","x vertex"
			   ,100,0.,50.);
   TH1F *yv  = new TH1F("yv","y vertex"
			   ,100,0.,50.);

   TH1F *ip  = new TH1F("ip","ipart"
			   ,100,0.,50.);

   AliMUONchamber*  iChamber;
//
//   Loop over events 
//
   Int_t Nh=0;
   Int_t Nh1=0;
   for (Int_t nev=0; nev<= evNumber2; nev++) {
       cout << "nev         " << nev <<endl;
       Int_t nparticles = gAlice->GetEvent(nev);
       cout << "nparticles  " << nparticles <<endl;
       if (nev < evNumber1) continue;
       if (nparticles <= 0) return;

       AliMUON *MUON  = (AliMUON*) gAlice->GetModule("MUON");
	   printf("\n nev %d \n ", nev);

       TTree *TH = gAlice->TreeH();
       Int_t ntracks = TH->GetEntries();
//
//   Loop over tracks
//
       for (Int_t track=0; track<ntracks;track++) {
	   
	   gAlice->ResetHits();
	   Int_t nbytes += TH->GetEvent(track);
	   if (MUON)  {
	       for(AliMUONhit* mHit=(AliMUONhit*)MUON->FirstHit(-1); 
		   mHit;
		   mHit=(AliMUONhit*)MUON->NextHit()) 
	       {
		   Int_t   nch   = mHit->fChamber;  // chamber number
		   Float_t x     = mHit->fX;        // x-pos of hit
		   Float_t y     = mHit->fY;        // y-pos
		   Float_t z     = mHit->fZ;        // y-pos
               
                if (nch > 1) continue;

		Int_t ipart = mHit->fParticle;
		TClonesArray *fPartArray = gAlice->Particles();
		TParticle *Part;
		Int_t ftrack = mHit->fTrack;
		Int_t id = ((TParticle*) fPartArray->UncheckedAt(ftrack))->GetPdgCode();
		  ip->Fill((float)ipart,(float) 1);

		if (ipart != 3) continue;
		  Float_t xv = Part->Vx();      //  vertex  
		  Float_t yv = Part->Vy();      //  vertex
		  Float_t zv  = Part->Vz();      // z vertex 
		  xv->Fill(xv,(float) 1);
		  yv->Fill(yv,(float) 1);
		  zv->Fill(zv,(float) 1);
	       }

	   }
       }
   }

   
//Create a canvas, set the view range, show histograms
   TCanvas *c1 = new TCanvas("c1","Charge and Residuals",400,10,600,700);
   pad11 = new TPad("pad11"," ",0.01,0.51,0.49,0.99);
   pad12 = new TPad("pad12"," ",0.51,0.51,0.99,0.99);
   pad13 = new TPad("pad13"," ",0.01,0.01,0.49,0.49);
   pad14 = new TPad("pad14"," ",0.51,0.01,0.99,0.49);
   pad11->SetFillColor(11);
   pad12->SetFillColor(11);
   pad13->SetFillColor(11);
   pad14->SetFillColor(11);
   pad11->Draw();
   pad12->Draw();
   pad13->Draw();
   pad14->Draw();
   
   pad11->cd();
   ip->SetFillColor(42);
   ip->SetXTitle("ipart");
   ip->Draw();

   pad12->cd();
   xv->SetFillColor(42);
   xv->SetXTitle("xvert");
   xv->Draw();

   pad13->cd();
   yv->SetFillColor(42);
   yv->SetXTitle("yvert");
   yv->Draw();

   pad14->cd();
   zv->SetFillColor(42);
   zv->SetXTitle("zvert");
   zv->Draw();
   
}
