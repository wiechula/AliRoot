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
#include "TGeometry.h"
#include "TBenchmark.h"

// --- Standard library ---
#include <iomanip.h>

// --- AliRoot header files ---
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliHeader.h"
#include "AliStream.h"
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
  InitParameters() ; 
  fDefaultInit = kTRUE ;
  fManager = 0 ;                     // We work in the standalong mode

 }

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(const char *eventFolderName, const char * name)
  :AliDigitizer(name,eventFolderName)
{
  // ctor
  InitParameters(); 
  Init();
  fDefaultInit = kFALSE;
}

//____________________________________________________________________________ 
AliPHOSDigitizer::AliPHOSDigitizer(AliRunDigitizer * ard):AliDigitizer(ard)
{
  // ctor
  SetTitle(ard->GetOutputFolderName()) ;
  InitParameters() ; 
  fDefaultInit = kFALSE ; 

  SetName(ard->GetOutputFile().Data());
}

//____________________________________________________________________________ 
  AliPHOSDigitizer::~AliPHOSDigitizer()
{
  // dtor
     
}

//____________________________________________________________________________
void AliPHOSDigitizer::Digitize(TObjArray * sdigArray)
{ 
  // Makes the digitization of the collected summable digits.
  //  It first creates the array of all PHOS modules
  //  filled with noise (different for EMC, CPV and PPSD) and
  //  then adds contributions from SDigits. 
  // This design avoids scanning over the list of digits to add 
  // contribution to new SDigits only.
  AliRunLoader* rl;
  if (fManager)  rl = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
  else rl = AliRunLoader::GetRunLoader(GetTitle());
  if (rl == 0x0)
   {
     Error("Digitize","Can not get RunLoader");
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rl->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("Digitize","Can not get PHOS Loader");
     return;
   }
  
  TClonesArray * digits = gime->Digits();
  if (digits == 0x0)
   {
     gime->LoadDigits("update");
     digits = gime->Digits();
   }
   
  if (digits == 0x0) Fatal("Digitize","Can not get array for digits from out RL");
  
  if (gime->TreeD() == 0x0) gime->MakeDigitsContainer();
  
  digits->Clear() ;

  const AliPHOSGeometry *geom = gime->GetPHOSGeometry(); 
  //Making digits with noise, first EMC
  Int_t nEMC = geom->GetNModules()*geom->GetNPhi()*geom->GetNZ();
  
  Int_t nCPV ;
  Int_t absID ;
  TString name      =  geom->GetName() ;
  
  nCPV = nEMC + geom->GetNumberOfCPVPadsZ()*geom->GetNumberOfCPVPadsPhi()*
    geom->GetNModules() ;

  digits->Expand(nCPV) ;

  // get first the sdigitizer from the tasks list (must have same name as the digitizer)
  AliPHOSSDigitizer* sDigitizer = dynamic_cast<AliPHOSSDigitizer*>(gime->SDigitizer());
  if ( !sDigitizer)
   { //first try to load
     gime->LoadSDigitizer();
     TTask* sd = gime->SDigitizer();
     if (sd == 0x0) cout<<"ERRRRRRRRRRRRRRRR NO SDigitizer\n";
     
     sDigitizer = dynamic_cast<AliPHOSSDigitizer*>(sd);
     
     if ( !sDigitizer ) 
       Fatal("Digitize","SDigitizer with name %s not found",GetName()); 
   }
   
  cout<<" OK. we have sdigitizer\n";   
  TClonesArray * sdigits = 0 ;

  //Find the first crystall with signal
  Int_t nextSig = 200000 ; 
  Int_t i;
  Int_t input = sdigArray->GetEntries();
  cout<<"AliPHOSDigitizer::Digitize: found "<<input<<" input SDigits arrays\n";
  for(i=0; i<input; i++) 
   {
    sdigits = (TClonesArray *)sdigArray->At(i) ;
    if ( !sdigits->GetEntriesFast() )
      continue ; 
    Int_t curNext = ((AliPHOSDigit *)sdigits->At(0))->GetId() ;
     if(curNext < nextSig) 
       nextSig = curNext ;
   }

  TArrayI index(input);
  index.Reset() ;  //Set all indexes to zero

  AliPHOSDigit * digit ;
  AliPHOSDigit * curSDigit ;

  TClonesArray * ticks = new TClonesArray("AliPHOSTick",1000) ;

  cout<<"Put Noise contribution\n";
  //Put Noise contribution
  for(absID = 1; absID <= nEMC; absID++){
    Float_t noise = gRandom->Gaus(0., fPinNoise) ; 
    new((*digits)[absID-1]) AliPHOSDigit( -1,absID,sDigitizer->Digitize(noise), TimeOfNoise() ) ;
    //look if we have to add signal?
    digit = (AliPHOSDigit *) digits->At(absID-1) ;
 
    if(absID==nextSig){
      //Add SDigits from all inputs 
      ticks->Clear() ;
      Int_t contrib = 0 ;
      Float_t a = digit->GetAmp() ;
      Float_t b = TMath::Abs( a /fTimeSignalLength) ;
      //Mark the beginnign of the signal
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime(),0, b);  
      //Mark the end of the ignal     
      new((*ticks)[contrib++]) AliPHOSTick(digit->GetTime()+fTimeSignalLength, -a, -b); 

      //loop over inputs
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
  
  //remove digits below thresholds
  for(i = 0; i < nEMC ; i++){
    digit = dynamic_cast<AliPHOSDigit*>( digits->At(i) ) ;
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
    digit = dynamic_cast<AliPHOSDigit*>( digits->At(i) ) ; 
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

  cout<<"\n\n    DIGITIZATION    \n\n";
  
  if(strcmp(GetName(), "") == 0 )   
    Init() ;
  if (strstr(option,"print")) {
    Print("");
    return ; 
  }
  
  if(strstr(option,"tim"))
    gBenchmark->Start("PHOSDigitizer");

  AliRunLoader *inrl, *outrl;//sk
  AliPHOSLoader *ingime, *outgime;//sk
  
  Int_t nevents ;
  TTree * treeD ;

  cout<<"\nAliPHOSDigitizer::Exec: Performing existance check\n";
  if(fManager)
   {
    cout<<"    AliPHOSDigitizer::Exec: reading data from manager\n";
    outrl = AliRunLoader::GetRunLoader(fManager->GetOutputFolderName());
    if (outrl == 0x0)
     {
       Error("Exec","Can not get RunLoader from Output Stream folder");
       return;
     }
    outgime = dynamic_cast<AliPHOSLoader*>(outrl->GetLoader("PHOSLoader"));
    if (outgime == 0x0)
     {
       Error("Exec","Can not get PHOS Loader");
       return;
     }
    
    treeD = outgime->TreeD();
    if (treeD == 0x0)
     {
      outgime->LoadDigits("update");
      treeD = outgime->TreeD();
     } 
    
    nevents = 1 ;    // Will process only one event
    
    //Check, if this branch already exits
    if (treeD) 
     { 
      TObjArray * lob = (TObjArray*)treeD->GetListOfBranches() ;
      TIter next(lob) ; 
      TBranch * branch = 0 ;  
      Bool_t phosfound = kFALSE, digitizerfound = kFALSE ; 
      
      while ( (branch = (TBranch*)next()) && (!phosfound || !digitizerfound) ) {
        if ( (strcmp(branch->GetName(), "PHOS")==0) && 
             (strcmp(branch->GetTitle(), GetName())==0) ) 
          phosfound = kTRUE ;
        
        else if ( (strcmp(branch->GetName(), "AliPHOSDigitizer")==0) && 
                  (strcmp(branch->GetTitle(), GetName())==0) ) 
          digitizerfound = kTRUE ; 
      }
      
      if ( phosfound ) {
        cerr << "WARNING: AliPHOSDigitizer -> Digits branch with name " << GetName() 
             << " already exits" << endl ;
        return ; 
      }   
      if ( digitizerfound ) {
        cerr << "WARNING: AliPHOSDigitizer -> Digitizer branch with name " << GetName() 
             << " already exits" << endl ;
        return ; 
      }
    }   
  }
  else 
  { //PHOS standalone
    cout<<"    AliPHOSDigitizer::Exec: PHOS standalone reading data from output\n";
    outrl = AliRunLoader::GetRunLoader(GetTitle());
    if (outrl == 0x0)
     {
       Error("Exec","Can not get Run Loader from folder %s",GetTitle());
       return;
     }
    outgime = dynamic_cast<AliPHOSLoader*>(outrl->GetLoader("PHOSLoader"));
    if (outgime == 0x0)
     {
       Error("Exec","Can not get PHOS Loader");
       return;
     }
    outrl->GetEvent(0);
    nevents = outrl->GetNumberOfEvents();
    if (outgime->TreeS() == 0x0) outgime->LoadSDigits("read"); //    gime->Event(ievent,"S");
  }

  cout<<"AliPHOSDigitizer::Exec: Existance check done\n";
  cout<<"\nAliPHOSDigitizer::Exec: Reading data\n";
  
  Int_t ievent;

  TObjArray* sdigArray = new TObjArray(2);

  for(ievent = 0; ievent < nevents; ievent++)
   {
    if(fManager)
     {
      cout<<"     AliPHOSDigitizer::Exec: Reading from event "<<ievent<<" MANAGER mode\n";
      Int_t input ;
      for(input = 0 ; input < fManager->GetNinputs(); input ++)
       {
         inrl =  AliRunLoader::GetRunLoader(fManager->GetInputFolderName(input));
         if (inrl == 0x0)
          {
            Error("Exec","Can not get RunLoader from Input %d Stream folder",input);
            return;
          }
  
         ingime = dynamic_cast<AliPHOSLoader*>(inrl->GetLoader("PHOSLoader"));
         if (ingime == 0x0)
          {
            Error("Exec","Can not get PHOS Loader for input %d.",input);
            return;
          }
         ingime->LoadSDigits();
         TClonesArray* sdigs = ingime->SDigits();
         if (sdigs == 0x0)
          {
            Error("Exec","Can not get PHOS SDigits for input %d.",input);
            return;
          }
         sdigArray->AddAt(sdigs, input);
      }
     }
    else
     {
       cout<<"     AliPHOSDigitizer::Exec: Reading event "<<ievent<<" NON manager mode\n";
       
       outrl->GetEvent(ievent);
       TClonesArray* sdigs =outgime->SDigits();
       if (sdigs == 0x0)
        {
          Error("Exec","Can not get PHOS SDigits (stand alone mode).");
          return;
        }
       sdigArray->AddAt(sdigs,0);
     }
    cout<<"AliPHOSDigitizer::Exec: Data are Read\n\n Calling Digitize()\n";
    
    Digitize(sdigArray) ; //Add prepared SDigits to digits and add the noise
    
    cout<<"AliPHOSDigitizer::Exec: Digitize() exited\n Writing data ....\n";
    
    WriteDigits();   
    cout<<"AliPHOSDigitizer::Exec: Data Written\n";

        
    if(strstr(option,"deb"))    PrintDigits(option);
    
    //increment the total number of Digits per run 
    fDigitsInRun += outgime->Digits()->GetEntriesFast();
    sdigArray->Clear();
  }
  
  //unload data to free memory 
  cout<<"AliPHOSDigitizer::Exec: Unloading Input(s)\n";
  if (fManager)
   {
      Int_t input ;
      for(input = 0 ; input < fManager->GetNinputs(); input ++)
       {
         inrl =  AliRunLoader::GetRunLoader(fManager->GetInputFolderName(input));
         if (inrl == 0x0)
          {
            Error("Exec","Can not get RunLoader from Input %d Stream folder",input);
            return;
          }
  
         ingime = dynamic_cast<AliPHOSLoader*>(inrl->GetLoader("PHOSLoader"));
         if (ingime == 0x0)
          {
            Error("Exec","Can not get PHOS Loader for input %d.",input);
            return;
          }
         ingime->UnloadSDigits();
       }
   }
  else
   {
     outgime->UnloadSDigits();
   }
  outgime->UnloadDigits();
  
  if(strstr(option,"tim")){
    gBenchmark->Stop("PHOSDigitizer");
    cout << "AliPHOSDigitizer:" << endl ;
    cout << "  took " << gBenchmark->GetCpuTime("PHOSDigitizer") << " seconds for Digitizing " 
         <<  gBenchmark->GetCpuTime("PHOSDigitizer")/nevents << " seconds per event " << endl ;
    cout << endl ;
  }
 
 cout<<"\n\n\nExec: FINISHED DIGITIZATION\n\n";
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
  // Makes all memory allocations
 return kFALSE;
}

//____________________________________________________________________________ 
void AliPHOSDigitizer::InitParameters()
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

  fTimeThreshold = 0.001*10000000 ; //Means 1 MeV in terms of SDigits amplitude
    
}

//__________________________________________________________________
void AliPHOSDigitizer::MixWith(const char* headerFile)
{
  // Allows to produce digits by superimposing background and signal event.
  // It is assumed, that headers file with SIGNAL events is opened in 
  // the constructor. 
  // Sets the BACKGROUND event, with which the SIGNAL event is to be mixed 
  // Thus we avoid writing (changing) huge and expensive 
  // backgound files: all output will be writen into SIGNAL, i.e. 
  // opened in constructor file. 
  //
  // One can open as many files to mix with as one needs.
  // However only Sdigits with the same name (i.e. constructed with the same SDigitizer)
  // can be mixed.

  if(fManager){
    cout << "Can not use this method under AliRunDigitizer " << endl ;
    return ;
  }

  if( strcmp(GetName(), "") == 0 )
    Init() ;

}

//__________________________________________________________________
void AliPHOSDigitizer::Print(Option_t* option)const {
  // Print Digitizer's parameters
  if( strcmp(GetName(), "") != 0 )
  {


    cout << "------------------- "<< GetName() << " -------------" << endl ;

    const Int_t nStreams = fManager->GetNinputs() ; 
    if (nStreams) {
      Int_t index = 0 ;  
      for (index = 0 ; index < nStreams ; index++)  
        cout << "Adding SDigits " << GetName() << " from " <<  fManager->GetInputFolderName(index) << endl ;
      
      cout << endl ;
      cout << "Writing digits to " <<   fManager->GetOutputFolderName() << endl ;   
    } else { 
      cout << endl ;
      cout << "Writing digits to " << GetTitle() << endl ;
    }    
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
  
  TString foldname;
  if (fManager) foldname = fManager->GetOutputFolderName();
  else foldname = GetTitle();
  
  AliRunLoader* rl = AliRunLoader::GetRunLoader(foldname);
  if (rl == 0x0)
   {
     Error("PrintDigits","Can not find Run Loader in folder %s",foldname.Data());
     return;
   }
  AliPHOSLoader* gime = dynamic_cast<AliPHOSLoader*>(rl->GetLoader("PHOSLoader"));
  if (gime == 0x0)
   {
     Error("PrintDigits","Can not get PHOS Loader");
     return;
   }
   
  if (gime->TreeD() == 0x0) gime->LoadDigits("read");
  
  TClonesArray * digits = gime->Digits() ; 

  cout << "AliPHOSDigitiser: event " << gAlice->GetEvNumber() << endl ;
  cout << "       Number of entries in Digits list " << digits->GetEntriesFast() << endl ;
  cout << endl ;
  if(strstr(option,"all")||strstr(option,"EMC")){
    
    //loop over digits
    AliPHOSDigit * digit;
    cout << "EMC digits (with primaries): " << endl ;
    cout << "Digit Id     Amplitude   Index       Nprim  Primaries list " <<  endl;      
    Int_t maxEmc = gime->PHOSGeometry()->GetNModules()*gime->GetPHOSGeometry()->GetNCristalsInModule() ;
    Int_t index ;
    for (index = 0 ; (index < digits->GetEntriesFast()) && 
         (((AliPHOSDigit * )  digits->At(index))->GetId() <= maxEmc) ; index++) {
      digit = (AliPHOSDigit * )  digits->At(index) ;
      if(digit->GetNprimary() == 0) continue;
      cout << setw(6)  <<  digit->GetId() << "   "  <<  setw(10)  <<  digit->GetAmp() <<   "    "  
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
Float_t AliPHOSDigitizer::TimeOfNoise(void)
{  // Calculates the time signal generated by noise
  //to be rewritten, now returns just big number
  return 1. ;

}

//____________________________________________________________________________
void AliPHOSDigitizer::WriteDigits()
{

  // Makes TreeD in the output file. 
  // Check if branch already exists: 
  //   if yes, exit without writing: ROOT TTree does not support overwriting/updating of 
  //      already existing branches. 
  //   else creates branch with Digits, named "PHOS", title "...",
  //      and branch "AliPHOSDigitizer", with the same title to keep all the parameters
  //      and names of files, from which digits are made.

  TString foldname;
  if (fManager) foldname = fManager->GetOutputFolderName();
  else foldname = GetTitle();
  
  AliPHOSLoader* gime = AliPHOSLoader::GetPHOSLoader(foldname);
  if (gime == 0x0)
   {
     Error("Exec","Can not get PHOS Loader from event folder named %s",foldname.Data());
     return;
   }

  TTree * treeD = gime->TreeD();
  if (treeD == 0x0) 
   {
     gime->MakeDigitsContainer();
     treeD = gime->TreeD();
   }
  const TClonesArray * digits = gime->Digits(); 
  
  if ( (treeD == 0x0) || (digits == 0x0)) 
   {
     Error("WriteDigits","Can not find array or tree");
     return;
   }
  // -- create Digits branch
  Int_t bufferSize = 32000 ;    
  TBranch * digitsBranch = treeD->Branch("PHOS",&digits,bufferSize);
  digitsBranch->SetTitle(GetName());
  digitsBranch->Fill();

  gime->WriteDigits("OVERWRITE");
  gime->WriteDigitizer("OVERWRITE");
}
