#include <iostream.h>
#include "AliITSLoader.h"
#include <AliRunLoader.h>
#include <TTree.h>
#include <TFile.h>

const TString AliITSLoader::fgkDefaultRawClustersContainerName = "TreeC";

ClassImp(AliITSLoader)

AliITSLoader::AliITSLoader() {
  fRawClustersContainerName = 0x0;
  fRawClustersFileName = 0x0;
  fRawClustersFile = 0x0;
  fRawClustersDir = 0x0;
}
/*****************************************************************************/ 
AliITSLoader::AliITSLoader(const Char_t *name,const Char_t *topfoldername)
  :AliLoader(name,topfoldername) {
  fRawClustersContainerName = new TString(fgkDefaultRawClustersContainerName);
  fRawClustersFileName = new TString(*fDetectorName + ".RawCl.root");
  fRawClustersFile = 0x0;
  fRawClustersDir = 0x0;
  cout<<"AliITSLoader::AliITSLoader: name = "<<name<<"; topfolder = "<<topfoldername<<endl;
}
/*****************************************************************************/ 

AliITSLoader::AliITSLoader(const Char_t *name,TFolder *topfolder)
  :AliLoader(name,topfolder) {
  fRawClustersContainerName = new TString(fgkDefaultRawClustersContainerName);
  fRawClustersFileName = new TString(*fDetectorName + ".RawCl.root");
  fRawClustersFile = 0x0;
  fRawClustersDir = 0x0;
}
/*****************************************************************************/ 
AliITSLoader::~AliITSLoader() {
  // destructor

  if(fRawClustersFile) { 
    delete fRawClustersFile;
    fRawClustersFile = 0;
  }
  if(fRawClustersFileName) delete fRawClustersFileName;
  if(fRawClustersContainerName) delete fRawClustersContainerName;
} 

/*****************************************************************************/ 
void AliITSLoader::CleanFolders() {
  // Cleans all posted objects
  AliLoader::CleanFolders();
  CleanRawClusters();
}

/*****************************************************************************/ 
Int_t AliITSLoader::GetEvent() {
  //changes to proper root directory and tries to load data from files to folders
  // 
  // 
  //returns:
  //     0  - in case of no error
  //     1  - event not found
  //     
   
  Int_t rval = AliLoader::GetEvent();
  if( rval != 0) return rval;

  if(fRawClustersFile)PostRawClusters();
  return 0;
}

/*****************************************************************************/ 
Int_t AliITSLoader::LoadRawClusters(Option_t* opt) {
  //opens the file, jups to proper root directory, posts data to proper folder
  //option is passed to TFile::Open and follows the same rules
  //in case of success returns 0
  if (TreeC())
    {
      TString sopt(opt);
      if(sopt.Contains("force",TString::kIgnoreCase) == 0){
        Warning("LoadRawClusters","Raw Clusters are already loaded. Use FORCE option to force reload. Nothing done");
        return 0;
      } 
      else {
        Warning("LoadRawClusters","FORCE option NOT yet implemented. Nothing done");
        return 0;
        //else force option was used
      }
    }
  Int_t retval = 0;
  if (fRawClustersFile  == 0x0)
    {
      retval = OpenRawClustersFile(opt);   
      if (retval) 
        {
          Error("LoadRawClusters","Error occured while opening RawClusters file");
          return retval;
        }
    }
  
  // Exit if the file is (re)created      
  if (AliLoader::TestFileOption(opt) == kFALSE) return retval;
  
  retval = PostRawClusters();
  if (retval) 
    {
      Error("LoadRawClusters","Error occured while posting RawClusters from file to folder");
      return retval;
    }
  return retval;
 
}
/*****************************************************************************/ 

void AliITSLoader::MakeTree(Option_t *opt){
  // invokes AliLoader::MakeTree + specific ITS tree(s)
  // Valid options: H,S,D,R,T and C (C=raw clusters)
  AliLoader::MakeTree(opt);
  const char *oC = strstr(opt,"C");
  if (oC) MakeRawClustersContainer();
}

/*****************************************************************************/ 
void AliITSLoader::MakeRawClustersContainer() {
  //Make the container for raw clusters

  //this virtual method creates the tree for hits in the file
  cout<<"AliITSLoader::MakeRawClustersContainer: Making raw clusters container\n";
  TTree* treeC = new TTree(*fRawClustersContainerName,"RawClusters Container"); //make a tree
  if (treeC == 0x0) {
    Error("MakeRawClustersContainer","Can not create RawClustersContainer for %s",GetName());
    return;
  }
  treeC->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(treeC);
  WriteRawClusters("OVERWRITE");//write tree to the file
}

