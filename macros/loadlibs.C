void loadlibs () 
{
  gSystem->Load("libMC");

  // libraries required by EVGEN
  // (commented libraries are already loaded in prevoius sequence)
  //gSystem->Load("libMC");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libSTEER");
  gSystem->Load("$(ROOTSYS)/lib/libEG"); 
  gSystem->Load("$(ROOTSYS)/lib/libEGPythia6");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libdummypythia6");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libdummyhijing");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTHijing");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libdummyHBTP");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTHbtp");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libdummymevsim");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTMevSim");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libEVGEN");

  gSystem->Load("$(G3INSTALL)/lib/tgt_$(ALICE_TARGET)/libminicern");
  gSystem->Load("$(ROOTSYS)/lib/libPhysics");

  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libCONTAINERS");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libFMD");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libMUON");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libPHOS");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libPMD");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libRICH");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libSTRUCT");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTOF");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTPC");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libTRD");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libZDC");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libITS");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libCASTOR");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libSTART");
  gSystem->Load("$(ALICE_ROOT)/lib/tgt_$(ALICE_TARGET)/libEMCAL");
}
