/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//_________________________________________________________________________
//*-- Author :  Dmitri Peressounko (SUBATECH & Kurchatov Institute) 
//////////////////////////////////////////////////////////////////////////////
// This TTask performs digitization of Summable digits (in the PHOS case it is just
// the sum of contributions from all primary particles into a given cell). 
// In addition it performs mixing of summable digits from different events.
// The name of the TTask is also the title of the branch that will contain 
// the created SDigits
// The title of the TTAsk is the name of the file that contains the hits from
// which the SDigits are created
//
// For each event two branches are created in TreeD:
//   "PHOS" - list of digits
//   "AliPHOSDigitizer" - AliPHOSDigitizer with all parameters used in digitization
//
// Note, that one can set a title for new digits branch, and repeat digitization with
// another set of parameters.
//
// Use case:
// root[0] AliPHOSDigitizer * d = new AliPHOSDigitizer() ;
// root[1] d->ExecuteTask()             
// Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//                       //Digitizes SDigitis in all events found in file galice.root 
//
// root[2] AliPHOSDigitizer * d1 = new AliPHOSDigitizer("galice1.root") ;  
//                       // Will read sdigits from galice1.root
// root[3] d1->MixWith("galice2.root")       
// Warning in <TDatabasePDG::TDatabasePDG>: object already instantiated
//                       // Reads another set of sdigits from galice2.root
// root[3] d1->MixWith("galice3.root")       
//                       // Reads another set of sdigits from galice3.root
// root[4] d->ExecuteTask("deb timing")    
//                       // Reads SDigits from files galice1.root, galice2.root ....
//                       // mixes them and stores produced Digits in file galice1.root          
//                       // deb - prints number of produced digits
//                       // deb all - prints list of produced digits
//                       // timing  - prints time used for digitization
//

// --- ROOT system ---
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TFolder.h"
#include "TObjString.h"
#include "TBenchmark.h"

// --- Standard library ---
#include <iomanip.h>

// --- AliRoot header files ---

#include "AliRun.h"
#include "AliRunDigitizer.h"
#include "AliPHOSDigit.h"
#include "AliPHOS.h"
#include "AliPHOSLoader.h"
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSTick.h"

ClassImp(AliPHOSDigitizer)


