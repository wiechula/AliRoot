#include "AliRunLoader.h"

//
//This class aims to be the only one interface for manging data
//It stores Loaders for all modules which knows the filenames 
//of the data files to be stored.
//It aims to substitude AliRun in automatic maging of data positioning
//thus there won't be necessity of loading gAlice from file in order to 
//get fast ccess to the data
//
//logical place for putting the Loader specific to the given detector is detector itself
// but, to load detector one need to load gAlice, and by the way all other detectors
// with their geometrieces and so on. 
// So, if one need to open TPC clusters there is no principal nedd to read everything
//
// When RunLoader is read from the file it does not connect to the folder structure
// it must be connected (mounted) manualy in the macro or class code. 
// Default event folder is defined by AliConfig::fgkDefaultEventFolderName
// but can be mounted elsewhere. Usefull specially in merging, when more than 
//

/**************************************************************************/

#include <TString.h>
#include <TFolder.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGeometry.h>
#include <TTask.h>

#include "AliRun.h"
#include "AliConfig.h"
#include "AliLoader.h"
#include "AliHeader.h"
#include "AliStack.h"
#include "TObjArray.h"
#include "AliDetector.h"
#include "AliRunDigitizer.h"

TDirectory * gKineDir = 0x0;

ClassImp(AliRunLoader)

const TString AliRunLoader::fgkRunLoaderName("RunLoader");

const TString AliRunLoader::fgkHeaderBranchName("Header");
const TString AliRunLoader::fgkHeaderContainerName("TE");
const TString AliRunLoader::fgkKineContainerName("TreeK");
const TString AliRunLoader::fgkTrackRefsContainerName("TreeTR");
const TString AliRunLoader::fgkKineBranchName("Particles");
const TString AliRunLoader::fgkDefaultKineFileName("Kinematics.root");
const TString AliRunLoader::fgkDefaultTrackRefsFileName("TrackRefs.root");
const TString AliRunLoader::fgkGAliceName("gAlice");
/**************************************************************************/

AliRunLoader::AliRunLoader()
{
  fEventFolder = 0x0;
  fCurrentEvent = 0;
  fNEventsPerFile = 1;
  fStack = 0x0;
  fHeader = 0x0;
  fLoaders = 0x0;
  fKineFileName = "";
  fTrackRefsFileName = "";
  fGAFile = 0x0;
  fKineFile = 0x0;
  fKineDir = 0x0;
  fTrackRefsFile = 0x0;
  fTrackRefsDir = 0x0;
  AliConfig::Instance();//force to build the folder structure
}
/**************************************************************************/

AliRunLoader::AliRunLoader(const char* eventfoldername):
  TNamed(fgkRunLoaderName,fgkRunLoaderName),
  fKineFileName(fgkDefaultKineFileName),
  fTrackRefsFileName(fgkDefaultTrackRefsFileName)
{
//ctor
  fEventFolder = 0x0;
  fCurrentEvent = 0;
  fNEventsPerFile = 1;
  fStack = 0x0;
  fHeader = 0x0;
  fGAFile = 0x0;
  fKineFile = 0x0;
  fKineDir = 0x0;
  fLoaders = new TObjArray();
  SetEventFolderName(eventfoldername);
}
/**************************************************************************/

AliRunLoader::~AliRunLoader()
{
  
  if(fLoaders)
   {
    fLoaders->SetOwner();
    delete fLoaders;
   }

  UnloadHeader();
  UnloadKinematics();
  UnloadTrackRefs();
  UnloadgAlice();
  
  RemoveEventFolder();
  //fEventFolder is deleted by the way of removing - TopAliceFolder owns it

  delete fGAFile; 
  delete fKineFile;
  delete fTrackRefsFile;
}
/**************************************************************************/

AliRunLoader::AliRunLoader(TFolder* topfolder):TNamed(fgkRunLoaderName,fgkRunLoaderName)
{
 if(topfolder == 0x0)
  {
    Fatal("AliRunLoader(TFolder*)","Parameter is NULL");
    return;
  }
 fEventFolder = topfolder;
 
 TObject* obj = fEventFolder->FindObject(fgkRunLoaderName);
 if (obj)
  { //if it is, then sth. is going wrong... exits aliroot session
    Fatal("AliRunLoader(const char*)",
          "In Event Folder Named %s object named %s already exists. I am confused ...",
           fEventFolder->GetName(),fgkRunLoaderName.Data());
    return;//never reached
  }
   
 fLoaders = new TObjArray();
 fEventFolder->Add(this);//put myself to the folder to accessible for all
  
}
/**************************************************************************/

