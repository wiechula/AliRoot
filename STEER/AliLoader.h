        #ifndef ALIGETTER_H
#define ALIGETTER_H


#include <AliConfig.h>
class TString;
class TFile;
class TFolder;
class TTree;
class TTask;
class AliRunLoader;
class AliDigitizer;

class AliLoader: public TNamed
 {
//base class for loaders
//loaders is common for reading data for all detectors
//Each detector has a getter data member
//getter is accessible via folder structure as well

   public:
    AliLoader();
    AliLoader(const Char_t *detname,const Char_t *eventfoldername); //contructor with name of the top folder of the tree
    AliLoader(const Char_t *detname,TFolder* eventfolder);

    virtual ~AliLoader();//----------------- 
 

    virtual Int_t  GetEvent();//changes to root directory
                             //relies on the event number defined in gAlice
    virtual Int_t  SetEvent();//basically the same that GetEvent but do not post data to folders
    virtual void   MakeTree(Option_t* opt);

    //these methods are responsible for reading data
    // 1. Opens the file (accordingly to gAlice->GetEvNumber()/Nevents per File
    // 2. Changes to proper ROOT directory
    // 3. Retrives data and posts to folders
    //options: 
    //   ""       - readonly file
    //   "update" - update file
    //   "recreate" 
    Int_t          LoadHits(Option_t* opt="");
    Int_t          LoadSDigits(Option_t* opt="");
    Int_t          LoadDigits(Option_t* opt="");
    Int_t          LoadRecPoints(Option_t* opt="");
    Int_t          LoadTracks(Option_t* opt="");
    
    Int_t          LoadSDigitizer(Option_t* opt="");
    Int_t          LoadDigitizer(Option_t* opt="");
    Int_t          LoadReconstructioner(Option_t* opt="");
    Int_t          LoadTracker(Option_t* opt="");

    void           UnloadHits(){CleanHits();CloseHitsFile();}
    void           UnloadSDigits(){CleanSDigits();CloseSDigitsFile();}
    void           UnloadDigits(){CleanDigits();CloseDigitsFile();}
    void           UnloadRecPoints(){CleanRecPoints();CloseRecPointsFile();}
    void           UnloadTracks(){CleanTracks();CloseTracksFile();}
    
  //these methods writes object from folder to proper file
    virtual Int_t  WriteHits(Option_t* opt=""); 
    virtual Int_t  WriteSDigits(Option_t* opt="");
    virtual Int_t  WriteDigits(Option_t* opt="");
    virtual Int_t  WriteRecPoints(Option_t* opt="");
    virtual Int_t  WriteTracks(Option_t* opt="");
    virtual void   Write(Option_t* opt="all"){};
    
    
    virtual Int_t  WriteSDigitizer(Option_t* opt="");
    virtual Int_t  WriteDigitizer(Option_t* opt="");
    virtual Int_t  WriteReconstructioner(Option_t* opt="");
    virtual Int_t  WriteTracker(Option_t* opt="");
    
//   virtual Int_t  WritePIDer(Option_t* opt="");
    
  //void SetTopFolder(TString& str){}; //Sets root top folder for the run
    
    void           SetHitsFileName(const TString& fname){fHitsFileName = fname;}
    void           SetSDigitsFileName(const TString& fname){fSDigitsFileName = fname;}
    void           SetDigitsFileName(const TString& fname){fDigitsFileName = fname;}
    void           SetRecPointsFileName(const TString& fname){fRecPointsFileName = fname;}
    void           SetTracksFileName(const TString& fname){fTracksFileName = fname;}

    const TString& GetHitsFileName(){return fHitsFileName;}
    const TString& GetSDigitsFileName(){return fSDigitsFileName;}
    const TString& GetDigitsFileName(){return fDigitsFileName;}
    const TString& GetRecPointsFileName(){return fRecPointsFileName;}
    const TString& GetTracksFileName(){return fTracksFileName;}
  
    Int_t          SetEventFolder(TFolder* eventfolder);//sets the event folder
    Int_t          Register();//triggers creation of subfolders for a given detector
      
    TFolder*       GetTopFolder();  //returns top aliroot folder
    TFolder*       GetEventFolder();//returns the folder that event is sitting
    TFolder*       GetDataFolder(); //returns the folder that hits, sdigits, etc are sitting (this contains folders named like detectors)
    TFolder*       GetDetectorDataFolder();//returns the folder that hits, sdigits, etc 
                                     //are sitting for a given detector (subfolder of Data Folder)
    TFolder*       GetModulesFolder();
    TFolder*       GetTasksFolder();
    TFolder*       GetQAFolder();
    
    TTask*         SDigitizer();
    AliDigitizer*  Digitizer();
    TTask*         Reconstructioner();
    TTask*         Tracker();
    

    TObject**      SDigitizerRef();
    TObject**      DigitizerRef();
    TObject**      ReconstructionerRef();
    TObject**      TrackerRef();
    
    virtual void   MakeHitsContainer();
    virtual void   MakeSDigitsContainer();
    virtual void   MakeDigitsContainer();
    virtual void   MakeRecPointsContainer();
    virtual void   MakeTracksContainer();
        
    const TString& GetHitsContainerName() const{ return fHitsContainerName;}
    const TString& GetSDigitsContainerName() const{ return fSDigitsContainerName;}
    const TString& GetDigitsContainerName() const{ return fDigitsContainerName;}
    const TString& GetRecPointsContainerName() const{ return fRecPointsContainerName;}
    const TString& GetTracksContainerName() const{ return fTracksContainerName;}

    virtual void   CleanFolders();
    virtual void   CloseFiles();
    
    TTree*         TreeH(); //returns the tree from folder; shortcut method
    TTree*         TreeS(); //returns the tree from folder; shortcut method
    TTree*         TreeD(); //returns the tree from folder; shortcut method
    TTree*         TreeR(); //returns the tree from folder; shortcut method
    TTree*         TreeT(); //returns the tree from folder; shortcut method

    virtual Int_t PostSDigitizer(TTask* sdzer);//adds it to Run SDigitizer
    virtual Int_t PostDigitizer(AliDigitizer* task);
    virtual Int_t PostReconstructioner(TTask* task);
    virtual Int_t PostTracker(TTask* task);
    
    virtual void  CleanHits()     {Clean(fHitsContainerName);}//cleans hits from folder
    virtual void  CleanSDigits()  {Clean(fSDigitsContainerName);}
    virtual void  CleanDigits()   {Clean(fDigitsContainerName);}
    virtual void  CleanRecPoints(){Clean(fRecPointsContainerName);}
    virtual void  CleanTracks()   {Clean(fTracksContainerName);}
    
    virtual void  CleanSDigitizer();
    virtual void  CleanDigitizer();
    virtual void  CleanReconstructioner();
    virtual void  CleanTracker();

    void          SetHitsFileOption(Option_t* newopt);//Hits File Option in open
    void          SetSDigitsFileOption(Option_t* newopt);//
    void          SetDigitsFileOption(Option_t* newopt);//
    void          SetRecPointsFileOption(Option_t* newopt);//
    void          SetTracksFileOption(Option_t* newopt);//
    
    virtual void  SetCompressionLevel(Int_t cl);
    void          SetDirName(TString& name);
    
    const TString& GetDetectorName() const{return fDetectorName;}
    AliRunLoader*  GetRunLoader();//gets the run-getter from event folder
    
    void          SetDigitsFileNameSuffix(const TString& suffix);//adds the suffix before ".root", 
                                                          //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
                                                          //made on Jiri Chudoba demand
   protected:

    /*********************************************/
    /************    PROTECTED      **************/
    /*********     M E T H O D S       ***********/
    /*********************************************/

    Int_t         OpenHitsFile(Option_t* opt);   //Opens hits file and jumps to directory cooresponding to current event.
    Int_t         OpenSDigitsFile(Option_t* opt);//If dir does not exists try to create it
    Int_t         OpenDigitsFile(Option_t* opt); //opt is passed to TFile::Open
    Int_t         OpenRecPointsFile(Option_t* opt);
    Int_t         OpenTracksFile(Option_t* opt);
    Int_t         OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt);
    
    void          CloseHitsFile(){CloseDataFile(fHitsFile,fHitsDir);}
    void          CloseSDigitsFile(){CloseDataFile(fSDigitsFile,fSDigitsDir);}
    void          CloseDigitsFile(){CloseDataFile(fDigitsFile,fDigitsDir);} 
    void          CloseRecPointsFile(){CloseDataFile(fRecPointsFile,fRecPointsDir);}
    void          CloseTracksFile(){CloseDataFile(fTracksFile,fTracksDir);}
    void          CloseDataFile(TFile*& file,TDirectory*& dir);
    //reads data from the file and posts them into folder
    virtual Int_t PostHits(); 
    virtual Int_t PostSDigits();
    virtual Int_t PostDigits();
    virtual Int_t PostRecPoints();
    virtual Int_t PostTracks();

    virtual Int_t PostSDigitizer();//gets SDigitizer from file and adds it to Run SDigitizer
    virtual Int_t PostDigitizer();//gets Digitizer from file and adds it to Run Digitizer
    virtual Int_t PostReconstructioner();//gets Reconstructioner from file and adds it to Run Reconstructioner
    virtual Int_t PostTracker();//gets tracker from file and adds it to Run Tracker
    
    void Clean(const TString& name);
    
    TString       GetUnixDir();
    TObject*      GetDetectorData(const char* name){return GetDetectorDataFolder()->FindObject(name);}
    TObject**     GetDetectorDataRef(TObject* obj);
    
    Bool_t        CheckReload(const TFile* file, const TString& basefilename);//checks if we have to reload given file
    /**********************************************/
    /************    PROTECTED      ***************/
    /*********        D A T A          ************/
    /**********************************************/
   
    TString       fDetectorName;
    
    TString       fHitsFileName;  //name of file with hits
    TString       fSDigitsFileName;//name of file with SDigits 
    TString       fDigitsFileName;//name of file with Digits
    TString       fRecPointsFileName;//name of file with Recontructed Points
    TString       fTracksFileName;//name of file with Tracks
    
    TFile*        fHitsFile;    //!pointer to file with hits
    TFile*        fSDigitsFile; //!pointer to file with SDigits
    TFile*        fDigitsFile; //!pointer to file with Digits
    TFile*        fRecPointsFile; //!pointer to file with Recontructed Points
    TFile*        fTracksFile; //!pointer to file with Tracks
   
    TDirectory*   fHitsDir;    //!pointer to Dir with current event in Hits File
    TDirectory*   fSDigitsDir; //!pointer to Dir with current event in SDigits File
    TDirectory*   fDigitsDir; //!pointer to Dir with current event in Digits File
    TDirectory*   fRecPointsDir; //!pointer to Dir with current event in Recontructed Points File
    TDirectory*   fTracksDir; //!pointer to Dir with current event in Tracks File