/*****************************************************************************/

Int_t AliITSLoader::OpenRawClustersFile(Option_t* opt) {
  //Opens raw clus. file and jumps to directory cooresponding to current event.
  //If dir does not exists try to create it
  //opt is passed to TFile::Open

  return OpenDataFile(*fRawClustersFileName,fRawClustersFile,fRawClustersDir,opt);
}
/*****************************************************************************/ 
Int_t AliITSLoader::PostRawClusters() {
  // Posts the RawClusterst container to proper folder
  if (fRawClustersDir == 0x0) {
    Error("PostRawClusters","RawClusters Directory is NULL. LoadRawClusters before.");
    return 2;
  }
  fRawClustersDir->cd();
  
  TObject* tree = fRawClustersDir->Get(GetRawClustersContainerName());
  if(tree) {
    //if such an obejct already exists - remove it first
    TObject* obj = GetDetectorDataFolder()->FindObject(GetRawClustersContainerName());
    if (obj){
      Warning("PostRawClusters","Object named %s already exitsts in %s data folder. Removing it",
              GetRawClustersContainerName().Data(), fDetectorName->Data());
      GetDetectorDataFolder()->Remove(obj);
    }
     
    GetDetectorDataFolder()->Add(tree);
    return 0;
  }
  else {
    Warning("PostRawClusters","Can not find %s in file %s",
            GetRawClustersContainerName().Data(),fRawClustersFile->GetName());
  }
  return 0;
  
}
/*****************************************************************************/ 
void AliITSLoader::SetCompressionLevel(Int_t cl){
  // Sets compression level for all the opened files
  AliLoader::SetCompressionLevel(cl);
  if(fRawClustersFile)fRawClustersFile->SetCompressionLevel(cl);
}


/*****************************************************************************/ 
Int_t AliITSLoader::SetEvent() {
  //basically the same that GetEvent but do not post data to folders

  Int_t rval = AliLoader::SetEvent();
  if( rval != 0) return rval;

  AliRunLoader* rg = GetRunLoader();
  Int_t evno = rg->GetEventNumber();

  if(fRawClustersFile) {
    fRawClustersDir = ChangeDir(fRawClustersFile,evno);
    if (fRawClustersDir == 0x0) {
      Error("SetEvent","Can not chage directory to file %s",fRawClustersFile->GetName());
      return 1;
    }
  }
  return 0;
}
/*****************************************************************************/ 
TTree* AliITSLoader::TreeC() {
  //Get a RawCluster  container from the detector data folder
  TTree* tree = dynamic_cast<TTree*>(GetDetectorDataFolder()->FindObject(GetRawClustersContainerName()));
  return tree;
}
/*****************************************************************************/ 
Int_t AliITSLoader::WriteRawClusters(Option_t *opt) {

  //try to get sdigits from folder  
  TTree *treec = TreeC();
  if(treec == 0x0) {//did not get, nothing to write
    Warning("WriteRawClusters","RawClusters object named %s not found in folder %s.\nNothing to write. Returning", fRawClustersContainerName->Data(), GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
    return 0;
  }
  
  //check if file is opened
  if (fRawClustersDir == 0x0) {
      //if not open, try to open
    if (OpenRawClustersFile("UPDATE")) {
      // oops, can not open the file, give an error message and return error code
      Error("WriteRawClusters","Can not open rawclusters file. Raw Clusters ARE NOT WRITTEN");
      return 1;
    }
  }

  if (fRawClustersFile->IsWritable() == kFALSE) {
    Error("WriteRawClusters","File %s is not writable",fRawClustersFile->GetName());
    return 2;
  }
  
  fRawClustersDir->cd(); //set the proper directory active

  //see if RawClusters container already exists in this (root) directory
  TObject* tree = fRawClustersDir->Get(*fRawClustersContainerName);
  if (tree) { //if they exist, see if option OVERWRITE is used
    const char *oOverWrite = strstr(opt,"OVERWRITE");
      if(!oOverWrite) { //if it is not used -  give an error message and return an error code
        Error("WriteRawClusters","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
  }
  
  cout<<GetName()<<"::WriteRawClusters(opt="<<opt<<")  fRawClustersFile = ";
  cout<<fRawClustersFile->GetName()<<" this = "<<GetName();
  cout<<" RawClusters = "<<treec->GetName()<<endl;

  treec->SetDirectory(fRawClustersDir); //forces setting the directory to this directory (we changed dir few lines above)
  

  treec->Write(0,TObject::kOverwrite);
  fRawClustersFile->Write(0,TObject::kOverwrite);


  return 0;
}