Int_t AliRunLoader::GetEvent(Int_t evno)
{
//Gets event number evno
//Reloads all data properly
  if (fCurrentEvent == evno) return 0;
  fCurrentEvent = evno;

  Int_t retval;

  if (GetHeader())
   {
     retval = TreeE()->GetEvent(fCurrentEvent);
     if ( retval == 0)
      {
        Error("GetEvent","Cannot find event: %d\n ",fCurrentEvent);
        return 5;
      }
   }
  if (TreeE()) fStack = GetHeader()->Stack();
  
  if (fStack) 
   { 
     fStack->SetEventFolderName(fEventFolder->GetName());
   }
  else
   {
     Warning("GetEvent","Did not found stack in header");
   }
  
  retval = SetEvent();
  if (retval)
   {
     Error("GetEvent","Error occured while setting event %d",evno);
     return 1;
   }

  if (fTrackRefsFile) 
   {
    retval = PostTrackRefs();
   }
   
  if (fKineFile) 
   {
    retval = PostKinematics();
    fStack->GetEvent();
   }
  
  if (retval)
   {
     Error("GetEvent","Error occured while posting kinematics. Event %d",evno);
     return 2;
   }

  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
     retval = loader->GetEvent();
     if (retval)
      {
       Error("GetEvent","Error occured while getting event for %s. Event %d.",
              loader->GetDetectorName().Data(), evno);
       return 3;
      }
   }
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::SetEventNumber(Int_t evno)
{
  //cleans folders and sets the root dirs in files 
  if (fCurrentEvent == evno) return 0;
  fCurrentEvent = evno;
  return SetEvent();
}
/**************************************************************************/
Int_t AliRunLoader::SetEvent()
{
 //if kinematocs was loaded Cleans folder data
 //change 

  if (TreeK()) CleanKinematics();
  if (fKineFile)
   {  
     fKineDir = AliLoader::ChangeDir(fKineFile,fCurrentEvent);
     if (fKineDir == 0x0)
      {
        Error("SetEvent","Can not change to root directory in file %s",fKineFileName.Data());
        return 1;
      }
   }
  
  if (TreeTR()) CleanTrackRefs();
  if (fTrackRefsFile)
   {  
     fTrackRefsDir = AliLoader::ChangeDir(fTrackRefsFile,fCurrentEvent);
     if (fTrackRefsDir == 0x0)
      {
        Error("SetEvent","Can not change to root directory in file %s",fTrackRefsFileName.Data());
        return 2;
      }
   }

  
  TIter next(fLoaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
     Loader->SetEvent();
   }

  return 0;
}

/**************************************************************************/
AliRunLoader* AliRunLoader::Open
  (const char* filename, const char* eventfoldername, Option_t* option)
{
//Opens a desired file 'filename'
//gets the the run-Loader and mounts it desired folder
//returns the pointer to run Loader which can be further used for accessing data 
//in case of error returns NULL
 
 cout<<"\n\n\nNew I/O strcture: See more info:"<<endl;
 cout<<"http://alisoft.cern.ch/people/skowron/codedoc/split/index.html"<<endl;
 cout<<"\n\n\n"<<endl;
 
 AliRunLoader* result = 0x0;
 TFile * gAliceFile = 0x0;
 
 gAliceFile = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
 if (gAliceFile==0)
    gAliceFile = TFile::Open(filename,option);//open a file
 else
  {
    cerr<<"Warning <AliRunLoader::Open>: File "<<filename<<"already open"<<endl;
    TString opt(gAliceFile->GetOption());
    if (opt.CompareTo(option,TString::kIgnoreCase))
     {
      cerr<<"Warning <AliRunLoader::Open>: File "<<filename
           <<" already has different open option("
           <<gAliceFile->GetOption()<<") than desired ("<<option<<"). REOPENING!"<<endl;
      delete gAliceFile;
      gAliceFile = TFile::Open(filename,option);//open a file
     }
  }

 if (!gAliceFile) 
  {//null pointer returned
    cerr<<"ERROR <AliRunLoader::Open>"
        <<"Can not open file "<<filename<<endl;
    return 0x0;
  }
  
 if (gAliceFile->IsOpen() == kFALSE)
  {//pointer to valid object returned but file is not opened
    cerr<<"ERROR <AliRunLoader::Open>"
        <<"Can not open file "<<filename<<endl;
    return 0x0;
  }
 
 //if file is "read" or "update" than we try to find AliRunLoader there - if not found cry and exit
 //else create new AliRunLoader
 if ( AliLoader::TestFileOption(option) )
  { 
    cout<<"Reading RL from file"<<endl;
    
    result = dynamic_cast<AliRunLoader*>(gAliceFile->Get(fgkRunLoaderName));//get the run Loader from the file
    if (result == 0x0)
     {//didn't get
       cerr<<"ERROR <AliRunLoader::Open>"
           <<"Can not find run-Loader in file "<<filename<<endl;
       delete gAliceFile;//close the file
       return 0x0;
     }
    Int_t tmp = result->SetEventFolderName(eventfoldername);//mount a event folder   
    if (tmp)//if SetEvent  returned error
     {
       cerr<<"ERROR <AliRunLoader::Open>"
           <<"Can not mount event in folder "
           <<eventfoldername<<endl;
       delete result; //delete run-Loader
       delete gAliceFile;//close the file
       return 0x0;
     }
  }
 else
  {
    cout<<"Creating new AliRunLoader. Folder name is "<<eventfoldername<<endl;
    result = new AliRunLoader(eventfoldername);
  }
 
//procedure for extracting dir name from the file name 
 TString fname(filename);
 Int_t  nsl = fname.Last('/');//look for slash in file name
 TString dirname;
 if (nsl < 0) 
  {//slash not found
    Int_t  nsl = fname.Last(':');//look for colon e.g. rfio:galice.root
    if (nsl < 0) dirname = ".";//not found
    else dirname = fname.Remove(nsl);//found
  }
 else dirname = fname.Remove(nsl);//slash found
 
 cout<<"Dir name is : "<<dirname<<endl;
 result->SetDirName(dirname); 
 result->SetGAliceFile(gAliceFile);//set the pointer to gAliceFile
 return result;
}
/**************************************************************************/
Int_t AliRunLoader::GetNumberOfEvents()
{
 //returns number of events in Run
 Int_t retval;
 if( TreeE() == 0x0 )
  {
    retval = LoadHeader();
    if (retval) 
     {
       Error("GetNumberOfEvents","Error occured while loading header");
       return -1;
     }
  }
 return (Int_t)TreeE()->GetEntries();
}

