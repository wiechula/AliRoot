#ifndef ALIITSGETTER_H
#define ALIITSGETTER_H



#include <AliLoader.h>



class AliITSLoader: public AliLoader
 {
   public:
    AliITSLoader();
    AliITSLoader(const Char_t *name,const Char_t *topfoldername);
    AliITSLoader(const Char_t *name,TFolder *topfolder);
    
    virtual ~AliITSLoader();//-----------------

    virtual Int_t  GetEvent();//changes to root directory
    virtual void   CleanFolders();
    const TString& GetRawClustersContainerName() const{ return *fRawClustersContainerName;}
    virtual void   CleanRawClusters() {Clean(*fRawClustersContainerName);}
    Int_t          LoadRawClusters(Option_t* opt="");
    virtual void   MakeTree(Option_t* opt);
    virtual void   SetCompressionLevel(Int_t cl);
    void           SetRawClustersFileName(TString& fname){*fRawClustersFileName = fname;}
    virtual Int_t  SetEvent();
    TTree*         TreeC(); // returns a pointer to the tree of  RawClusters
    void           UnloadRawClusters(){CleanRawClusters(); CloseRawClustersFile();}
    virtual Int_t  WriteRawClusters(Option_t* opt="");


   protected:

    // METHODS
    void           CloseRawClustersFile(){CloseDataFile(fRawClustersFile,fRawClustersDir);}
    virtual void   MakeRawClustersContainer();
    Int_t          OpenRawClustersFile(Option_t *opt);
    Int_t          PostRawClusters();

    // DATA
    TString* fRawClustersContainerName; //name of container (TREE) for raw clusters -  TreeC
    TString* fRawClustersFileName; //name of file with raw clusters
    TFile* fRawClustersFile; //pointer to file with raw clusters
    TDirectory* fRawClustersDir; //! pointer to Dir with current event raw data clusters
    static const TString fgkDefaultRawClustersContainerName;

   public:
     ClassDef(AliITSLoader,1)
 };
 
#endif