//____________________________________________________________________________ 
  AliPHOSDigitizer::AliPHOSDigitizer() 
{
  // ctor

  fPinNoise           = 0.004 ;
  fEMCDigitThreshold  = 0.012 ;
  fCPVNoise           = 0.01;
  fCPVDigitThreshold  = 0.09 ;
  fTimeResolution     = 0.5e-9 ;
  fTimeSignalLength   = 1.0e-9 ;
  fDigitsInRun  = 0 ; 
  fADCchanelEmc = 0.0015;        // width of one ADC channel in GeV
  fADCpedestalEmc = 0.005 ;      //
  fNADCemc = (Int_t) TMath::Power(2,16) ;  // number of channels in EMC ADC

  fADCchanelCpv = 0.0012 ;          // width of one ADC channel in CPV 'popugais'
  fADCpedestalCpv = 0.012 ;         // 
  fNADCcpv = (Int_t) TMath::Power(2,12);      // number of channels in CPV ADC

  fTimeThreshold = 0.001*10000000 ; //Means 1 MeV in terms of SDigits amplitude
  fManager = 0 ;                        // We work in the standalong mode
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(const char *headerFile,const char * name)
{
  // ctor
  SetName(name) ;
  SetTitle(headerFile) ;
  fManager = 0 ;                     // We work in the standalong mode
  Init() ;
  
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(AliRunDigitizer * ard):AliDigitizer(ard)
{
  // ctor
  SetName("");     //Will call init in the digitizing
  SetTitle("aliroot");
}

//____________________________________________________________________________ 
  AliPHOSDigitizer::~AliPHOSDigitizer()
{
  // dtor


}

//____________________________________________________________________________
void AliPHOSDigitizer::Digitize(TObjArray* sdigArray)
{ 
  
  // Makes the digitization of the collected summable digits.
  //  It first creates the array of all PHOS modules
  //  filled with noise (different for EMC, CPV and PPSD) and
  //  then adds contributions from SDigits. 
  // This design avoids scanning over the list of digits to add 
  // contribution to new SDigits only.

  
  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (rn == 0x0)
   {
     Error("Digitize","Can not get RunLoader from Input Stream 0 folder");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("Digitize","Can not get PHOS Loader");
     return;
   }

  TClonesArray * digits = gime->Digits(); 
  digits->Clear() ;

  const AliPHOSGeometry *geom = gime->PHOSGeometry() ; 

  //Making digits with noise, first EMC
  Int_t nEMC = geom->GetNModules()*geom->GetNPhi()*geom->GetNZ();
  
  Int_t nCPV ;
  Int_t absID ;
  TString name      =  geom->GetName() ;
  
  nCPV = nEMC + geom->GetNumberOfCPVPadsZ()*geom->GetNumberOfCPVPadsPhi()*
    geom->GetNModules() ;

  digits->Expand(nCPV) ;

  // get first the sdigitizer from the tasks list (must have same name as the digitizer)
  const AliPHOSSDigitizer* sDigitizer = dynamic_cast<AliPHOSSDigitizer*>(gime->SDigitizer());
  if ( !sDigitizer) 
   {
     cerr << "ERROR: AliPHOSDigitizer::Digitize -> SDigitizer with name " << GetName() << " not found " << endl ; 
     abort() ; 
   }
    
  // loop through the sdigits posted to the White Board and add them to the noise

  Int_t input = 0 ;

  
  TClonesArray* sdigits;
  //Find the first crystall with signal
  Int_t nextSig = 200000 ; 
  Int_t i;
  for(i=0; i<input; i++)
   {
    sdigits = (TClonesArray *)sdigArray->At(i);
    if ( !sdigits->GetEntriesFast() )
      continue ; 
    Int_t curNext = ((AliPHOSDigit *)sdigits->At(0))->GetId() ;
     if(curNext < nextSig) 
       nextSig = curNext ;
  }

  TArrayI index(input) ;
  index.Reset() ;  //Set all indexes to zero

  AliPHOSDigit * digit ;
  AliPHOSDigit * curSDigit ;

  TClonesArray * ticks = new TClonesArray("AliPHOSTick",1000) ;

  //Put Noise contribution
  for(absID = 1; absID <= nEMC; absID++){
    Float_t noise = gRandom->Gaus(0., fPinNoise) ; 
    new((*digits)[absID-1]) AliPHOSDigit( -1,absID,sDigitizer->Digitize(noise), TimeOfNoise() ) ;
    //look if we have to add signal?
    if(absID==nextSig){
      //Add SDigits from all inputs 
      digit = (AliPHOSDigit *) digits->At(absID-1) ;

      ticks->Clear() ;
      Int_t contrib = 0 ;
      Float_t a = digit->GetAmp() ;
      Float_t b = TMath::Abs( a /fTimeSignalLength) ;
      //Mark the beginnign of the signal
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime(),0, b);  
      //Mark the end of the ignal     
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime()+fTimeSignalLength, -a, -b); 

      //loop over inputs
      for(i=0; i<input; i++)
       {
        if(((TClonesArray *)sdigArray->At(i))->GetEntriesFast() > index[i] )
          curSDigit = (AliPHOSDigit*)((TClonesArray *)sdigArray->At(i))->At(index[i]) ;         
        else
          curSDigit = 0 ;
        //May be several digits will contribute from the same input
        while(curSDigit && curSDigit->GetId() == absID){           
          //Shift primary to separate primaries belonging different inputs
          Int_t primaryoffset ;
          if(fManager)
            primaryoffset = fManager->GetMask(i) ; 
          else
            primaryoffset = 10000000*i ;
          curSDigit->ShiftPrimary(primaryoffset) ;
          
          a = curSDigit->GetAmp() ;
          b = a /fTimeSignalLength ;
          new((*ticks)[contrib++]) AliPHOSTick(curSDigit->GetTime(),0, b);  
          new((*ticks)[contrib++]) AliPHOSTick(curSDigit->GetTime()+fTimeSignalLength, -a, -b); 

          *digit = *digit + *curSDigit ;  //add energies

          index[i]++ ;
          if(((TClonesArray *)sdigArray->At(i))->GetEntriesFast() > index[i] )
            curSDigit = (AliPHOSDigit*)((TClonesArray *)sdigArray->At(i))->At(index[i]) ;         
          else
            curSDigit = 0 ;
        }
      }

      //calculate and set time
      Float_t time = FrontEdgeTime(ticks) ;
      digit->SetTime(time) ;

      //Find next signal module
      nextSig = 200000 ;
      for(i=0; i<input; i++){
        sdigits = ((TClonesArray *)sdigArray->At(i)) ;
        Int_t curNext = nextSig ;
        if(sdigits->GetEntriesFast() > index[i] ){
          curNext = ((AliPHOSDigit *) sdigits->At(index[i]))->GetId() ;
          
        }
        if(curNext < nextSig) nextSig = curNext ;
      }
    }
  }
  
  ticks->Delete() ;
  delete ticks ;


  //Now CPV digits (different noise and no timing)
  for(absID = nEMC+1; absID <= nCPV; absID++){
    Float_t noise = gRandom->Gaus(0., fCPVNoise) ; 
    new((*digits)[absID-1]) AliPHOSDigit( -1,absID,sDigitizer->Digitize(noise), TimeOfNoise() ) ;
    //look if we have to add signal?
    if(absID==nextSig){
      digit = (AliPHOSDigit *) digits->At(absID-1) ;
      //Add SDigits from all inputs
      for(i=0; i<input; i++){
        if(((TClonesArray *)sdigArray->At(i))->GetEntriesFast() > index[i] )
          curSDigit = (AliPHOSDigit*)((TClonesArray *)sdigArray->At(i))->At(index[i]) ;         
        else
          curSDigit = 0 ;

        //May be several digits will contribute from the same input
        while(curSDigit && curSDigit->GetId() == absID){           
          //Shift primary to separate primaries belonging different inputs
          Int_t primaryoffset ;
          if(fManager)
            primaryoffset = fManager->GetMask(i) ; 
          else
            primaryoffset = 10000000*i ;
          curSDigit->ShiftPrimary(primaryoffset) ;

          //add energies
          *digit = *digit + *curSDigit ;  
          index[i]++ ;
          if(((TClonesArray *)sdigArray->At(i))->GetEntriesFast() > index[i] )
            curSDigit = (AliPHOSDigit*)((TClonesArray *)sdigArray->At(i))->At(index[i]) ;         
          else
            curSDigit = 0 ;
        }
      }

      //Find next signal module
      nextSig = 200000 ;
      for(i=0; i<input; i++){
        sdigits = (TClonesArray *)sdigArray->At(i) ;
        Int_t curNext = nextSig ;
        if(sdigits->GetEntriesFast() > index[i] )
          curNext = ((AliPHOSDigit *) sdigits->At(index[i]))->GetId() ;
        if(curNext < nextSig) nextSig = curNext ;
      }
      
    }
  }
  delete sdigArray ; //We should not delete its contents
  
  
  
  //remove digits below thresholds
  for(i = 0; i < nEMC ; i++){
    digit = (AliPHOSDigit*) digits->At(i) ;
    if(sDigitizer->Calibrate( digit->GetAmp() ) < fEMCDigitThreshold)
      digits->RemoveAt(i) ;
    else
      digit->SetTime(gRandom->Gaus(digit->GetTime(),fTimeResolution) ) ;
  }


  for(i = nEMC; i < nCPV ; i++)
    if(sDigitizer->Calibrate(((AliPHOSDigit*)digits->At(i))->GetAmp()) < fCPVDigitThreshold)
      digits->RemoveAt(i) ;
    
  digits->Compress() ;  
  
  Int_t ndigits = digits->GetEntriesFast() ;
  digits->Expand(ndigits) ;

  //Set indexes in list of digits and make true digitization of the energy
  for (i = 0 ; i < ndigits ; i++) { 
    AliPHOSDigit * digit = (AliPHOSDigit *) digits->At(i) ; 
    digit->SetIndexInList(i) ;     
    Float_t energy = sDigitizer->Calibrate(digit->GetAmp()) ;
    digit->SetAmp(DigitizeEnergy(energy,digit->GetId()) ) ;
  }

}
//____________________________________________________________________________
Int_t AliPHOSDigitizer::DigitizeEnergy(Float_t energy, Int_t absId)
{
  Int_t chanel ;
  if(absId <= fEmcCrystals){ //digitize as EMC 
    chanel = (Int_t) TMath::Ceil((energy - fADCpedestalEmc)/fADCchanelEmc) ;       
    if(chanel > fNADCemc ) chanel =  fNADCemc ;
  }
  else{ //Digitize as CPV
    chanel = (Int_t) TMath::Ceil((energy - fADCpedestalCpv)/fADCchanelCpv) ;       
    if(chanel > fNADCcpv ) chanel =  fNADCcpv ;
  }
  return chanel ;
}
//____________________________________________________________________________
void AliPHOSDigitizer::Exec(Option_t *option) 
{ 
  // Managing method
  if(strcmp(GetName(), "") == 0 )   
    Init() ;
  
  if (strstr(option,"print")) {
    Print("");
    return ; 
  }
  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSDigitizer");
  
  if(fManager == 0x0)
   {
     Fatal("Exec","Can not find RunDigitizer");
     return;
   }

  Int_t input ;
  TObjArray * sdigArray = new TObjArray(2);
  TClonesArray * sdigits = 0 ;

  for(input = 0 ; input < fManager->GetNinputs(); input ++)
   {
    AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(input));
    if (rn == 0x0)
     {
       Error("Digitize","Can not get RunLoader from Input Stream %s folder",input);
       return;
     }
    AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
    if (gime == 0x0)
     {
       Error("Digitize","Can not get PHOS Loader");
       return;
     }

    gime->LoadSDigits("READ");
    sdigits = gime->SDigits();
    
    if(!sdigits)
     {
       cerr << "AliPHOSDigitizer -> No Input " << endl ;
       return ;
     }
    sdigArray->AddAt(sdigits, input) ;
   }
  

  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (rn == 0x0)
   {
     Error("Digitize","Can not get RunLoader from Output Stream folder");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("Digitize","Can not get PHOS Loader");
     return;
   }

  gime->LoadDigits("UPDATE");

  
  TTree * treeD = gime->TreeD();

  if(treeD == 0 ){
    cerr << " AliPHOSDigitizer :: Can not find TreeD " << endl ;
    return ;
  }

  //Check, if this branch already exits
  TObjArray * lob = (TObjArray*)treeD->GetListOfBranches() ;
  TIter next(lob) ; 
  TBranch * branch = 0 ;  
  Bool_t phosfound = kFALSE, 
  digitizerfound = kFALSE ; 
  
  while ( (branch = (TBranch*)next()) && (!phosfound || !digitizerfound) ) 
   {
    if ( (strcmp(branch->GetName(), "PHOS")==0) && 
         (strcmp(branch->GetTitle(), GetName())==0) ) 
      phosfound = kTRUE ;
    
    else if ( (strcmp(branch->GetName(), "AliPHOSDigitizer")==0) && 
              (strcmp(branch->GetTitle(), GetName())==0) ) 
      digitizerfound = kTRUE ; 
   }

  if ( phosfound ) 
   {
     cerr << "WARNING: AliPHOSDigitizer -> Digits branch with name " << GetName() 
          << " already exits" << endl ;
     return ; 
   }   
  if ( digitizerfound ) 
   {
    cerr << "WARNING: AliPHOSDigitizer -> Digitizer branch with name " << GetName() 
         << " already exits" << endl ;
    return ; 
   }   

  Digitize(sdigArray); //Add prepared SDigits to digits and add the noise
  WriteDigits();
  if(strstr(option,"deb"))
      PrintDigits(option);
    
  
  if(strstr(option,"tim"))
   {
    gBenchmark->Stop("PHOSDigitizer");
    cout << "AliPHOSDigitizer:" << endl ;
    cout << "  took " << gBenchmark->GetCpuTime("PHOSDigitizer") << " seconds for Digitizing " 
         <<  gBenchmark->GetCpuTime("PHOSDigitizer")<< " seconds per event " << endl ;
    cout << endl ;
  }
  
}

