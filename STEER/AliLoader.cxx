//
//Class (base) responsible for management of data:
//    - opening and closing proper files
//    - posting data to folders
//    - writing data from folders to proper files
//
//Author: Alice Offline Group http://alisoft.cern.ch
//Responsible: Piotr.Skowronski@cern.ch
#include <AliLoader.h>

//standard library includes
#include <iostream.h>

//Root includes
#include <TString.h>
#include <TFile.h>
#include <TFolder.h>
#include <TTree.h>
#include <TTree.h>
#include <TTask.h>

//AliRoot includes
#include <AliRun.h>
#include <AliRunLoader.h>
#include <AliRunDigitizer.h>
#include <AliDigitizer.h>


const TString AliLoader::fgkDefaultHitsContainerName("TreeH");
const TString AliLoader::fgkDefaultDigitsContainerName = "TreeD";
const TString AliLoader::fgkDefaultSDigitsContainerName = "TreeS";
const TString AliLoader::fgkDefaultRecPointsContainerName = "TreeR";
const TString AliLoader::fgkDefaultTracksContainerName = "TreeT";
const TString AliLoader::fgkLoaderBaseName("Loader");

ClassImp(AliLoader)
 
/*****************************************************************************/ 

AliLoader::AliLoader()
 {
   fDetectorName = "";
   fHitsFileName = "";
   fSDigitsFileName = "";
   fDigitsFileName = "";
   fRecPointsFileName = "";
   fTracksFileName = "";
   
   fHitsFile = 0x0;
   fSDigitsFile = 0x0;
   fDigitsFile = 0x0;
   fRecPointsFile = 0x0;
   fTracksFile = 0x0;

   fHitsDir = 0x0;
   fSDigitsDir = 0x0;
   fDigitsDir = 0x0;
   fRecPointsDir = 0x0;
   fTracksDir = 0x0;
   
   fEventFolder = 0x0;
   fDataFolder = 0x0;
   fDetectorDataFolder = 0x0;
   
   fModuleFolder = 0x0;
   fPrimariesFolder = 0x0;
   fQAFolder = 0x0;
   fTasksFolder = 0x0;
   
   fHitsContainerName = "";
   fDigitsContainerName = "";
   fSDigitsContainerName = "";
   fRecPointsContainerName = "";
   fTracksContainerName = "";
   
   fModuleFolder = 0x0;
   fPrimariesFolder = 0x0;
   fDataFolder = 0x0;
   fDetectorDataFolder = 0x0;
   fTasksFolder = 0x0;
   fQAFolder = 0x0;
 }
/******************************************************************/

AliLoader::
AliLoader(const Char_t* detname,const Char_t* eventfoldername)
 {
  //ctor
   cout<<"AliLoader::AliLoader(const Char_t* detname,const Char_t* eventfoldername)"<<endl;

     //try to find folder eventfoldername in top alice folder
     //safe because GetTopFolder will abort in case of failure

   fEventFolder = dynamic_cast<TFolder*>(GetTopFolder()->FindObject(eventfoldername));

   if (!fEventFolder)
    {
      Fatal("AliLoader","Can not find event folder %s. Aborting",eventfoldername);
    }

   fModuleFolder = 0x0;
   fPrimariesFolder = 0x0;
   fDataFolder = 0x0;
   fDetectorDataFolder = 0x0;
   fTasksFolder = 0x0;
   fQAFolder = 0x0;

   
   fHitsFile = 0x0;
   fSDigitsFile = 0x0;
   fDigitsFile = 0x0;
   fRecPointsFile = 0x0;
   fTracksFile = 0x0;

   fHitsDir = 0x0;
   fSDigitsDir = 0x0;
   fDigitsDir = 0x0;
   fRecPointsDir = 0x0;
   fTracksDir = 0x0;

   fHitsContainerName = fgkDefaultHitsContainerName;
   fDigitsContainerName = fgkDefaultDigitsContainerName;
   fSDigitsContainerName = fgkDefaultSDigitsContainerName;
   fRecPointsContainerName = fgkDefaultRecPointsContainerName;
   fTracksContainerName = fgkDefaultTracksContainerName;

   fDetectorName = detname;

   fName = fDetectorName+"Loader";

   fHitsFileName =      fDetectorName + ".Hits.root";
   fSDigitsFileName =   fDetectorName + ".SDigits.root";
   fDigitsFileName =    fDetectorName + ".Digits.root";
   fRecPointsFileName = fDetectorName + ".RecPoints.root";
   fTracksFileName =    fDetectorName + ".Tracks.root";

 }
/*****************************************************************************/ 

AliLoader::
AliLoader(const Char_t * detname,TFolder* eventfolder)
{

   fEventFolder = eventfolder;
   fDetectorName = detname;
   fName = *fDetectorName+"Loader";

   fHitsFileName =      fDetectorName + ".Hits.root";
   fSDigitsFileName =   fDetectorName + ".SDigits.root";
   fDigitsFileName =    fDetectorName + ".Digits.root";
   fRecPointsFileName = fDetectorName + ".RecPoints.root";
   fTracksFileName =    fDetectorName + ".Tracks.root";
   
   fHitsFile = 0x0;
   fSDigitsFile = 0x0;
   fDigitsFile = 0x0;
   fRecPointsFile = 0x0;
   fTracksFile = 0x0;

   fHitsDir = 0x0;
   fSDigitsDir = 0x0;
   fDigitsDir = 0x0;
   fRecPointsDir = 0x0;
   fTracksDir = 0x0;

   fHitsContainerName = fgkDefaultHitsContainerName;
   fDigitsContainerName = fgkDefaultDigitsContainerName;
   fSDigitsContainerName = fgkDefaultSDigitsContainerName;
   fRecPointsContainerName = fgkDefaultRecPointsContainerName;
   fTracksContainerName = fgkDefaultTracksContainerName;
  
}
/*****************************************************************************/ 
AliLoader::~AliLoader()
{
  CleanFolders();
  CloseFiles();
}

/*****************************************************************************/ 
Int_t AliLoader::LoadHits(Option_t* opt)
{
//opens the file, jups to proper root directory, posts data to proper folder
//option is passed to TFile::Open and follows the same rules
//If File will be opened in ReadOnly, no hits will be written
//in case of success returns 0
//  cout<<GetName()<<"::LoadHits()\n";
  if (TreeH())
   {
    TString sopt(opt);
    if( sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadHits","Hits are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    //else force option was used
   }

  SetHitsFileOption(opt);
  
  Int_t retval;
  retval = OpenHitsFile(opt);
  if (retval) 
   {
    Error("LoadHits","Error occured while opening hits file");
    return retval;
   }
  
  //if file is recreated there is no sense to search for data to post and get Error message
  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
    MakeHitsContainer();
    return 0;
   }
  
//  Warning("LoadHits","Try to call PostHits");
  retval = PostHits();
  if (retval)
   {
    Error("LoadHits","Error occured while posting hits from file to folder");
    return retval;
   }
  return 0;
}
/*****************************************************************************/ 
Int_t AliLoader::LoadSDigits(Option_t* opt)
{
//opens the file, jups to proper root directory, posts data to proper folder
//option is passed to TFile::Open and follows the same rules
//in case of success returns 0
  if (TreeS())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadSDigits","Summable Digits are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    //else force option was used
   }
  SetSDigitsFileOption(opt);
  
  Int_t retval;
  if (fSDigitsFile  == 0x0)
   {
    retval = OpenSDigitsFile(opt);
    if (retval) 
     {
      Error("LoadSDigits","Error occured while opening SDigits file");
      return retval;
     }
    }
  
  //if file was   
  if (AliLoader::TestFileOption(opt) == kFALSE) 
   {
    MakeSDigitsContainer();
    return 0;
   }
  
  retval = PostSDigits();
  if (retval) 
   {
    Error("LoadSDigits","Error occured while posting SDigits from file to folder");
    return retval;
   }
  return 0;
 
}
/*****************************************************************************/ 