/**************************************************************************/
void AliRunLoader::MakeHeader()
{
 //Makes header and connects it to header tree (if it exists)
  cout<<"Make Header\n";
  if(fHeader == 0x0)
   {
     cout<<"Creating new Header Object\n";
     fHeader= new AliHeader();
   }
  TTree* tree = TreeE();
  if (tree)
   {
     cout<<"Got Tree from folder\n";
     TBranch* branch = tree->GetBranch(fgkHeaderBranchName);
     if (branch == 0x0)
      {
        cout<<"Creating new branch \n";
        branch = tree->Branch(fgkHeaderBranchName, "AliHeader", &fHeader, 4000, 0);
        branch->SetAutoDelete(kFALSE);
      }
     else
      {
        cout<<"Got Branch from Tree \n";
        branch->SetAddress(&fHeader);
        tree->GetEvent(fCurrentEvent);
        fStack = fHeader->Stack(); //should be safe - if we created Stack, header returns pointer to the same object
        fStack->SetEventFolderName(fEventFolder->GetName());
        if (TreeK()) fStack->GetEvent();
      }
   } 
  cout<<"Exiting MakeHeader \n";
}
/**************************************************************************/

void AliRunLoader::MakeStack()
{
//Creates the stack object -  do not connect the tree
  if(fStack == 0x0)
   { 
     fStack = new AliStack(10000);
     fStack->SetEventFolderName(fEventFolder->GetName());
   }
}

/**************************************************************************/

void AliRunLoader::MakeTree(Option_t *option)
{
//Creates trees
  const char *oK = strstr(option,"K"); //Kine  
  const char *oE = strstr(option,"E"); //Header

  TTree* tree;

  if(oK && !TreeK())
   { 
     if (fKineDir == 0x0)
      {
        Error("MakeTree(\"K\")","LoadKinematics first");
      }
     else
      {
        fKineDir->cd();
        tree = new TTree(fgkKineContainerName,"Kinematics");
        GetEventFolder()->Add(tree);
        MakeStack();
        fStack->ConnectTree();
        WriteKinematics("OVERWRITE");
     }
   }
  
  if(oE && !TreeE())
   {
     fGAFile->cd();
     tree = new TTree(fgkHeaderContainerName,"Tree with Headers");
     GetEventFolder()->Add(tree);
     MakeHeader();
     WriteHeader("OVERWRITE");
   }
  
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
    loader->MakeTree(option);
   }

}
/**************************************************************************/
    
Int_t AliRunLoader::LoadgAlice()
{
// if (LoadHeader())
//  {
//    Error("LoadgAlice","LoadHeader returned error. Exiting");
//    return 1;
//  }

 AliRun* alirun = dynamic_cast<AliRun*>(fGAFile->Get(fgkGAliceName));
 if (alirun == 0x0)
  {
    Error("LoadgAlice"," Can not find gAlice in file %s",fGAFile->GetName());
    return 2;
  }
 alirun->SetRunLoader(this);
 if (gAlice)
  {
    Warning("LoadgAlice","gAlice already exists. Putting retrived object in folder named %s",
             GetEventFolder()->GetName());
  }
 else
  {
    gAlice = alirun;
  }

 GetEventFolder()->Add(alirun);
 return 0; 
}
/**************************************************************************/