//____________________________________________________________________________ 
Float_t AliPHOSDigitizer::FrontEdgeTime(TClonesArray * ticks) 
{ // 
  ticks->Sort() ; //Sort in accordance with times of ticks
  TIter it(ticks) ;
  AliPHOSTick * ctick = (AliPHOSTick *) it.Next() ;
  Float_t time = ctick->CrossingTime(fTimeThreshold) ;    

  AliPHOSTick * t ;  
  while((t=(AliPHOSTick*) it.Next())){
    if(t->GetTime() < time)  //This tick starts before crossing
      *ctick+=*t ;
    else
      return time ;

    time = ctick->CrossingTime(fTimeThreshold) ;    
  }
  return time ;
}
//____________________________________________________________________________ 
Bool_t AliPHOSDigitizer::Init()
{
  fPinNoise           = 0.004 ;
  fEMCDigitThreshold  = 0.012 ;
  fCPVNoise           = 0.01;
  fCPVDigitThreshold  = 0.09 ;
  fTimeResolution     = 0.5e-9 ;
  fTimeSignalLength   = 1.0e-9 ;
  fDigitsInRun  = 0 ; 
  fADCchanelEmc = 0.0015;        // width of one ADC channel in GeV
  fADCpedestalEmc = 0.005 ;      //
  fNADCemc = (Int_t) TMath::Power(2,16) ;  // number of channels in EMC ADC

  fADCchanelCpv = 0.0012 ;          // width of one ADC channel in CPV 'popugais'
  fADCpedestalCpv = 0.012 ;         // 
  fNADCcpv = (Int_t) TMath::Power(2,12);      // number of channels in CPV ADC

  fTimeThreshold = 0.001*10000000; //Means 1 MeV in terms of SDigits amplitude
    
  if(fManager)
    SetTitle("aliroot");
  else 
   if (strcmp(GetTitle(),"")==0) 
    SetTitle("galice.root") ;

  if( strcmp(GetName(), "") == 0 )
    SetName("Default") ;
  
  
  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetInputFolderName(0));
  if (rn == 0x0)
   {
     Error("Digitize","Can not get RunLoader from Input Stream 0 folder");
     return kFALSE;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("Init","Can not get PHOS Loader");
     return kFALSE;
   }
  
  const AliPHOSGeometry * geom = gime->PHOSGeometry() ;
  fEmcCrystals = geom->GetNModules() *  geom->GetNCristalsInModule() ;
  
  return kTRUE ;
}

