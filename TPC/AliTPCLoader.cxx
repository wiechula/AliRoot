#include "AliTPCLoader.h"
#include <TTree.h>
#include <TFile.h>

const TString AliTPCLoader::fgkDefaultHitsFileName      = "TPC.Hits.root";
const TString AliTPCLoader::fgkDefaultSDigitsFileName   = "TPC.SDigits.root";
const TString AliTPCLoader::fgkDefaultDigitsFileName    = "TPC.Digits.root";
const TString AliTPCLoader::fgkDefaultRecPointsFileName = "TPC.RecPoints.root";
const TString AliTPCLoader::fgkDefaultTracksFileName    = "TPC.Tracks.root";


ClassImp(AliTPCLoader)
AliTPCLoader::AliTPCLoader()
 {
 }
/*****************************************************************************/ 
AliTPCLoader::AliTPCLoader(const Char_t *name,const Char_t *topfoldername)
 :AliLoader(name,topfoldername)
{
  cout<<"AliTPCLoader::AliTPCLoader: name = "<<name<<"; topfolder = "<<topfoldername<<endl;
}
/*****************************************************************************/ 

AliTPCLoader::AliTPCLoader(const Char_t *name,TFolder *topfolder)
 :AliLoader(name,topfolder)
 {
 }