Int_t AliRunLoader::LoadHeader()
{
 if (GetEventFolder() == 0x0)
  {
    Error("LoadHaeder","Event folder not specified yet");
    return 1;
  }

 if (fGAFile == 0x0)
  {
    Error("LoadHaeder","Session not opened. Use AliRunLoader::Open");
    return 2;
  }
 
 if (fGAFile->IsOpen() == kFALSE)
  {
    Error("LoadHaeder","Session not opened. Use AliRunLoader::Open");
    return 2;
  }

 TTree* tree = dynamic_cast<TTree*>(fGAFile->Get(fgkHeaderContainerName));
 if (tree == 0x0)
  {
    Fatal("LoadHaeder","Can not find header tree named %s in file %s",
           fgkHeaderContainerName.Data(),fGAFile->GetName());
    return 2;
  }

 CleanHeader();
 GetEventFolder()->Add(tree);
 MakeHeader();//creates header object and connects to tree
 return 0; 

}
/**************************************************************************/

Int_t AliRunLoader::LoadKinematics(Option_t* option)
{
 if (GetEventFolder() == 0x0)
  {
    Error("LoadKinematics","Event folder not specified yet");
    return 1;
  }
 Int_t retval = OpenKineFile(option);
 if (retval)
   {
    Error("LoadKinematics","Error occured while opening Kine file");
    return retval;
   }
 
  
 if (AliLoader::TestFileOption(option) == kFALSE) return 0;
 
 retval = PostKinematics();
 if (retval)
   {//propagate error up 
    Error("LoadKinematics","PostKinematics returned error");
    return retval;
   }
 if (fStack) fStack->GetEvent();
 return 0;
}
/**************************************************************************/
Int_t AliRunLoader::OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt)
{
//Opens File with kinematics
 if (file)
  {
    if (file->IsOpen() == kFALSE)
     {//pointer is not null but file is not opened
       Warning("OpenKineFile","Pointer to file is not null, but file is not opened");
       delete file;
       file = 0x0; //proceed with opening procedure
     }
    else
     { 
       Warning("LoadKinematics","Kinematics file already opened");
       return 0;
     }
  }
//try to find if that file is opened somewere else
 file = (TFile *)( gROOT->GetListOfFiles()->FindObject(filename) );
 if (file)
  {
   if(file->IsOpen() == kTRUE)
    {
     Warning("OpenDataFile","File %s already opened by sombody else.",file->GetName());
     return 0;
    }
  }

 file = TFile::Open(filename,opt);
 if (file == 0x0)
  {//file is null
    Error("LoadKinematics","Can not open file %s",filename.Data());
    return 1;
  }
 if (file->IsOpen() == kFALSE)
  {//file is not opened
   Error("LoadKinematics","Can not open file %s",filename.Data());
   return 1;
  }
  

 dir = AliLoader::ChangeDir(file,fCurrentEvent);
 if (dir == 0x0)
  {
    Error("OpenKineFile","Can not change to root directory in file %s",filename.Data());
    return 3;
  }
 return 0; 
}
/**************************************************************************/

Int_t AliRunLoader::PostKinematics()
{
  //takes the kine container from file and puts it in the folder
  
  cout<<"Posting Kinematics\n";
  
  if (fKineDir == 0x0)
   {
     Error("PostKinematics","Kinematics Directory is NULL. LoadKinematics before.");
     return 2; 
   }

  TObject* tree = fKineDir->Get(fgkKineContainerName);
  if(tree)
   {
     //if such an obejct already exists - remove it first
     CleanKinematics();//if kine tree already is in folder
     GetEventFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostKinematics","Can not find Kinematics Contaioner object <%s> in file %s",
             fgkKineContainerName.Data(),fKineFile->GetName());
    return 1;
   }
}
/**************************************************************************/

Int_t AliRunLoader::PostTrackRefs()
{
  //takes the kine container from file and puts it in the folder
  
  cout<<"Posting TrackRefs\n";
  
  if (fTrackRefsDir == 0x0)
   {
     Error("PostTrackRefs","TrackRefs Directory is NULL. LoadTrackRefs before.");
     return 2; 
   }

  TObject* tree = fTrackRefsDir->Get(fgkTrackRefsContainerName);
  if(tree)
   {
     //if such an obejct already exists - remove it first
     CleanTrackRefs();//if kine tree already is in folder
     GetEventFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostTrackRefs","Can not find TrackRefs Contaioner object <%s> in file %s",
             fgkTrackRefsContainerName.Data(),fTrackRefsFile->GetName());
    return 1;
   }
}
/**************************************************************************/