//__________________________________________________________________

//__________________________________________________________________
void AliPHOSDigitizer::Print(Option_t* option)const {
  // Print Digitizer's parameters
  if( strcmp(GetName(), "") != 0 )
   {
    
    cout << "------------------- "<< GetName() << " -------------" << endl ;
    cout << "Digitizing sDigits from file(s): " <<endl ;
    
     TCollection * folderslist = ((TFolder*)gROOT->FindObjectAny("Folders/RunMC/Event/Data/PHOS/SDigits"))->GetListOfFolders() ; 
    TIter next(folderslist) ; 
    TFolder * folder = 0 ; 
    
    while ( (folder = (TFolder*)next()) ) {
      if ( folder->FindObject(GetName())  ) 
        cout << "Adding SDigits " << GetName() << " from " << folder->GetName() << endl ; 
    }
    cout << endl ;
    cout << "Writing digits to " << GetTitle() << endl ;
    
    cout << endl ;
    cout << "With following parameters: " << endl ;
    cout << "     Electronics noise in EMC (fPinNoise) = " << fPinNoise << endl ;
    cout << "  Threshold  in EMC  (fEMCDigitThreshold) = " << fEMCDigitThreshold  << endl ; ;
    cout << "                 Noise in CPV (fCPVNoise) = " << fCPVNoise << endl ; 
    cout << "    Threshold in CPV (fCPVDigitThreshold) = " << fCPVDigitThreshold << endl ; 
    cout << "---------------------------------------------------" << endl ;
  }
  else
    cout << "AliPHOSDigitizer not initialized " << endl ;
  
}