//these variables were introduced because if TFile is created with "recreated" stored option is "CREATE"
//and we need to remeber this (Max events per file functionality)
    TString       fHitsFileOption;//Hits File Option in open
    TString       fSDigitsFileOption;//
    TString       fDigitsFileOption;//
    TString       fRecPointsFileOption;//
    TString       fTracksFileOption;//
    

    TFolder*      fModuleFolder;      //!Folder that contains the modules 
    TFolder*      fPrimariesFolder;   //!Folder that contains the Primary Particles 

    TFolder*      fEventFolder; //! Folder with data that changes from event to event, even in single run
    TFolder*      fDataFolder;  //!  Folder with data (hits, sdigits, etc, grouped in folders named like detectors
    TFolder*      fDetectorDataFolder;//!Folder that contains the detector data
    
    TFolder*      fTasksFolder;       //!Folder that contains the Tasks (sdigitizer, digitizer, reconstructioner)
    TFolder*      fQAFolder;          //!Folder that contains the QA objects


    TString       fHitsContainerName;//name of conatiner (TREE) for hits - standard TreeH
    TString       fDigitsContainerName;//name of conatiner (TREE) for digits - standard  TreeD
    TString       fSDigitsContainerName;//name of conatiner (TREE) for Sdigits - standard TreeS
    TString       fRecPointsContainerName;//name of conatiner (TREE) for Rec Points - standard TreeR
    TString       fTracksContainerName;//name of conatiner (TREE) for Tracks 
    
    static const TString   fgkDefaultHitsContainerName;//default name of conatiner (TREE) for hits
    static const TString   fgkDefaultDigitsContainerName;//default name of conatiner (TREE) for digits
    static const TString   fgkDefaultSDigitsContainerName;//default name of conatiner (TREE) for Sdigits
    static const TString   fgkDefaultRecPointsContainerName;//default name of conatiner (TREE) for Rec Points
    static const TString   fgkDefaultTracksContainerName;//default name of conatiner (TREE) for Tracks
    
    
    static const TString   fgkLoaderBaseName;//base name of Loader: fDetectorName+fgkLoaderBaseName. i.e. PHOSLoader

    
   private:
    //descendant classes should
    //use protected interface methods to access these folders
    

    /**********************************************/
    /***********     P U B L I C     **************/
    /*********       S T A T I C       ************/
    /*********         METHODS         ************/
    /*********     They are used by    ************/
    /*********** AliRunLoader as well**************/
    /**********************************************/
   public:
    static TDirectory*    ChangeDir(TFile* file, Int_t eventno); //changes the root directory in "file" to directory corresponing to eventno
    static Bool_t         TestFileOption(Option_t* opt);
    const TString SetFileOffset(const TString& fname);//adds the proper number before .root

    ClassDef(AliLoader,1)
 };
/******************************************************************/
/************************ I N L I N E S ***************************/
/******************************************************************/

inline TFolder* AliLoader::GetDetectorDataFolder()
 {
 //helper function which returns the folder of name "name" in data folder
   if(!fDetectorDataFolder)
    {
      fDetectorDataFolder = dynamic_cast<TFolder*>(GetDataFolder()->FindObject(fDetectorName.Data()));
      if(!fDetectorDataFolder)
       {
         Fatal("GetDetectorDataFolder",
               "Can not find folder %s in folder %s. Aborting",
               fDetectorName.Data(),GetDataFolder()->GetName());
         return 0x0;
       }
      
    }
   return fDetectorDataFolder;
 }

#endif