TTree* AliRunLoader::TreeE() const
{
 //returns the tree from folder; shortcut method
 return dynamic_cast<TTree*>(fEventFolder->FindObject(fgkHeaderContainerName));
}
/**************************************************************************/
AliHeader* AliRunLoader::GetHeader() const
{
 return fHeader;
}
/**************************************************************************/
 
TTree* AliRunLoader::TreeK() const
{
 //returns the tree from folder; shortcut method
 return dynamic_cast<TTree*>(GetEventFolder()->FindObject(fgkKineContainerName));
}

/**************************************************************************/
TTree* AliRunLoader::TreeTR() const
{
 //returns the tree from folder; shortcut method
 return dynamic_cast<TTree*>(GetEventFolder()->FindObject(fgkTrackRefsContainerName));
}

/**************************************************************************/
AliRun* AliRunLoader::GetAliRun() const
{
//returns AliRun which sits in the folder
 if (fEventFolder == 0x0) return 0x0;
 return dynamic_cast<AliRun*>(fEventFolder->FindObject(fgkGAliceName));
}


Int_t AliRunLoader::WriteGeometry(Option_t* opt)
{
  fGAFile->cd();
  TGeometry* geo = gAlice->GetGeometry();
  if (geo == 0x0)
   {
     Error("WriteGeometry","Can not get geometry from gAlice");
     return 1;
   }
  geo->Write();
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteHeader(Option_t* opt)
{
  cout<<"\n\n\nWRITING HEADER\n\n\n";
  
  TTree* tree = TreeE();
  if ( tree == 0x0)
   {
     Warning("WriteHeader","Can not find Header in Folder");
     return 0;
   } 
  if (fGAFile->IsWritable() == kFALSE)
   {
     Error("WriteHeader","File %s is not writable",fGAFile->GetName());
     return 1;
   }

  TObject* obj = fGAFile->Get(fgkHeaderContainerName);
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteHeader","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  tree->SetDirectory(fGAFile);
  tree->Write(0,TObject::kOverwrite);
  fGAFile->Write(0,TObject::kOverwrite);
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteAliRun(Option_t* opt)
{
  fGAFile->cd();
  if (gAlice) gAlice->Write();
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteKinematics(Option_t* opt)
{

  if(TreeK() == 0x0)
   {//did not get, nothing to write
     Warning("WriteKinematics","Kinematics object named %s not found in folder %s.\nNothing to write. Returning",
              fgkKineContainerName.Data(), GetEventFolder()->GetName());
     return 0;
   }
  
  //check if file is opened
  if (fKineDir == 0x0)
   { 
     //if not open, try to open
     if (OpenKineFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteKinematics","Can not open Kine file. Kinematics is NOT WRITTEN");
        return 1;
      }
   }

  if (fKineFile->IsWritable() == kFALSE)
   {
     Error("WriteKinematics","File %s is not writable",fKineFile->GetName());
     return 1;
   }
  
  //see if hits container already exists in this (root) directory
  TObject* tree = fKineDir->Get(fgkKineContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteKinematics","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteKinematics(opt="<<opt<<")  fKineFile = " <<fKineFile->GetName()
      <<" fKineDir = " <<fKineDir->GetName()<<" kine tree = "<<TreeK()->GetName()<<endl;
  
  fKineDir->cd();
  TreeK()->SetDirectory(fKineDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  TreeK()->Write(0,TObject::kOverwrite);
  cout<<"Writing Hits File"<<endl;
  fKineFile->Write(0,TObject::kOverwrite);

  return 0;
   
}
/**************************************************************************/
Int_t AliRunLoader::WriteTrackRefs(Option_t* opt)
{
  if(TreeTR() == 0x0)
   {//did not get, nothing to write
     Warning("WriteTrackRefs","TrackRefs object named %s not found in folder %s.\nNothing to write. Returning",
              fgkTrackRefsContainerName.Data(), GetEventFolder()->GetName());
     return 0;
   }
  
  //check if file is opened
  if (fTrackRefsDir == 0x0)
   { 
     //if not open, try to open
     if (OpenTrackRefsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteTrackRefs","Can not open TrackRefs file. TrackRefs is NOT WRITTEN");
        return 1;
      }
   }

  if (fTrackRefsFile->IsWritable() == kFALSE)
   {
     Error("WriteTrackRefs","File %s is not writable",fTrackRefsFile->GetName());
     return 1;
   }
  
  //see if hits container already exists in this (root) directory
  TObject* tree = fTrackRefsDir->Get(fgkTrackRefsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteTrackRefs","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  cout<<GetName()<<"::WriteTrackRefs(opt="<<opt<<")  fTrackRefsFile = " <<fTrackRefsFile->GetName()
       <<" fTrackRefsDir = " <<fTrackRefsDir->GetName()<<" TreeTR() tree = "<<TreeTR()->GetName()<<endl;
  
  fTrackRefsDir->cd();
  TreeTR()->SetDirectory(fTrackRefsDir); //forces setting the directory to this directory (we changed dir few lines above)
  
  cout<<"Writing tree"<<endl;
  TreeTR()->Write(0,TObject::kOverwrite);
  cout<<"Writing Hits File"<<endl;
  fTrackRefsFile->Write(0,TObject::kOverwrite);

  return 0;
  
}
/**************************************************************************/

Int_t AliRunLoader::WriteHits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteHits(opt);
     if (res)
      {
        Error("WriteHits","Failed to write hits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteSDigits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteSDigits(opt);
     if (res)
      {
        Error("WriteHits","Failed to write summable digits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteDigits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteDigits(opt);
     if (res)
      {
        Error("WriteHits","Failed to write digits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteRecPoints(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteRecPoints(opt);
     if (res)
      {
        Error("WriteHits","Failed to write Reconstructed Points for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteTracks(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteTracks(opt);
     if (res)
      {
        Error("WriteHits","Failed to write Tracks for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::SetEventFolderName(const TString& name)
{  //sets top folder name for this run; of alread
  if (name.IsNull())
   {
     Error("SetTopFolderName","Name is empty");
     return 1;
   }
  
  //check if such a folder already exists - try to find it in alice top folder
  TObject* obj = AliConfig::Instance()->GetTopFolder()->FindObject(name);
  if(obj)
   {
     TFolder* fold = dynamic_cast<TFolder*>(obj);
     if (fold == 0x0)
      {
       Error("SetTopFolderName","Such a obejct already exists in top alice folder and it is not a folder.");
       return 2;
      }
     //folder which was found is our folder
     if (fEventFolder == fold)
      {
       return 0;
      }
     else
      {
       Error("SetTopFolderName","Such a folder already exists in top alice folder. Can not mount.");
       return 2;
      }
   }

  //event is alredy connected, just change name of the folder
  if (fEventFolder) 
   {
     fEventFolder->SetName(name);
     return 0;
   }
   
  //build the event folder structure
  cout<<"AliRunLoader::SetEventFolderName Creting new event folder named "<<name<<endl;
  fEventFolder = AliConfig::Instance()->BuildEventFolder(name,"Event Folder");
  fEventFolder->Add(this);//put myself to the folder to accessible for all
  
  if (Stack()) Stack()->SetEventFolderName(fEventFolder->GetName());
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     loader->SetEventFolder(fEventFolder);
     loader->Register();
   }
  
  fEventFolder->SetOwner();
  return 0;
}
/**************************************************************************/

void AliRunLoader::AddLoader(AliLoader* loader)
 {
 //Adds the Loader for given detector 
  if (loader == 0x0) //if null shout and exit
   {
     Error("AddLoader","Parameter is NULL");
     return;
   }
  if (fEventFolder) loader->SetEventFolder(fEventFolder); //if event folder is already defined, 
                                                          //pass information to the Loader
  fLoaders->Add(loader);//add the Loader to the array
 }
/**************************************************************************/

void AliRunLoader::AddLoader(AliModule* det)
 {
//Asks module (detector) ro make a Loader and stores in the array
   if (det == 0x0) return;
   AliLoader* get = det->MakeLoader(fEventFolder->GetName());
   if(get) 
    {
      AddLoader(get);
      cout<<"AliRunLoader::AddLoader Detector: "<<det->GetName()<<" Loader :"<<get->GetName()<<endl;
    }
 }

/**************************************************************************/

AliLoader* AliRunLoader::GetLoader(const char* detname) const
{
  return (AliLoader*)fLoaders->FindObject(detname);
}

/**************************************************************************/

AliLoader* AliRunLoader::GetLoader(AliDetector* det) const
{
 if(det == 0x0) return 0x0;
 TString getname(det->GetName());
 getname+="Loader";
 cout<<"AliRunLoader::GetLoader(AliDetector* det): loader name is "<<getname<<endl;
 return GetLoader(getname);
}

/**************************************************************************/

void AliRunLoader::CleanFolders()
{
//  fEventFolder->Add(this);//put myself to the folder to accessible for all

  CleanDetectors();
  CleanHeader();
  CleanKinematics();
}
/**************************************************************************/

void AliRunLoader::CleanDetectors()
{
//Calls CleanFolders for all detectors
  TIter next(fLoaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
     cout<<Loader->GetName();
     Loader->CleanFolders();
   }
}
/**************************************************************************/

void AliRunLoader::RemoveEventFolder()
{
//remove all the tree of event 
//all the stuff changing EbE stays untached (PDGDB, tasks, etc.)

 if (fEventFolder == 0x0) return;
 fEventFolder->SetOwner(kFALSE);//don't we want to deleted while removing the folder that we are sitting in
 fEventFolder->Remove(this);//remove us drom folder
 
 AliConfig::Instance()->GetTopFolder()->SetOwner(); //brings ownership back for fEventFolder since it sits in top folder
 AliConfig::Instance()->GetTopFolder()->Remove(fEventFolder); //remove the event tree
 delete fEventFolder;
}
/**************************************************************************/

void AliRunLoader::SetGAliceFile(TFile* gafile)
{
 fGAFile = gafile;
}

/**************************************************************************/

Int_t AliRunLoader::LoadHits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  cout<<"Loading Hits\n";
  TObjArray* loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     cout<<"All"<<endl;
     loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     loaders = &arr;//get the pointer array
   }   

  cout<<"For detectors. All is "<<loaders->GetEntries()<<endl;
  
  TIter next(loaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
    cout<<" "<<loader->GetName();
    loader->LoadHits(opt);
   }
  cout<<endl;

  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadSDigits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     cout<<"All"<<endl;
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadSDigits(opt);
   }
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadDigits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     cout<<"All"<<endl;
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadDigits(opt);
   }
  return 0;
} 


/**************************************************************************/

Int_t AliRunLoader::LoadRecPoints(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     cout<<"All"<<endl;
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadRecPoints(opt);
   }
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadTracks(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     cout<<"All"<<endl;
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadTracks(opt);
   }
  return 0;
} 

/**************************************************************************/

AliRunLoader* AliRunLoader::GetRunLoader(const char* eventfoldername)
 {
  TFolder* evfold= dynamic_cast<TFolder*>(AliConfig::Instance()->GetTopFolder()->FindObject(eventfoldername));
  if (evfold == 0x0)
   {
     return 0x0;
   }
  AliRunLoader* runget = dynamic_cast<AliRunLoader*>(evfold->FindObject(AliRunLoader::fgkRunLoaderName));
  return runget;
  
 }
/**************************************************************************/

void AliRunLoader::CdGAFile()
{
//sets gDirectory to galice file
//work around 
  if(fGAFile) fGAFile->cd();
}
 
/**************************************************************************/

void AliRunLoader::GetListOfDetectors(const char * namelist,TObjArray& pointerarray) const
 {
//this method looks for all Loaders corresponding 
//to names (many) specified in namelist i.e. namelist ("ITS TPC TRD")
  
   char buff[10];
   char dets [200];
   strcpy(dets,namelist);//compiler cries when char* = const Option_t*;
   dets[strlen(dets)+1] = '\n';//set endl at the end of string 
   char* pdet = dets;
   Int_t tmp;
   for(;;)
    {
      tmp = sscanf(pdet,"%s",buff);//read the string from the input string pdet into buff
      if ( (buff[0] == 0) || (tmp == 0) ) break; //if not read
     
      pdet = strstr(pdet,buff) + strlen(buff);;//move the input pointer about number of bytes (letters) read
      //I am aware that is a little complicated. I don't know the number of spaces between detector names
      //so I read the string, than I find where it starts (strstr) and move the pointer about length of a string
      //If there is a better way, please write me (Piotr.Skowronski@cern.ch)
      cout<<"Buff"<<endl;
      //construct the Loader name
      TString getname(buff);
      getname+="Loader";
      AliLoader* loader = GetLoader(getname);//get the Loader
      if (loader)
       {
        pointerarray.Add(loader);
       }
      else
       {
        Error("GetListOfDetectors","Can not find Loader for %s",buff);
       }
        
      buff[0] = 0;
    }
 }
/*****************************************************************************/ 

void AliRunLoader::Clean(const TString& name)
{
//removes object with given name from event folder and deletes it
  if (GetEventFolder() == 0x0) return;
  TObject* obj = GetEventFolder()->FindObject(name);
  if(obj)
   {
     GetEventFolder()->Remove(obj);
     delete obj;
   }
}

/*****************************************************************************/ 

AliRunDigitizer* AliRunLoader::GetRunDigitizer()
{
//returns Run Digitizer from folder

 TFolder* topf = AliConfig::Instance()->GetTopFolder();
 return dynamic_cast<AliRunDigitizer*>(topf->FindObjectAny(AliConfig::Instance()->GetDigitizerTaskName()));
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunSDigitizer()
{
//returns SDigitizer Task from folder

 TFolder* topf = AliConfig::Instance()->GetTopFolder();
 return dynamic_cast<TTask*>(topf->FindObjectAny(AliConfig::Instance()->GetSDigitizerTaskName()));
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunReconstructioner()
{
//returns Reconstructioner Task from folder
 TFolder* topf = AliConfig::Instance()->GetTopFolder();
 return dynamic_cast<TTask*>(topf->FindObjectAny(AliConfig::Instance()->GetReconstructionerTaskName()));
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunTracker()
{
//returns Tracker Task from folder
 TFolder* topf = AliConfig::Instance()->GetTopFolder();
 return dynamic_cast<TTask*>(topf->FindObjectAny(AliConfig::Instance()->GetTrackerTaskName()));
}
/*****************************************************************************/ 

void AliRunLoader::SetCompressionLevel(Int_t cl)
{
 if (fGAFile) fGAFile->SetCompressionLevel(cl);
 if (fKineFile) fKineFile->SetCompressionLevel(cl);
 TIter next(fLoaders);
 AliLoader *loader;
 while((loader = (AliLoader*)next()))
  {
   loader->SetCompressionLevel(cl);
  }
}
/**************************************************************************/

void AliRunLoader::UnloadHeader()
{
 //removes TreeE from folder and deletes it
 // as well as fHeader object
 if (GetEventFolder()) GetEventFolder()->Remove(TreeE());
 delete fHeader;
 fHeader = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadKinematics()
{
 CleanKinematics();//removes from folder and deletes
 delete fKineFile;//closes Kine file
 fKineFile = 0x0;
 fKineDir = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadTrackRefs()
{
 CleanTrackRefs();//removes from folder and deletes
 delete fTrackRefsFile;//closes TrackRefs file
 fTrackRefsFile = 0x0;
 fTrackRefsDir = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadgAlice()
{
 if (gAlice == GetAliRun())
  {
   cout<<"AliRunLoader::UnloadgAlice(): Set gAlice = 0x0";
   gAlice = 0x0;//if gAlice is the same that in folder (to be deleted by the way of folder)
  }
 AliRun* alirun = GetAliRun();
 if (GetEventFolder()) GetEventFolder()->Remove(alirun);
 delete alirun;
}
/**************************************************************************/

void  AliRunLoader::MakeTrackRefsContainer()
{
  if (TreeTR()) return;
  TTree* tree = new TTree(fgkTrackRefsContainerName,"Tree with Track References");
  GetEventFolder()->Add(tree);
  WriteTrackRefs();
}
/**************************************************************************/

Int_t AliRunLoader::LoadTrackRefs(Option_t* option)
{
//Load track references from file (opens file and posts tree to folder)

 if (GetEventFolder() == 0x0)
  {
    Error("LoadTrackRefs","Event folder not specified yet");
    return 1;
  }
 Int_t retval = OpenTrackRefsFile(option);
 if (retval)
   {
    Error("LoadTrackRefs","Error occured while opening Kine file");
    return retval;
   }
 
  
 if (AliLoader::TestFileOption(option) == kFALSE) return 0;

 retval = PostTrackRefs();
 
 if (retval)
  {//propagate error up 
    Error("LoadTrackRefs","PostTrackRefs returned error");
    return retval;
  }
 return 0;
}
/**************************************************************************/

void  AliRunLoader::SetDirName(TString& dirname)
{
//sets directory name 
  if (dirname.IsNull()) return;
  
  fTrackRefsFileName = dirname + "/" + fTrackRefsFileName;
  fKineFileName = dirname + "/" + fKineFileName;

  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
    loader->SetDirName(dirname);
   }

}
Int_t AliRunLoader::OpenKineFile(Option_t* opt)
 {
 //Opens file with kinematicss
   return OpenDataFile(SetFileOffset(fKineFileName),fKineFile,fKineDir,opt);
 }
Int_t AliRunLoader::OpenTrackRefsFile(Option_t* opt)
 {
  //opens file with Track References
   return OpenDataFile(SetFileOffset(fTrackRefsFileName),fTrackRefsFile,fTrackRefsDir,opt);
 }
Int_t AliRunLoader::GetFileOffset() const
{
  return fCurrentEvent%fNEventsPerFile;
}

/*****************************************************************************/ 
const TString AliRunLoader::SetFileOffset(const TString& fname)
{
return fname;

/*  Int_t offset = GetFileOffset();
  if (offset < 1) return fname;
  TString soffset;
  soffset += offset;//automatic conversion to string
  TString dotroot(".root");
  const TString& offfsetdotroot = offset+dotroot;
  TString out = fname.ReplaceAll(dotroot,offfsetdotroot);
  cout<<"AliLoader::SetFileOffset: in="<<fname<<" out="<<out<<endl;
  return out;
*/
}

/*****************************************************************************/ 