//__________________________________________________________________
void AliPHOSDigitizer::PrintDigits(Option_t * option){
  // Print a table of digits

  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (rn == 0x0)
   {
     Error("PrintDigits","Can not get RunLoader from Output Stream folder");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("PrintDigits","Can not get PHOS Loader");
     return;
   }

  TClonesArray * digits = gime->Digits() ; 

  cout << "AliPHOSDigitiser: event " << gAlice->GetEvNumber() << endl ;
  cout << "       Number of entries in Digits list " << digits->GetEntriesFast() << endl ;
  cout << endl ;
  if(strstr(option,"all")||strstr(option,"EMC")){
    
    //loop over digits
    AliPHOSDigit * digit;
    cout << "EMC digits (with primaries): " << endl ;
    cout << "Digit Id     Amplitude   Index       Nprim  Primaries list " <<  endl;      
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; (index < digits->GetEntriesFast()) && 
         (((AliPHOSDigit * )  digits->At(index))->GetId() <= maxEmc) ; index++) {
      digit = (AliPHOSDigit * )  digits->At(index) ;
      if(digit->GetNprimary() == 0) continue;
      cout << setw(6)  <<  digit->GetId() << "   "  <<         setw(10)  <<  digit->GetAmp() <<   "    "  
           << setw(6)  <<  digit->GetIndexInList() << "    "   
           << setw(5)  <<  digit->GetNprimary() <<"    ";
      
      Int_t iprimary;
      for (iprimary=0; iprimary<digit->GetNprimary(); iprimary++)
        cout << setw(5)  <<  digit->GetPrimary(iprimary+1) << "    ";
      cout << endl;           
    }    
    cout << endl;
  }

  if(strstr(option,"all")||strstr(option,"CPV")){
    
    //loop over CPV digits
    AliPHOSDigit * digit;
    cout << "CPV digits: " << endl ;
    cout << "Digit Id     Amplitude   Index       Nprim  Primaries list " <<  endl;      
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->PHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; index < digits->GetEntriesFast(); index++) {
      digit = (AliPHOSDigit * )  digits->At(index) ;
      if(digit->GetId() > maxEmc){
        cout << setw(6)  <<  digit->GetId() << "   "  <<         setw(10)  <<  digit->GetAmp() <<   "    "  
             << setw(6)  <<  digit->GetIndexInList() << "    "   
             << setw(5)  <<  digit->GetNprimary() <<"    ";
        
        Int_t iprimary;
        for (iprimary=0; iprimary<digit->GetNprimary(); iprimary++)
          cout << setw(5)  <<  digit->GetPrimary(iprimary+1) << "    ";
        cout << endl;           
      }    
    }
  }

}