Int_t AliLoader::LoadDigits(Option_t* opt)
{
//opens the file, jups to proper root directory, posts data to proper folder
//option is passed to TFile::Open and follows the same rules
//in case of success returns 0
  if (TreeD())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadDigits","Digits are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    //else force option was used
   }

  SetDigitsFileOption(opt);
  
  Int_t retval;
  retval = OpenDigitsFile(opt);
  if (retval) 
   {
    Error("LoadDigits","Error occured while opening Digits file");
    return retval;
   }
  
  //test if there is sense to look for data in file
  //if it is recreates
  cout<<"LoadDigits("<<opt<<") ";
  if (AliLoader::TestFileOption(opt) == kFALSE)
   { cout<<"Make digits container\n";
     MakeDigitsContainer();
     return 0;
   }
  cout<<"Posting \n"; 
  retval = PostDigits();
  if (retval) 
   {
    Error("LoadDigits","Error occured while posting Digits from file to folder");
    return retval;
   }
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::LoadRecPoints(Option_t* opt)
{
//opens the file, jups to proper root directory, posts data to proper folder
//option is passed to TFile::Open and follows the same rules
//in case of success returns 0
  if (TreeR())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadRecPoints","Recontructed Points are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    //else force option was used
   }

  SetRecPointsFileOption(opt);

  Int_t retval;
  retval = OpenRecPointsFile(opt);
  if (retval) 
   {
    Error("LoadRecPoints","Error occured while opening RecPoints file");
    return retval;
   }

  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
     MakeRecPointsContainer();
     return 0;
   }
  retval = PostRecPoints();
  if (retval) 
   {
    Error("LoadRecPoints","Error occured while posting RecPoints from file to folder");
    return retval;
   }
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::LoadTracks(Option_t* opt)
{
//opens the file, jups to proper root directory, posts data to proper folder
//option is passed to TFile::Open and follows the same rules
//retrives track container from file and puts it in folder
//in case of success returns 0

  if (TreeT())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadTracks","Tracks are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    //else force option was used
   }

  SetTracksFileOption(opt);

  Int_t retval;
  retval = OpenTracksFile(opt); //
  if (retval) 
   {
    Error("LoadTracks","Error occured while opening Tracks file");
    return retval;
   }

  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
    MakeTracksContainer();
    return 0;
   }
  
  retval = PostTracks();
  if (retval) 
   {
    Error("LoadTracks","Error occured while posting Tracks from file to folder");
    return retval;
   }
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::LoadSDigitizer(Option_t* opt)
{
  if (SDigitizer())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadSDigitizer","Digits are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    else
     { //else force option was used
       CleanSDigitizer();
     }
   }
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     Error("LoadSDigitizer","Can not get RunSDigitizer from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (fSDigitsFile  == 0x0)
   {
      retval = OpenSDigitsFile(opt);
      if (retval) 
       {
         Error("LoadSDigitizer","Error occured while opening Summable Digits file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostSDigitizer();      
  if (retval) 
   {
    Error("LoadSDigits","Error occured while posting SDigits from file to folder");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadDigitizer(Option_t* opt)
{
  if (Digitizer())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadDigitizer","Digits are already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    else
     { //else force option was used
       CleanDigitizer();
     }
   }
  TTask* rsd = AliRunLoader::GetRunDigitizer();
  if (rsd == 0x0)
   {
     Error("LoadDigitizer","Can not get RunDigitizer from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (fDigitsFile  == 0x0)
   {
      retval = OpenDigitsFile(opt);
      if (retval) 
       {
         Error("LoadDigitizer","Error occured while opening Digits file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostDigitizer();      
  if (retval) 
   {
    Error("LoadDigitizer","Error occured while posting Digitizer from file to task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadReconstructioner(Option_t* opt)
{
   if (Reconstructioner())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadReconstructioner","Reconstructioner is already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    else
     { //else force option was used
       CleanReconstructioner();
     }
   }
  TTask* rsd = AliRunLoader::GetRunReconstructioner();
  if (rsd == 0x0)
   {
     Error("LoadReconstructioner","Can not get Run Reconstructioner from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (fRecPointsFile  == 0x0)
   {
      retval = OpenRecPointsFile(opt);
      if (retval) 
       {
         Error("LoadReconstructioner","Error occured while opening Reconstructed Points file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostReconstructioner();
  if (retval) 
   {
    Error("LoadReconstructioner","Error occured while posting Reconstructioner from file to Task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadTracker(Option_t* opt)
{
   if (Tracker())
   {
    TString sopt(opt);
    if(sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadTracker","Tracker is already loaded. Use FORCE option to force reload. Nothing done");
       return 0;
     }
    else
     { //else force option was used
       CleanTracker();
     }
   }
  TTask* rsd = AliRunLoader::GetRunReconstructioner();
  if (rsd == 0x0)
   {
     Error("LoadReconstructioner","Can not get Run Reconstructioner from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (fRecPointsFile  == 0x0)
   {
      retval = OpenRecPointsFile(opt);
      if (retval) 
       {
         Error("LoadReconstructioner","Error occured while opening Reconstructed Points file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostReconstructioner();
  if (retval) 
   {
    Error("LoadReconstructioner","Error occured while posting Reconstructioner from file to Task");
    return retval;
   }
  return 0;
}

/*****************************************************************************/ 


Int_t AliLoader::SetEvent()
{
 //basically the same that GetEvent but do not post data to folders
   CleanFolders();
   
   AliRunLoader* rg = GetRunLoader();
   if (rg == 0x0)
    {
      Error("SetEvent","Can not get RunGettr");
      return 1;
    }
   Int_t evno = rg->GetEventNumber();
   
   if(fHitsFile)
    {
      if (CheckReload(fHitsFile,fHitsFileName))
       {
         UnloadHits();
         cout<<"AliLoader::SetEvent: Reloading new file for hits. file opt is "<<fHitsFileOption<<endl;
         OpenHitsFile(fHitsFileOption);
       }
      fHitsDir = ChangeDir(fHitsFile,evno);
      if (fHitsDir == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fHitsFile->GetName());
          return 1;
        }
    }

   if(fSDigitsFile)
    {
      if (CheckReload(fSDigitsFile,fSDigitsFileName))
       {
         UnloadSDigits();
         cout<<"AliLoader::SetEvent: Reloading new file for sdigits. file opt is "<<fSDigitsFileOption<<endl;
         OpenSDigitsFile(fSDigitsFileOption);
       }

      fSDigitsDir = ChangeDir(fSDigitsFile,evno);
      if (fSDigitsDir == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fSDigitsFile->GetName());
          return 1;
        }
    }
   
   if(fDigitsFile)
    {
      if (CheckReload(fDigitsFile,fDigitsFileName))
       {
         UnloadDigits();
         OpenDigitsFile(fDigitsFileOption);
       }
    
      fDigitsDir = ChangeDir(fDigitsFile,evno);
      if (fDigitsDir == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fDigitsFile->GetName());
          return 1;
        }
    }
   
   if(fRecPointsFile)
    {
      if (CheckReload(fRecPointsFile,fRecPointsFileName))
       {
         UnloadRecPoints();
         OpenRecPointsFile(fRecPointsFileOption);
       }
    
      fRecPointsDir = ChangeDir(fRecPointsFile,evno);
      if (fRecPointsDir == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fRecPointsFile->GetName());
          return 1;
        }
    }

   if(fTracksFile)
    {
      if (CheckReload(fTracksFile,fTracksFileName))
       {
         UnloadTracks();
         OpenTracksFile(fTracksFileOption);
       }

      fTracksDir = ChangeDir(fTracksFile,evno);
      if (fTracksDir == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fTracksFile->GetName());
          return 1;
        }
    }
  return 0;
 
}
/******************************************************************/

Int_t AliLoader::GetEvent()
 {
 //changes to proper root directory and tries to load data from files to folders
 // 
 // 
 //returns:
 //     0  - in case of no error
 //     1  - event not found
 //     
   
   
   if (SetEvent())
    {
      Error("GetEvent","Set Event returned error");
      return 1;
    }
   
   if(fHitsFile) PostHits();
   if(fSDigitsFile) PostSDigits();
   if(fDigitsFile) PostDigits();
   if(fRecPointsFile) PostRecPoints();
   if(fTracksFile) PostTracks();
 
   return 0;
 
 }
/******************************************************************/

Int_t AliLoader::WriteHits(Option_t* opt)
{
  //writes hits from folder to file
  //
  //if option OVERWRITE overwrite existing data
  //if object already exists in the file data are not written 
  //in case OVERWRITE option is not specified
  
  //try to get hits from folder  
  TTree *hits = dynamic_cast<TTree*>(GetDetectorDataFolder()->FindObject(fHitsContainerName));
  if(hits == 0x0)
   {//did not get, nothing to write
     Warning("WriteHits","Hits object named %s not found in folder %s.\nNothing to write. Returning",
              fHitsContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (fHitsDir == 0x0)
   { 
     //if not open, try to open
     SetHitsFileOption("UPDATE");
     if (OpenHitsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteHits","Can not open hits file. HITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (fHitsFile->IsWritable() == kFALSE)
   {
     Error("WriteHits","File %s is not writable",fHitsFile->GetName());
     return 2;
   }
  
  fHitsDir->cd(); //set the proper directory active

  //see if hits container already exists in this (root) directory
  TObject* tree = fHitsDir->Get(fHitsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteHits","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteHits(opt="<<opt<<")  fHitsFile = "<<fHitsFile->GetName()
                 <<" this = "<<GetName()<<" hits = "<<hits->GetName()<<endl;

  hits->SetDirectory(fHitsDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  hits->Write(0,TObject::kOverwrite);
  cout<<"Writing Hits File"<<endl;
  fHitsFile->Write(0,TObject::kOverwrite);
  
  return 0;
}
/******************************************************************/
Int_t AliLoader::WriteSDigits(Option_t* opt)
 {

  //try to get sdigits from folder  
  TTree *sdigits = dynamic_cast<TTree*>(GetDetectorDataFolder()->FindObject(fSDigitsContainerName));
  if(sdigits == 0x0)
   {//did not get, nothing to write
     Warning("WriteSDigits","SDigits object named %s not found in folder %s.\nNothing to write. Returning",
              fSDigitsContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (fSDigitsDir == 0x0)
   { 
     //if not open, try to open
     SetSDigitsFileOption("UPDATE");
     if (OpenSDigitsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteSDigits","Can not open sdigits file. HITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (fSDigitsFile->IsWritable() == kFALSE)
   {
     Error("WriteSDigits","File %s is not writable",fSDigitsFile->GetName());
     return 2;
   }
  
  fSDigitsDir->cd(); //set the proper directory active

  //see if sdigits container already exists in this (root) directory
  TObject* tree = fSDigitsDir->Get(fSDigitsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteSDigits","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteSDigits(opt="<<opt<<")  fSDigitsFile = "<<fSDigitsFile->GetName()
                 <<" this = "<<GetName()<<" sdigits = "<<sdigits->GetName()<<endl;

  sdigits->SetDirectory(fSDigitsDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  sdigits->Write(0,TObject::kOverwrite);
  cout<<"Writing SDigits File"<<endl;
  fSDigitsFile->Write(0,TObject::kOverwrite);


   return 0;
 }
/*****************************************************************************/ 
Int_t AliLoader::WriteDigits(Option_t* opt)
 {
  //try to get digits from folder  
  TObject * obj = GetDetectorDataFolder()->FindObject(fDigitsContainerName);
  if(obj == 0x0)
   {//did not get, nothing to write
     Warning("WriteDigits","Digits object named %s not found in folder %s.\nNothing to write. Returning",
              fDigitsContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }

  TTree *digits = dynamic_cast<TTree*>(obj);
  if(digits == 0x0)
   {//did not get, nothing to write
     Warning("WriteDigits","Digits object named %s not found in folder %s.\nNothing to write. Returning",
              fDigitsContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (fDigitsDir == 0x0)
   { 
     //if not open, try to open
     SetDigitsFileOption("UPDATE");
     if (OpenDigitsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteDigits","Can not open digits file. DIGITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (fDigitsFile->IsWritable() == kFALSE)
   {
     Error("WriteDigits","File %s is not writable",fDigitsFile->GetName());
     return 2;
   }
  
  fDigitsDir->cd(); //set the proper directory active

  //see if digits container already exists in this (root) directory
  TObject* tree = fDigitsDir->Get(fDigitsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteDigits","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteDigits(opt="<<opt<<")  fDigitsFile = "<<fDigitsFile->GetName()
                 <<" this = "<<GetName()<<" digits = "<<digits->GetName()<<endl;

  digits->SetDirectory(fDigitsDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  digits->Write(0,TObject::kOverwrite);
  cout<<"Writing Digits File"<<endl;
  fDigitsFile->Write(0,TObject::kOverwrite);
  cout<<"Done"<<endl;
  return 0;
 }
/*****************************************************************************/ 
Int_t AliLoader::WriteRecPoints(Option_t* opt)
 {
  TTree *recpoints = dynamic_cast<TTree*>(GetDetectorDataFolder()->FindObject(fRecPointsContainerName));
  if(recpoints == 0x0)
   {//did not get, nothing to write
     Warning("WriteRecPoints","RecPoints object named %s not found in folder %s.\nNothing to write. Returning",
              fRecPointsContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (fRecPointsDir == 0x0)
   { 
     //if not open, try to open
     SetRecPointsFileOption("UPDATE");
     if (OpenRecPointsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteRecPoints","Can not open rec points file. HITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (fRecPointsFile->IsWritable() == kFALSE)
   {
     Error("WriteRecPoints","File %s is not writable",fRecPointsFile->GetName());
     return 2;
   }
  
  fRecPointsDir->cd(); //set the proper directory active

  //see if rec points container already exists in this (root) directory
  TObject* tree = fRecPointsDir->Get(fRecPointsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteRecPoints","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteRecPoints(opt="<<opt<<")  fRecPointsFile = "<<fRecPointsFile->GetName()
                 <<" this = "<<GetName()<<" rec points = "<<recpoints->GetName()<<endl;

  recpoints->SetDirectory(fRecPointsDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  recpoints->Write(0,TObject::kOverwrite);
  cout<<"Writing RecPoints File"<<endl;
  fRecPointsFile->Write(0,TObject::kOverwrite);

   
   return 0;
 }
/*****************************************************************************/ 
Int_t AliLoader::WriteTracks(Option_t* opt)
 {
  TObject * obj = GetDetectorDataFolder()->FindObject(fTracksContainerName);
  if(obj == 0x0)
   {//did not get, nothing to write
     Warning("WriteTracks","Tracks object named %s not found in folder %s.\nNothing to write. Returning",
              fTracksContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  TTree *tracks = dynamic_cast<TTree*>(obj);
  if(tracks == 0x0)
   {//did not get, nothing to write
     Warning("WriteTracks","Tracks object named %s not found in folder %s.\nNothing to write. Returning",
              fTracksContainerName.Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (fTracksDir == 0x0)
   { 
     //if not open, try to open
     SetTracksFileOption("UPDATE");
     if (OpenTracksFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteTracks","Can not open tracks file. HITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (fTracksFile->IsWritable() == kFALSE)
   {
     Error("WriteTracks","File %s is not writable",fTracksFile->GetName());
     return 2;
   }
  
  fTracksDir->cd(); //set the proper directory active

  //see if tracks container already exists in this (root) directory
  TObject* tree = fTracksDir->Get(fTracksContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteTracks","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteTracks(opt="<<opt<<")  fTracksFile = "<<fTracksFile->GetName()
                 <<" this = "<<GetName()<<" tracks = "<<tracks->GetName()<<endl;

  tracks->SetDirectory(fTracksDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  tracks->Write(0,TObject::kOverwrite);
  cout<<"Writing Tracks File"<<endl;
  fTracksFile->Write(0,TObject::kOverwrite);

   
   return 0;
 }
/*****************************************************************************/ 
Int_t  AliLoader::WriteSDigitizer(Option_t* opt)
{
//Writes SDigitizer to detector Summbale Digits file/current event ROOT directory
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkSDigitizerTaskName/fDetName/'fDetName+SDigitizer'
//  TTask *sdgzer = SDigitizer();
  
  Error("WriteSDigitizer","Do NOT FORGET TO IMPLEMENT, SKOWRON");
  return 0;
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteDigitizer(Option_t* opt)
{
//Writes Digitizer to detector "Digits file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkDigitizerTaskName/fDetName/'fDetName+Digitizer'
  Error("WriteDigitizer","Do NOT FORGET TO IMPLEMENT, SKOWRON");
  return 0;
}
/*****************************************************************************/ 
Int_t  AliLoader::WriteReconstructioner(Option_t* opt)
{
//Writes Tracker to detector "RecPoint file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkTrackerTaskName/fDetName/'fDetName+Tracker'
  Error("WriteTracker","Do NOT FORGET TO IMPLEMENT, SKOWRON");
  return 0;
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteTracker(Option_t* opt)
{
//Writes Tracker to detector "Tracks file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkTrackerTaskName/fDetName/'fDetName+Tracker'
  Error("WriteTracker","Do NOT FORGET TO IMPLEMENT, SKOWRON");
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::OpenHitsFile(Option_t* opt)
{
//Opens hits file and jumps to directory cooresponding to current event.
//If dir does not exists try to create it
//opt is passed to TFile::Open
  return OpenDataFile(SetFileOffset(fHitsFileName),fHitsFile,fHitsDir,opt);
}
/*****************************************************************************/

Int_t AliLoader::OpenSDigitsFile(Option_t* opt)
{
//Opens  summable digits file and jumps to directory cooresponding to current event.
//If dir does not exists try to create it
//opt is passed to TFile::Open
  return OpenDataFile(SetFileOffset(fSDigitsFileName),fSDigitsFile,fSDigitsDir,opt);
}
/*****************************************************************************/ 

Int_t AliLoader::OpenDigitsFile(Option_t* opt)
{
//Opens digits file and jumps to directory cooresponding to current event.
//If dir does not exists try to create it
//opt is passed to TFile::Open
  return OpenDataFile(SetFileOffset(fDigitsFileName),fDigitsFile,fDigitsDir,opt);

}
/*****************************************************************************/ 

Int_t AliLoader::OpenRecPointsFile(Option_t* opt)
{
//Opens recontructed points file and jumps to directory cooresponding to current event.
//If dir does not exists try to create it
//opt is passed to TFile::Open
  return OpenDataFile(SetFileOffset(fRecPointsFileName),fRecPointsFile,fRecPointsDir,opt);
}
/*****************************************************************************/ 

Int_t AliLoader::OpenTracksFile(Option_t* opt)
{
//Opens tracks file and jumps to directory cooresponding to current event.
//If dir does not exists try to create it
//opt is passed to TFile::Open
  return OpenDataFile(SetFileOffset(fTracksFileName),fTracksFile,fTracksDir,opt);
}
/*****************************************************************************/ 

Int_t AliLoader::OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt)
{
//Opens file named 'filename', and assigns pointer to it to 'file'
//jumps to directory corresponding to current event and stores the pointer to it in 'dir'
//option 'opt' is passed to TFile::Open
  if (file)
   {
     if(file->IsOpen() == kTRUE)
       {
         Warning("OpenDataFile>"," File %s already opened. First close it.",file->GetName());
         return 0;
       }
     else
       {
         Warning("OpenDataFile","Pointer to file %s is not null, but file is not opened",
                file->GetName());
         delete file;
         file = 0x0;
       }
   }

  file = (TFile *)(gROOT->GetListOfFiles()->FindObject(filename));
  if (file)
   {
     if(file->IsOpen() == kTRUE)
       {
         Warning("OpenDataFile","File %s already opened by sombody else. First close it.",
                 file->GetName());
         return 0;
       }
   }
  
  file = TFile::Open(filename,opt);//open the file
  if (file == 0x0)
   {//file is null
     Error("OpenDataFile","Can not open file %s",filename.Data());
     return 1;
   }
  if (file->IsOpen() == kFALSE)
   {//file is null
     Error("OpenDataFile","Can not open file %s",filename.Data());
     return 1;
   }


  AliRunLoader* rg = GetRunLoader();
  if (rg == 0x0)
   {
     Error("OpenDataFile","Can not find Run-Loader in folder");
     return 2;
   }
  Int_t evno = rg->GetEventNumber(); 
  
  dir = ChangeDir(file,evno);
  if (dir == 0x0)
   {
     Error("OpenDataFile","Can not chage directory in file %s.",file->GetName());
     return 3; 
   }

  return 0;
}
/******************************************************************/

void  AliLoader::CloseDataFile(TFile*& file,TDirectory*& dir)
{
//deletes file and sets pointers to 0x0
  if (file)
   {
    delete file;
    file = 0x0;
   }
  dir = 0x0;
}
/******************************************************************/

TFolder* AliLoader::GetTopFolder()
{
//returns TOP aliroot folder, just a simple interface to AliConfig (gives shorter notation)
 return AliConfig::Instance()->GetTopFolder();
}

/******************************************************************/

TFolder* AliLoader::GetEventFolder()
{
//get EVENT folder (data that are changing from event to event, even in single run)
  return fEventFolder;
}
/******************************************************************/
TFolder* AliLoader::GetDataFolder()
{
//returns the folder speciofic to given detector e.g. /Folders/Event/Data/TPC/
// cout<<"AliLoader::GetDataFolder()"<<endl;
 if (!fDataFolder)
  {
   fDataFolder =  dynamic_cast<TFolder*>(GetEventFolder()->FindObject(AliConfig::fgkDataFolderName));
   
   if (!fDataFolder)
    {
     Fatal("GetDataFolder","Can not find AliRoot data folder. Aborting");
     return 0x0;
    }
  }
  return fDataFolder;
}

/*****************************************************************************/ 

TFolder* AliLoader::GetTasksFolder()
{
//Returns pointer to Folder with Alice Tasks
 if (!fTasksFolder)
  {
   fTasksFolder =  dynamic_cast<TFolder*>(GetTopFolder()->FindObject(AliConfig::fgkTasksFolderName));
   
   if (!fTasksFolder)
    {
     Fatal("GetTasksFolder","Can not find tasks folder. Aborting");
     return 0x0;
    }
  }
  return fTasksFolder;
   
}
/*****************************************************************************/ 

TFolder* AliLoader::GetModulesFolder()
{
 if (!fModuleFolder)
  {
   fModuleFolder =  dynamic_cast<TFolder*>(GetEventFolder()->FindObjectAny(AliConfig::GetModulesFolderName()));
   
   if (!fModuleFolder)
    {
     Fatal("GetModulesFolder","Can not find modules folder. Aborting");
     return 0x0;
    }
  }
  return fModuleFolder;
   
}
/*****************************************************************************/ 

TFolder* AliLoader::GetQAFolder()
{ 
  //returns folder with Quality assurance 
  if (fQAFolder == 0x0)
   {
     fQAFolder = dynamic_cast<TFolder*>(GetEventFolder()->FindObjectAny(AliConfig::Instance()->GetQAFolderName()));

     if (fQAFolder == 0x0)
      {
       Fatal("GetQAFolder","Can not find Quality Assurance folder. Aborting");
       return 0x0;
      }
   }
  return fQAFolder;
  
}
/*****************************************************************************/ 
TTask* AliLoader::SDigitizer()
{
//returns SDigitizer task for this detector

  TTask* rsd = AliRunLoader::GetRunSDigitizer();
   if (rsd == 0x0)
    {
      return 0x0;
    }
  TString name(*fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
  return dynamic_cast<TTask*>(rsd->FindObject(name));
}
/*****************************************************************************/ 

AliDigitizer* AliLoader::Digitizer()
{
//returns Digitizer task for this detector
  AliRunDigitizer* rd = AliRunLoader::GetRunDigitizer();
   if (rd == 0x0)
    {
      return 0x0;
    }
  TString name(*fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
  return dynamic_cast<AliDigitizer*>(rd->FindObject(name));
}
/*****************************************************************************/ 

TTask* AliLoader::Reconstructioner()
{
//returns Recontructioner (Cluster Finder, Cluster Maker, 
//or whatever you want to call it) task for this detector
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
   if (rrec == 0x0)
    {
      return 0x0;
    }
  TString name(*fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());

  return dynamic_cast<TTask*>(rrec->FindObject(name));

}
/*****************************************************************************/ 

TTask* AliLoader::Tracker()
{
  return Reconstructioner();
}

/*****************************************************************************/ 
/*****************************************************************************/ 

TDirectory* AliLoader::ChangeDir(TFile* file, Int_t eventno)
{
//changes the root directory in "file" to "dirname" which corresponds to event 'eventno'
//in case of success returns the pointer to directory
//else NULL
 
 if (!file)
  {
    cerr<<"Error <AliLoader::ChangeDir>: File is null"<<endl;
    return 0x0;
  }
 if (!file->IsOpen())
  {
    cerr<<"Error <AliLoader::ChangeDir>: File is not opened"<<endl;
    return 0x0;
  }

 TString dirname("Event");
 dirname+=eventno;
 cout<<" AliLoader::ChangeDir Changing Dir to "<<dirname<<" in file "<<file->GetName()<<endl;

 Bool_t result;
 
 TDirectory* dir = dynamic_cast<TDirectory*>(file->Get(dirname));

 if (dir == 0x0)
  {
    cerr<<"Warning <AliLoader::ChangeDir>: "<<
          "Can not find directory "<<dirname<<
          " in file "<<file->GetName()<<", creating..."<<endl;

            
    TDirectory* newdir = file->mkdir(dirname);
    if (newdir == 0x0)
     {
       cerr<<"Error <AliLoader::ChangeDir>: Can not create new directory in file "
            <<file->GetName()<<". Probably file is read only"<<endl;
       return 0x0;
     }
    result = file->cd(dirname);
    if (result == kFALSE)
     {
       return 0x0;
     }
  }
 else
  {
   file->cd();//make a file active 
   file->cd(dirname);//cd event dir
  }

 return gDirectory;
}
/*****************************************************************************/ 

TString AliLoader::GetUnixDir()
 {
 //This Method will manage jumping through unix directories in case of 
 //run with more events per run than defined in gAlice
 
   TString dir;
   
   return dir;
 }
/*****************************************************************************/ 
/************************************************************/

void AliLoader::MakeTree(Option_t *option)
 {

  const char *oH = strstr(option,"H");

  const char *oD = strstr(option,"D");
  const char *oS = strstr(option,"S");
  const char *oR = strstr(option,"R");
  const char *oT = strstr(option,"T");
  
  if (oH) MakeHitsContainer();
  
  if (oD) MakeDigitsContainer();
  if (oS) MakeSDigitsContainer();
  if (oR) MakeRecPointsContainer();
  if (oT) MakeTracksContainer();
  
    
 }
/*****************************************************************************/ 

void AliLoader::MakeHitsContainer()
{
//this virtual method creates the tree for hits in the file
  if (TreeH()) return;//tree already made 
  cout<<"AliLoader::MakeHitsContainer: Making hits cotainer for "<<GetName()<<endl;
  TTree* treeH = new TTree(fHitsContainerName,"Hits Container"); //make a tree
  if (treeH == 0x0)
   {
     Error("MakeHitsContainer","Can not create HitsContainer for %s",GetName());
     return;
   }
  treeH->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeH);
  WriteHits("OVERWRITE");//write tree to the file
  
}

/*****************************************************************************/ 

void AliLoader::MakeSDigitsContainer()
 {
//Make standard container for Summable Dogits
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader
  if (TreeS()) return;//tree alreadyt made 
  cout<<"AliLoader::MakeSDigitsContainer: Making SDigits cotainer for "<<GetName()<<endl;
  TTree* treeS = new TTree(fSDigitsContainerName,"Summable Digits Container"); //make a tree

  if (treeS == 0x0)
   {
     Error("MakeSDigitsContainer","Can not create Summable Digits Container for %s",GetName());
     return;
   }
  treeS->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeS);
  WriteSDigits("OVERWRITE");//write tree to the file (force file creation)

 }

/*****************************************************************************/ 

void AliLoader::MakeDigitsContainer()
 {
//Make standard container for digits
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader

//this virtual method creates the tree for hits in the file
  if (TreeD()) return;//tree already made 
  cout<<"AliLoader::MakeDigitsContainer: Making digits cotainer for "<<GetName()<<endl;
  TTree* treeD = new TTree(fDigitsContainerName,"Digits Container"); //make a tree
  if (treeD == 0x0)
   {
     Error("MakeDigitsContainer","Can not create DigitsContainer for %s",GetName());
     return;
   }
  treeD->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeD);
  WriteDigits("OVERWRITE");//write tree to the file
 }
/*****************************************************************************/ 

void AliLoader::MakeRecPointsContainer()
 {
//Make standard container for RecPoints (Clusters)
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader
//Make standard container for digits
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader

//this virtual method creates the tree for hits in the file
  if (TreeR()) return;//tree alreadyt made 
  cout<<"AliLoader::MakeRecPointsContainer: Making rec points cotainer for "<<GetName()<<endl;
  TTree* treeR = new TTree(fRecPointsContainerName,"RecPoints Container"); //make a tree
  if (treeR == 0x0)
   {
     Error("MakeRecPointsContainer","Can not create RecPointsContainer for %s",GetName());
     return;
   }
  treeR->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeR);
  WriteRecPoints("OVERWRITE");//write tree to the file
 }
/*****************************************************************************/ 

void AliLoader::MakeTracksContainer()
 {
//Make standard container for tracks
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader
  if (TreeT()) return;//tree alreadyt made 
  cout<<"AliLoader::MakeTracksContainer: Making tracks cotainer for "<<GetName()<<endl;
  TTree* treeD = new TTree(fTracksContainerName,"Tracks Container"); //make a tree
  if (treeD == 0x0)
   {
     Error("MakeTracksContainer","Can not create TracksContainer for %s",GetName());
     return;
   }
  treeD->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeD);
  WriteTracks("OVERWRITE");//write tree to the file
 }

/*****************************************************************************/ 
Int_t AliLoader::PostSDigits()
{
//Posts the SDigits container to proper folder
//  Warning("PostSDigits","Called");

  if (fSDigitsDir == 0x0)
   {
     Error("PostSDigits","SDigits Directory is NULL. LoadSDigits before.");
     return 2; 
   }
  fSDigitsDir->cd();
  
  TObject* tree = fSDigitsDir->Get(GetSDigitsContainerName());
  if(tree)
   {
     //if such an obejct already exists - remove it first
     TObject* obj = GetDetectorDataFolder()->FindObject(GetSDigitsContainerName());
     if (obj)
      {
        Warning("PostSDigits","Object named %s already exitsts in %s data folder. Removing it",
                 GetSDigitsContainerName().Data(), fDetectorName.Data());
        GetDetectorDataFolder()->Remove(obj);
      }
     
     GetDetectorDataFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostSDigits","Can not find %s in file %s",
            GetSDigitsContainerName().Data(),fSDigitsFile->GetName());
   }
  return 0;
  
}
/*****************************************************************************/ 

Int_t AliLoader::PostHits()
{
//takes the hits container from file and puts it in the folder
  
//  Warning("PostHits","Called");
  
  if (fHitsDir == 0x0)
   {
     Error("PostHits","Hits Directory is NULL. LoadHits before.");
     return 2; 
   }

  TObject* tree = fHitsDir->Get(fHitsContainerName);
  if(tree)
   {
     //if such an obejct already exists - remove it first
     CleanHits();//clean folder - probobly redundant - but for security
     GetDetectorDataFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostHits","Can not find Hits Contaioner object <%s> in file %s",
             fHitsContainerName.Data(),fHitsFile->GetName());
    return 1;
   }
}

/******************************************************************/

Int_t AliLoader::PostDigits()
{
  if (fDigitsDir == 0x0)
   {
     Error("PostDigits","File with tracks in not opened");
     return 2;
   }

  TObject* obj = fDigitsDir->Get(GetDigitsContainerName());
  if(obj)
   {
     GetDetectorDataFolder()->Add(obj);
     return 0;
   }
  else
   {
    Warning("PostDigits","Can not find Digits Container in file %s",fDigitsFile->GetName());
    return 1;
   }
}
/*****************************************************************************/ 

Int_t AliLoader::PostRecPoints()
{
  if (fRecPointsDir == 0x0)
   {
     Error("PostRecPoints","File with tracks in not opened");
     return 2;
   }

  TObject* tree = fRecPointsDir->Get(GetRecPointsContainerName());
  if(tree)
   {
     GetDetectorDataFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostRecPoints","Can not find TreeR in file %s",fRecPointsFile->GetName());
    return 1;
   }
}
/*****************************************************************************/ 
Int_t AliLoader::PostTracks()
{
//retrives tracks container from file and posts it to proper folder
  cout<<"####PostTracks####\n";
  if (fTracksDir == 0x0)
   {
     Error("PostTracks","File with tracks in not opened");
     return 2;
   }
  
  TObject* tree = fTracksDir->Get(GetTracksContainerName()); //get the container
  if(tree) //if got 
   {
     GetDetectorDataFolder()->Add(tree); //put in the folder
     return 0;
   }
  else
   {
    Warning("PostTracks","Can not find Tree in file %s",fTracksFile->GetName());
    return 1;
   }
}

/*****************************************************************************/ 

Int_t AliLoader::PostSDigitizer()
 {
 
   if (fSDigitsDir == 0x0)
    {
     Error("PostSDigitizer","fSDigitsDir is Null. Probobly SDigits not loaded");
     return 1;
    }
   
   TString name(*fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
   TTask* sdigitizer = dynamic_cast<TTask*>(fSDigitsDir->Get(name));
   
   if (sdigitizer == 0x0)
    {
     Error("PostSDigitizer","Can not find SDigitizer named in file %s for event %d",
           name.Data(),fSDigitsFile->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
  
   return PostSDigitizer(sdigitizer);

 }
/*****************************************************************************/ 
Int_t AliLoader::PostSDigitizer(TTask* sdzer)
{
//adds it to Run SDigitizer
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     Error("PostSDigitizer","Can not get RunSDigitizer from folder. Can not post");
     return 1;
   }
  rsd->Add(sdzer);
  return 0;
}

/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer()
 {
   if (fDigitsDir == 0x0)
    {
     Error("PostDigitizer","fDigitsDir is Null. Probobly Digits not loaded");
     return 1;
    }
   
   TString name(*fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
   AliDigitizer* digitizer = dynamic_cast<AliDigitizer*>(fDigitsDir->Get(name));
   
   if (digitizer == 0x0)
    {
     Error("PostDigitizer","Can not find Digitizer named in file %s for event %d",
           name.Data(),fDigitsFile->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
   return PostDigitizer(digitizer);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer(AliDigitizer* task)
 {
 //adds it to Run Digitizer
  TTask* rsd = AliRunLoader::GetRunDigitizer();
  if (rsd == 0x0)
   {
     Error("PostDigitizer","Can not get RunDigitizer from folder. Can not post");
     return 1;
   }
  rsd->Add(task);
  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::PostReconstructioner()
 {
   if (fRecPointsDir == 0x0)
    {
     Error("PostReconstructioner","fRecPointsDir is Null. Probobly RecPoints not loaded");
     return 1;
    }
   
   TString name(*fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());
   TTask* task = dynamic_cast<TTask*>(fRecPointsDir->Get(name));
   
   if (task == 0x0)
    {
     Error("PostReconstructioner","Can not find Reconstructioner named in file %s for event %d",
           name.Data(),fRecPointsFile->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
  
   return PostReconstructioner(task);

 }
/*****************************************************************************/ 

Int_t AliLoader::PostReconstructioner(TTask* task)
 {
   //adds it to Run Reconstructioner
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
   {
     Error("PostReconstructioner","Can not get RunReconstructioner from folder. Can not post");
     return 1;
   }
  rrec->Add(task);
  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker()
 {
   if (fTracksDir == 0x0)
    {
     Error("PostTracker","fTracksDir is Null. Probobly Tracks or Tracker not loaded");
     return 1;
    }
   
   TString name(*fDetectorName + AliConfig::Instance()->GetTrackerTaskName());
   TTask* task = dynamic_cast<TTask*>(fTracksDir->Get(name));
   
   if (task == 0x0)
    {
     Error("PostTracker","Can not find Tracker named in file %s for event %d",
           name.Data(),fTracksFile->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }

   return PostTracker(task);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker(TTask* task)
 {
  //adds it to Run Tracker
  TTask* runtracker = AliRunLoader::GetRunTracker();
  if (runtracker == 0x0)
   {
     Error("PostTracker","Can not get RunTracker from folder. Can not post");
     return 1;
   }
  runtracker->Add(task);
  return 0;

 }
/*****************************************************************************/ 

TObject** AliLoader::GetDetectorDataRef(TObject *obj)
{
 if (obj == 0x0)
  {
    return 0x0;
  }
 return GetDetectorDataFolder()->GetListOfFolders()->GetObjectRef(obj) ;
}
/*****************************************************************************/ 

TObject** AliLoader::SDigitizerRef()
{
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     return 0x0;
   }
  return rsd->GetListOfTasks()->GetObjectRef(SDigitizer());
}
/*****************************************************************************/ 

TObject** AliLoader::DigitizerRef()
{
 AliRunDigitizer* rd = AliRunLoader::GetRunDigitizer();
 if (rd == 0x0)
  {
    return 0x0;
  }
 return rd->GetListOfTasks()->GetObjectRef(Digitizer()) ;
}
/*****************************************************************************/ 

TObject** AliLoader::ReconstructionerRef()
{
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
   {
     return 0x0;
   }
  return rrec->GetListOfTasks()->GetObjectRef(Reconstructioner());
}
/*****************************************************************************/ 

TObject** AliLoader::TrackerRef()
{
   TTask* rrec = AliRunLoader::GetRunTracker();
   if (rrec == 0x0)
    {
      return 0x0;
    }
   return rrec->GetListOfTasks()->GetObjectRef(Tracker());
}

/*****************************************************************************/ 
void AliLoader::CleanFolders()
 {
 //Cleans all posted objects == removes from folders and deletes them
 
//  cout<<"**********************************************"<<endl;
//  cout<<"AliLoader::CleanFolders:"<<endl;
//  cout<<"Before"<<endl;
//  GetDetectorDataFolder()->ls();
  
  CleanHits();
  CleanSDigits();
  CleanDigits();
  CleanRecPoints();
  CleanTracks();
  
//  cout<<endl<<endl<<"After"<<endl;
//  GetDetectorDataFolder()->ls();
 }
/*****************************************************************************/ 

void AliLoader::Clean(const TString& name)
{
  TObject* obj = GetDetectorDataFolder()->FindObject(name);
  if(obj)
   {
     GetDetectorDataFolder()->Remove(obj);
     delete obj;
   }
}
/*****************************************************************************/ 

void AliLoader::CleanSDigitizer()
{
//removes and deletes detector task from Run Task
  TTask* task = AliRunLoader::GetRunSDigitizer();
  if (task == 0x0)
   {
     Error("CleanSDigitizer","Can not get RunSDigitizer from folder. Can not clean");
     return;
   }

  delete task->GetListOfTasks()->Remove(SDigitizer()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanDigitizer()
{
//removes and deletes detector task from Run Task
  TTask* task = AliRunLoader::GetRunDigitizer();
  if (task == 0x0)
   {
     Error("CleanDigitizer","Can not get RunDigitizer from folder. Can not clean");
     return;
   }

  delete task->GetListOfTasks()->Remove(Digitizer()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanReconstructioner()
{
//removes and deletes detector Reconstructioner from Run Reconstructioner
  TTask* task = AliRunLoader::GetRunReconstructioner();
  if (task == 0x0)
   {
     Error("CleanReconstructioner","Can not get RunReconstructioner from folder. Can not clean");
     return;
   }

  delete task->GetListOfTasks()->Remove(Reconstructioner()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanTracker()
{
//removes and deletes detector tracker from Run Tracker
  TTask* task = AliRunLoader::GetRunTracker();
  if (task == 0x0)
   {
     Error("CleanTracker","Can not get RunTracker from folder. Can not clean");
     return;
   }

  delete task->GetListOfTasks()->Remove(Tracker()); //TTList::Remove does not delete object

}
/*****************************************************************************/ 

/*****************************************************************************/ 
TTree* AliLoader::TreeH()
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(GetHitsContainerName());
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}
/*****************************************************************************/ 
TTree* AliLoader::TreeS()
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(GetSDigitsContainerName());
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}
/*****************************************************************************/ 
TTree* AliLoader::TreeD()
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(GetDigitsContainerName());
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}
/*****************************************************************************/ 
TTree* AliLoader::TreeR()
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(GetRecPointsContainerName());
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}
/*****************************************************************************/ 

TTree* AliLoader::TreeT()
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(GetTracksContainerName());
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}
/*****************************************************************************/ 

void AliLoader::SetCompressionLevel(Int_t cl)
{
//Sets compression level in all opened files
 if (fHitsFile) fHitsFile->SetCompressionLevel(cl);
 if (fSDigitsFile) fSDigitsFile->SetCompressionLevel(cl);
 if (fDigitsFile) fDigitsFile->SetCompressionLevel(cl);
 if (fRecPointsFile) fRecPointsFile->SetCompressionLevel(cl);
 if (fTracksFile) fTracksFile->SetCompressionLevel(cl);
}
/*****************************************************************************/ 

void AliLoader::CloseFiles()
{
 if (fHitsFile) 
  {
   delete fHitsFile;
  }
 if (fSDigitsFile)
  {
   delete fSDigitsFile;
  }
 if (fDigitsFile)
  {
   delete fDigitsFile;
  }
 if (fRecPointsFile) 
  {
   delete fRecPointsFile;
  }
 if (fTracksFile) 
  {
   delete fTracksFile;
  }
 
}
/*****************************************************************************/ 

Int_t  AliLoader::SetEventFolder(TFolder* eventfolder)
{
 if (eventfolder == 0x0)
  {
    Error("SetEventFolder","Stupid joke. Argument is NULL");
    return 1;
  }
 fEventFolder = eventfolder;
 return 0;
}//sets the event folder
/*****************************************************************************/ 

Int_t AliLoader::Register()
{
//triggers creation of subfolders for a given detector

 if (fEventFolder == 0x0)
  {
    Error("Register","Event folder is not set");
    return 1;
  }
 Int_t retval = AliConfig::Instance()->AddDetector(fEventFolder,fDetectorName,fDetectorName);
 if(retval)
  {
    Error("SetEventFolder","Can not create tasks and/or folders for %s. Event folder name is %s",
          fDetectorName.Data(),fEventFolder->GetName());
    return retval;
  }
 return 0;
}
/*****************************************************************************/ 
AliRunLoader* AliLoader::GetRunLoader()
{
//gets the run-loader from event folder
  AliRunLoader* rg = 0x0;
  TObject * obj = GetEventFolder()->FindObject(AliRunLoader::fgkRunLoaderName);
  if (obj) rg = dynamic_cast<AliRunLoader*>(obj);
  return rg;
}
/*****************************************************************************/ 
Bool_t  AliLoader::TestFileOption(Option_t* opt)
{
//tests the TFile::Option
//if file is truncated at opening moment ("recreate", "new" or "create") returns kFALSE;
//else kTRUE (means opened with "read" or "update" mode)
  TString option(opt);
  if (option.Contains("recreate",TString::kIgnoreCase)) return kFALSE;
  if (option.Contains("new",TString::kIgnoreCase)) return kFALSE;
  if (option.Contains("create",TString::kIgnoreCase)) return kFALSE;
  return kTRUE;
}
/*****************************************************************************/ 
void  AliLoader::SetDirName(TString& dirname)
{
  fHitsFileName = dirname + "/" + fHitsFileName;
  fSDigitsFileName = dirname + "/" + fSDigitsFileName;
  fDigitsFileName = dirname + "/" + fDigitsFileName;
  fRecPointsFileName = dirname + "/" + fRecPointsFileName;
  fTracksFileName = dirname + "/" + fTracksFileName;
}

/*****************************************************************************/ 
const TString AliLoader::SetFileOffset(const TString& fname)
{

//return fname;
  Long_t offset = (Long_t)GetRunLoader()->GetFileOffset();
  if (offset < 1) return fname;

  TString soffset;
  soffset += offset;//automatic conversion to string
  TString dotroot(".root");
  const TString& offfsetdotroot = offset + dotroot;
  TString out = fname;
  out = out.ReplaceAll(dotroot,offfsetdotroot);
  cout<<"AliLoader::SetFileOffset: in="<<fname<<" out="<<out<<endl;
  return out;

}
/*****************************************************************************/ 

void AliLoader::SetDigitsFileNameSuffix(const TString& suffix)
{
  //adds the suffix before ".root", 
  //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
  //made on Jiri Chudoba demand
  cout<<"AliLoader::SetDigitsFileNameSuffix(\""<<suffix<<"\")\n";
  cout<<"Digits File Name before: "<<fDigitsFileName<<endl;
  TString dotroot(".root");
  const TString& suffixdotroot = suffix + dotroot;
  fDigitsFileName = fDigitsFileName.ReplaceAll(dotroot,suffixdotroot);
  cout<<"                 after: "<<fDigitsFileName<<endl;
}
/*****************************************************************************/ 

Bool_t AliLoader::CheckReload(const TFile* file, const TString& basefilename)
{
//checks if we have to reload given file
 if (file == 0x0) return kFALSE;
 TString tmp = SetFileOffset(basefilename);
 if (tmp.CompareTo(file->GetName())) return kTRUE;  //file must be reloaded
 return  kFALSE;
}
/*****************************************************************************/ 

void AliLoader::SetHitsFileOption(Option_t* newopt)
{
//Hits File Option in open
  if (fHitsFileOption.CompareTo(newopt) == 0) return;
  
  fHitsFileOption = newopt;
  if (fHitsFile)
   {
     UnloadHits();
     LoadHits(fHitsFileOption);
   }
}
/*****************************************************************************/ 

void AliLoader::SetSDigitsFileOption(Option_t* newopt)
{
//SDigits File Option in open
  if (fSDigitsFileOption.CompareTo(newopt) == 0) return;
  
  fSDigitsFileOption = newopt;
  if (fSDigitsFile)
   {
     UnloadSDigits();
     LoadSDigits(fSDigitsFileOption);
   }
}
/*****************************************************************************/ 

void AliLoader::SetDigitsFileOption(Option_t* newopt)
{
//Digits File Option in open
  if (fDigitsFileOption.CompareTo(newopt) == 0) return;
  
  fDigitsFileOption = newopt;
  if (fDigitsFile)
   {
     UnloadDigits();
     LoadDigits(fDigitsFileOption);
   }

}
/*****************************************************************************/ 

void AliLoader::SetRecPointsFileOption(Option_t* newopt)
{
//RecPoints File Option in open
  if (fRecPointsFileOption.CompareTo(newopt) == 0) return;
  
  fRecPointsFileOption = newopt;
  if (fRecPointsFile)
   {
     UnloadRecPoints();
     LoadRecPoints(fRecPointsFileOption);
   }
}
/*****************************************************************************/ 

void AliLoader::SetTracksFileOption(Option_t* newopt)
{
//Tracks File Option in open
  if (fTracksFileOption.CompareTo(newopt) == 0) return;
  
  fTracksFileOption = newopt;
  if (fTracksFile)
   {
     UnloadTracks();
     LoadTracks(fTracksFileOption);
   }
}
/*****************************************************************************/ 