//__________________________________________________________________
void AliPHOSDigitizer::SetSDigitsBranch(const char* title)
{
  // we set title (comment) of the SDigits branch in the first! header file
  if( strcmp(GetName(), "") == 0 )
    Init() ;

  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (rn == 0x0)
   {
     Error("SetSDigitsBranch","Can not get RunLoader from Output Stream folder");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("SetSDigitsBranch","Can not get PHOS Loader");
     return;
   }

  gime->SDigits()->SetName(title) ; 
 
}
//__________________________________________________________________
Float_t AliPHOSDigitizer::TimeOfNoise(void)
{  // Calculates the time signal generated by noise
  //to be rewritten, now returns just big number
  return 1. ;

}
//____________________________________________________________________________
void AliPHOSDigitizer::Reset() 
{ 
  // sets current event number to the first simulated event

  if( strcmp(GetName(), "") == 0 )
    Init() ;

 //  Int_t inputs ;
//   for(inputs = 0; inputs < fNinputs ;inputs++)
//       fIevent->AddAt(-1, inputs ) ;
  
}

//____________________________________________________________________________
void AliPHOSDigitizer::WriteDigits()
{
// Gets TreeD from Folder
// Check if branch already exists: 
//   if yes, exit without writing: ROOT TTree does not support overwriting/updating of 
//      already existing branches.
//   else creates branch with Digits, named "PHOS", title "...",
//      saves "AliPHOSDigitizer", with the same title to keep all the parameters
//      and names of files, from which digits are made.

  AliRunLoader* rn = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  if (rn == 0x0)
   {
     Error("Digitize","Can not get RunLoader from Output Stream folder");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rn->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("Digitize","Can not get PHOS Loader");
     return;
   }
  const TClonesArray * digits = gime->Digits();//get TClonesArray from folder
  TTree * treeD = gime->TreeD(); //get tree from folder
  
  if ((digits==0x0)||(treeD==0x0))
   {
     Error("WriteDigits","Can not get digits Clones Array or Digits Tree from Output Loader.");
     return;
   }
  
  // create new branches
  // -- create Digits branch
  
  Int_t bufferSize = 32000 ;    
  TBranch * digitsBranch = treeD->Branch("PHOS",&digits,bufferSize);//create branch
  digitsBranch->SetTitle(GetName());
  digitsBranch->Fill();
  treeD->AutoSave() ; // Write(0,kOverwrite) ;  

  gime->WriteDigits("OVERWRITE");//Ask PHOS loader to write tree with digits to file
  gime->WriteDigitizer("OVERWRITE");//Ask PHOS loader to write digitizer task to file 
                                    //reads from folder, Task was posted by RunDigizer
 
}
